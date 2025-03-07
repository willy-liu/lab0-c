#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
    }
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *entry, *safe;
    list_for_each_safe (entry, safe, head) {
        q_release_element(list_entry(entry, element_t, list));
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;

    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return false;
    }

    new_element->list.next = head->next;
    new_element->list.prev = head;
    head->next->prev = &new_element->list;
    head->next = &new_element->list;

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;

    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return false;
    }

    new_element->list.next = head;
    new_element->list.prev = head->prev;
    head->prev->next = &new_element->list;
    head->prev = &new_element->list;

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;

    struct list_head *node = head->next;
    element_t *element = list_entry(node, element_t, list);

    head->next = node->next;
    node->next->prev = head;

    if (sp && bufsize > 0) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->prev == head)
        return NULL;

    struct list_head *node = head->prev;
    element_t *element = list_entry(node, element_t, list);

    head->prev = node->prev;
    node->prev->next = head;

    if (sp && bufsize > 0) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int count = 0;
    struct list_head *current = head->next;
    while (current != head) {
        count++;
        current = current->next;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || head->next == head)
        return false;

    // solution:
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/solutions/1612140/one-pass-slow-and-fast
    struct list_head *slow = head->next;
    struct list_head *fast = head->next;

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    slow->prev->next = slow->next;
    slow->next->prev = slow->prev;

    element_t *element = list_entry(slow, element_t, list);
    free(element->value);
    free(element);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || head->next == head)
        return false;

    // q_sort(head, false);  // 先排序

    struct list_head *current = head->next;
    bool has_duplicates = false;

    while (current != head && current->next != head) {
        element_t *element = list_entry(current, element_t, list);
        element_t *next_element = list_entry(current->next, element_t, list);

        if (strcmp(element->value, next_element->value) == 0) {
            has_duplicates = true;
            struct list_head *dup = current->next;
            current->next = dup->next;
            dup->next->prev = current;
            free(next_element->value);
            free(next_element);
        } else {
            if (has_duplicates) {
                struct list_head *dup = current;
                current = current->prev;
                current->next = dup->next;
                dup->next->prev = current;
                free(element->value);
                free(element);
                has_duplicates = false;
            }
            current = current->next;
        }
    }

    if (has_duplicates) {
        struct list_head *dup = current;
        current = current->prev;
        current->next = dup->next;
        dup->next->prev = current;
        element_t *element = list_entry(dup, element_t, list);
        free(element->value);
        free(element);
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || head->next == head)
        return;

    struct list_head *current = head->next;

    while (current != head && current->next != head) {
        struct list_head *next = current->next;

        // Swap current and next
        current->prev->next = next;
        next->prev = current->prev;
        current->next = next->next;
        next->next->prev = current;
        next->next = current;
        current->prev = next;

        // Move to the next pair
        current = current->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || head->next == head)
        return;

    struct list_head *current = head;
    do {
        struct list_head *temp = current->next;
        current->next = current->prev;
        current->prev = temp;
        current = temp;
    } while (current != head);
}

static void q_reverse_between(struct list_head *start, struct list_head *end)
{
    struct list_head *prev = start->prev;
    struct list_head *current = start;
    struct list_head *next;

    while (current != end) {
        next = current->next;
        current->next = current->prev;
        current->prev = next;
        current = next;
    }

    next = end->next;

    end->next = end->prev;
    end->prev = next;

    prev->next = end;
    start->next = next;
    next->prev = start;
    end->prev = prev;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || head->next == head || k <= 1)
        return;

    // Keep track of the node before the current group
    struct list_head *group_prev = head;
    while (group_prev->next != head) {
        struct list_head *start = group_prev->next;
        struct list_head *current = start;
        int count = 0;

        // Count k nodes to check if a group of k exists
        while (current != head && count < k) {
            current = current->next;
            count++;
        }

        // Less than k elements remaining.
        if (count < k) {
            return;
        }
        // 'current' is now at the (k+1)-th node or head
        struct list_head *end = current->prev;
        q_reverse_between(start, end);  // Reverse the k-group

        group_prev = start;
    }
}

