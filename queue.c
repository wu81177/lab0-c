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
    struct list_head *new_head = malloc(sizeof(struct list_head));
    if (new_head) {
        INIT_LIST_HEAD(new_head);
        return new_head;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list)
        q_release_element(entry);
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;
    e->value = strdup(s);
    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;
    e->value = strdup(s);
    list_add_tail(&e->list, head);
    return true;
}
/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;

    element_t *e = container_of(head->next, element_t, list);

    if (e->value && sp && bufsize > 0) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(&e->list);
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;

    element_t *e = container_of(head->prev, element_t, list);

    if (e->value && sp && bufsize > 0) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(&e->list);
    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *curr;

    list_for_each (curr, head) {
        len++;
    }
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *forward = head->next;
    struct list_head *backward = head->prev;
    while (forward != backward && forward->next != backward) {
        forward = forward->next;
        backward = backward->prev;
    }
    element_t *e = container_of(forward, element_t, list);
    list_del(forward);
    q_release_element(e);
    free(forward);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return false;
    }
    struct list_head *outer, *inner;
    element_t *outer_ele, *inner_ele;
    list_for_each (outer, head) {
        // list_for_each(inner, head){
        for (inner = (outer)->next; inner != (head); inner = inner->next) {
            outer_ele = container_of(outer, element_t, list);
            inner_ele = container_of(inner, element_t, list);
            if (strcmp(outer_ele->value, inner_ele->value) == 0) {
                list_del(inner);
                q_release_element(inner_ele);
                free(inner);
            }
        }
    }
    return true;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *curr, *safe;
    list_for_each_safe (curr, safe, head)
        list_move(curr, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    struct list_head *curr, *safe, *group_prev;
    int count = 0;
    group_prev = head;

    list_for_each_safe (curr, safe, head) {
        count++;
        if (count <= k)
            continue;
        LIST_HEAD(tmp_q);
        count = 0;

        list_cut_position(&tmp_q, group_prev, curr);
        q_reverse(&tmp_q);
        list_splice(&tmp_q, group_prev);

        group_prev = safe->prev;
    }
}


/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    int count = 1;
    element_t *curr_ele = list_last_entry(head, element_t, list);
    while (curr_ele->list.prev != head) {
        element_t *prev_ele = list_last_entry(&curr_ele->list, element_t, list);
        if (strcmp(prev_ele->value, curr_ele->value) > 0) {
            list_del(&prev_ele->list);
            q_release_element(prev_ele);
            continue;
        }
        count++;
        curr_ele = prev_ele;
    }
    return count;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    int count = 1;
    element_t *curr_ele = list_last_entry(head, element_t, list);
    while (curr_ele->list.prev != head) {
        element_t *prev_ele = list_last_entry(&curr_ele->list, element_t, list);
        if (strcmp(prev_ele->value, curr_ele->value) < 0) {
            list_del(&prev_ele->list);
            q_release_element(prev_ele);
            continue;
        }
        count++;
        curr_ele = prev_ele;
    }
    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *first_q = list_entry(&head->next, queue_contex_t, chain);
    queue_contex_t *curr, *safe;
    list_for_each_entry_safe (curr, safe, head, chain) {
        if (curr == first_q)
            continue;
        first_q->size += curr->size;
        list_splice_init(curr->q, first_q->q);
        list_del_init(&curr->chain);
    }
    q_sort(first_q->q, descend);
    return first_q->size;
}
