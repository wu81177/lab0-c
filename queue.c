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
    if (!head)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list)
        q_release_element(entry);
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;
    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }
    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return false;
    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }
    list_add_tail(&e->list, head);
    return true;
}
/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
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
    if (!head || list_empty(head))
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
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    if (head->prev == head->next)
        return true;

    element_t *curr, *safe;
    bool flag = false;
    list_for_each_entry_safe (curr, safe, head, list) {
        if (curr->list.next != head && !strcmp(curr->value, safe->value)) {
            flag = 1;
            list_del(&curr->list);
            q_release_element(curr);
        } else if (flag) {
            list_del(&curr->list);
            q_release_element(curr);
            flag = 0;
        }
    }
    return true;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || head->prev == head->next)
        return;
    struct list_head *curr, *safe;
    list_for_each_safe (curr, safe, head)
        list_move(curr, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || head->prev == head->next)
        return;

    struct list_head *curr, *safe, *group_prev;
    int count = 0;
    group_prev = head;

    list_for_each_safe (curr, safe, head) {
        count++;
        if (count < k)
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
    if (!head)
        return;
    q_reverseK(head, 2);
}

static void merge_two(struct list_head *l1, struct list_head *l2, bool descend)
{
    if (!l1 || !l2)
        return;
    LIST_HEAD(tmp_head);

    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *ele_l1 = list_first_entry(l1, element_t, list);
        element_t *ele_l2 = list_first_entry(l2, element_t, list);

        if (descend == (strcmp(ele_l1->value, ele_l2->value) < 0))
            list_move_tail(l2->next, &tmp_head);
        else
            list_move_tail(l1->next, &tmp_head);
    }

    list_splice(&tmp_head, l2);
    list_splice_init(l2, l1);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || head->prev == head->next)
        return;
    struct list_head *mid;
    struct list_head *left = head;
    struct list_head *right = head;
    do {
        left = left->next;
        right = right->prev;
    } while (left != right && left->next != right);

    mid = left;

    LIST_HEAD(toMerge);
    list_cut_position(&toMerge, mid, head->prev);

    q_sort(head, descend);
    q_sort(&toMerge, descend);
    merge_two(head, &toMerge, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || head->prev == head->next)
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
    if (!head || head->prev == head->next)
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
    queue_contex_t *first_q = list_first_entry(head, queue_contex_t, chain);
    queue_contex_t *curr, *safe;
    list_for_each_entry_safe (curr, safe, head, chain) {
        if (curr == first_q)
            continue;
        first_q->size += curr->size;
        list_splice_init(curr->q, first_q->q);
        // merge_two(first_q->q,curr->q,descend);
    }
    q_sort(first_q->q, descend);
    return first_q->size;
}

// void q_shuffle(struct list_head *head)
// {
//     if (!head || head->next == head->prev)
//         return;
//     for (int len = q_size(head) - 1; len > 0; len--) {
//         int random = 1 + rand() % len;
//         struct list_head *old = NULL, *new = NULL;
//         int j = 1;
//         struct list_head *pos;
//         list_for_each (pos, head) {
//             if (j == random) {
//                 old = pos;
//             }
//             if (j == len + 1) {
//                 new = pos;
//                 break;
//             }
//             j++;
//         }
//         struct list_head *pre_new = new->prev;
//         struct list_head *pre_old = old->prev;
//         if (old->next == new) {
//             list_del(old);
//             list_add(old, new);
//         } else {
//             list_del(new);
//             list_del(old);
//             list_add(new, pre_old);
//             list_add(old, pre_new);
//         }
//     }
// }