/* Sort elements of queue in ascending/descending order */
/* Use insertion sort now */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || head->next == head || head->next->next == head)
        return;

    struct list_head *current = head->next->next;
    struct list_head *pos;

    while (current != head) {
        pos = current->prev;
        while (
            pos != head &&
            (descend
                 ? strcmp(list_entry(pos, element_t, list)->value,
                          list_entry(current, element_t, list)->value) < 0
                 : strcmp(list_entry(pos, element_t, list)->value,
                          list_entry(current, element_t, list)->value) > 0)) {
            pos = pos->prev;
        }
        // Remove current from the list
        current->prev->next = current->next;
        current->next->prev = current->prev;

        // Insert current after pos
        current->next = pos->next;
        current->prev = pos;
        pos->next->prev = current;
        pos->next = current;

        // Move to the next element
        current = current->next;
    }
}


/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || head->next == head)
        return 0;

    // From last element to first element
    struct list_head *cur = head->prev;
    const element_t *cur_elem = list_entry(cur, element_t, list);
    const char *min_val = cur_elem->value;  // keep the last element

    cur = cur->prev;
    while (cur != head) {
        // Avoid the last element is removed
        struct list_head *temp = cur->prev;
        element_t *entry = list_entry(cur, element_t, list);
        if (strcmp(entry->value, min_val) > 0) {
            cur->prev->next = cur->next;
            cur->next->prev = cur->prev;
            free(entry->value);
            free(entry);
        } else {
            min_val = entry->value;
        }
        cur = temp;
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || head->next == head)
        return 0;

    // From last element to first element
    struct list_head *cur = head->prev;
    const element_t *cur_elem = list_entry(cur, element_t, list);
    const char *max_val = cur_elem->value;  // keep the last element


    cur = cur->prev;
    while (cur != head) {
        // Avoid the last element is removed
        struct list_head *temp = cur->prev;
        element_t *entry = list_entry(cur, element_t, list);
        if (strcmp(entry->value, max_val) < 0) {
            cur->prev->next = cur->next;
            cur->next->prev = cur->prev;
            free(entry->value);
            free(entry);
        } else {
            max_val = entry->value;
        }
        cur = temp;
    }
    return q_size(head);
}


/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
static struct list_head *q_merge_two(struct list_head *head1,
                                     struct list_head *head2)
{
    struct list_head *current = head1;
    struct list_head *current1 = head1->next;
    struct list_head *current2 = head2->next;

    while (current1 != head1 && current2 != head2) {
        const element_t *element1 = list_entry(current1, element_t, list);
        const element_t *element2 = list_entry(current2, element_t, list);

        if (strcmp(element1->value, element2->value) < 0) {
            current->next = current1;
            current1->prev = current;
            current1 = current1->next;
        } else {
            current->next = current2;
            current2->prev = current;
            current2 = current2->next;
        }
        current = current->next;
    }

    while (current1 != head1) {
        current->next = current1;
        current1->prev = current;
        current1 = current1->next;
        current = current->next;
    }

    while (current2 != head2) {
        current->next = current2;
        current2->prev = current;
        current2 = current2->next;
        current = current->next;
    }

    current->next = head1;
    head1->prev = current;

    return head1;
}

int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || head->next == head)
        return 0;

    struct list_head *current_queue = head->next;
    struct list_head *next_queue = current_queue->next;

    while (next_queue != head) {
        queue_contex_t *current_context =
            list_entry(current_queue, queue_contex_t, chain);
        ;
        queue_contex_t *next_context =
            list_entry(next_queue, queue_contex_t, chain);
        ;

        struct list_head *merged_queue =
            q_merge_two(current_context->q, next_context->q);

        next_context->q->next = next_context->q;
        next_context->q->prev = next_context->q;
        current_context->q = merged_queue;

        next_queue = next_queue->next;
    }

    return q_size(list_entry(current_queue, queue_contex_t, chain)->q);
}
