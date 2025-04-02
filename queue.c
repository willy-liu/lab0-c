#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

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
    list_for_each_safe(entry, safe, head) {
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

    list_add(&new_element->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;

    element_t *element = list_entry(head->next, element_t, list);

    list_del(&element->list);

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

    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int count = 0;
    struct list_head *node;

    list_for_each(node, head)
        count++;

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

    q_release_element(list_entry(slow, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || head->next == head)
        return false;

    struct list_head *node, *safe;
    bool last_duplicate = false;

    list_for_each_safe(node, safe, head) {
        element_t *element = list_entry(node, element_t, list);
        const element_t *element_safe = list_entry(safe, element_t, list);

        if (safe != head && strcmp(element->value, element_safe->value) == 0) {
            last_duplicate = true;
            list_del(node);
            q_release_element(element);
        } else if (last_duplicate) {
            last_duplicate = false;
            list_del(node);
            q_release_element(element);
        }
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
        // Swap current and next
        list_move(current, current->next);

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

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_is_singular(head) || k <= 1)
        return;

    struct list_head *node, *safe;
    int count = 0;

    list_for_each_safe(node, safe, head) {
        count++;
        if (count % k == 0) {
            while (--count) {
                list_move_tail(node->prev, safe);
            }
        }
    }
}

struct list_head *merge(struct list_head *left,
                        struct list_head *right,
                        bool descend)
{
    struct list_head merged;
    INIT_LIST_HEAD(&merged);
    struct list_head *cur = &merged;

    while (left && right) {
        const element_t *left_elem = list_entry(left, element_t, list);
        const element_t *right_elem = list_entry(right, element_t, list);

        int cmp = strcmp(left_elem->value, right_elem->value);
        if (descend)
            cmp = -cmp;

        if (cmp <= 0) {
            cur->next = left;
            left = left->next;
        } else {
            cur->next = right;
            right = right->next;
        }
        cur = cur->next;
    }
    if (left)
        cur->next = left;
    if (right)
        cur->next = right;

    return merged.next;
}

struct list_head *merge_sort(struct list_head *head, bool descend)
{
    if (!head || !head->next)
        return head;

    struct list_head *slow = head, *mid;
    for (const struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next) {
        slow = slow->next;
    }
    mid = slow->next;
    slow->next = NULL;

    struct list_head *left = merge_sort(head, descend);
    struct list_head *right = merge_sort(mid, descend);

    return merge(left, right, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_is_singular(head))
        return;
    head->prev->next = NULL;  // break the circular list
    head->next = merge_sort(head->next, descend);
    struct list_head *cur = head;
    while (cur->next) {  // reconnect prev pointer
        cur->next->prev = cur;
        cur = cur->next;
    }
    cur->next = head;
    head->prev = cur;
    return;
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

int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))
        return list_entry(head->next, queue_contex_t, chain)->size;

    struct list_head merged;
    INIT_LIST_HEAD(&merged);

    while (1) {
        queue_contex_t *entry, *best_queue = NULL;
        struct list_head *best_node = NULL;

        list_for_each_entry(entry, head, chain) {
            if (list_empty(entry->q))
                continue;

            if (!best_node) {
                best_node = entry->q->next;
                best_queue = entry;
            } else if ((strcmp(
                            list_entry(entry->q->next, element_t, list)->value,
                            list_entry(best_node, element_t, list)->value) <
                        0) ^
                       descend) {
                best_node = entry->q->next;
                best_queue = entry;
            }
        }

        if (!best_node)
            break;

        if (list_is_singular(head)) {
            list_splice_tail_init(best_queue->q, &merged);
            best_queue->size = 0;
            break;
        }

        list_move_tail(best_node, &merged);
        best_queue->size--;
    }

    queue_contex_t *first_entry = list_entry(head->next, queue_contex_t, chain);
    list_splice_init(&merged, first_entry->q);
    first_entry->size = q_size(first_entry->q);

    return first_entry->size;
}