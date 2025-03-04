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
    if (!head)
        return NULL;
    head->next = head;
    head->prev = head;
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *current = head->next;
    while (current != head) {
        element_t *element = list_entry(current, element_t, list);
        current = current->next;
        if (element->value) {
            free(element->value);
        }
        free(element);
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
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
