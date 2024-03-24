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

// static void merge_two(struct list_head *l1, struct list_head *l2, bool
// descend)
// {
//     if (!l1 || !l2)
//         return;
//     LIST_HEAD(tmp_head);

//     while (!list_empty(l1) && !list_empty(l2)) {
//         element_t *ele_l1 = list_first_entry(l1, element_t, list);
//         element_t *ele_l2 = list_first_entry(l2, element_t, list);

//         if (descend == (strcmp(ele_l1->value, ele_l2->value) < 0))
//             list_move_tail(l2->next, &tmp_head);
//         else
//             list_move_tail(l1->next, &tmp_head);
//     }

//     list_splice(&tmp_head, l2);
//     list_splice_init(l2, l1);
// }

/* Sort elements of queue in ascending/descending order */
// void q_sort(struct list_head *head, bool descend)
// {
//     if (!head || head->prev == head->next)
//         return;
//     struct list_head *mid;
//     struct list_head *left = head;
//     struct list_head *right = head;
//     do {
//         left = left->next;
//         right = right->prev;
//     } while (left != right && left->next != right);

//     mid = left;

//     LIST_HEAD(toMerge);
//     list_cut_position(&toMerge, mid, head->prev);

//     q_sort(head, descend);
//     q_sort(&toMerge, descend);
//     merge_two(head, &toMerge, descend);
// }

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

static struct list_head *merge(struct list_head *a, struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        element_t *ela = list_entry(a, element_t, list);
        element_t *elb = list_entry(b, element_t, list);
        if (strcmp(ela->value, elb->value) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

/*
 * Combine final list merge with restoration of standard doubly-linked
 * list structure.  This approach duplicates code from merge(), but
 * runs faster than the tidier alternatives of either a separate final
 * prev-link restoration pass, or maintaining the prev links
 * throughout.
 */
static void merge_final(struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        element_t *ela = list_entry(a, element_t, list);
        element_t *elb = list_entry(b, element_t, list);
        if (strcmp(ela->value, elb->value) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        /*
         * If the merge is highly unbalanced (e.g. the input is
         * already sorted), this loop may run many iterations.
         * Continue callbacks to the client even though no
         * element comparison is needed, so the client's cmp()
         * routine can invoke cond_resched() periodically.
         */
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

/**
 * list_sort - sort a list
 * @priv: private data, opaque to list_sort(), passed to @cmp
 * @head: the list to sort
 * @cmp: the elements comparison function
 *
 * The comparison function @cmp must return > 0 if @a should sort after
 * @b ("@a > @b" if you want an ascending sort), and <= 0 if @a should
 * sort before @b *or* their original order should be preserved.  It is
 * always called with the element that came first in the input in @a,
 * and list_sort is a stable sort, so it is not necessary to distinguish
 * the @a < @b and @a == @b cases.
 *
 * This is compatible with two styles of @cmp function:
 * - The traditional style which returns <0 / =0 / >0, or
 * - Returning a boolean 0/1.
 * The latter offers a chance to save a few cycles in the comparison
 * (which is used by e.g. plug_ctx_cmp() in block/blk-mq.c).
 *
 * A good way to write a multi-word comparison is::
 *
 *	if (a->high != b->high)
 *		return a->high > b->high;
 *	if (a->middle != b->middle)
 *		return a->middle > b->middle;
 *	return a->low > b->low;
 *
 *
 * This mergesort is as eager as possible while always performing at least
 * 2:1 balanced merges.  Given two pending sublists of size 2^k, they are
 * merged to a size-2^(k+1) list as soon as we have 2^k following elements.
 *
 * Thus, it will avoid cache thrashing as long as 3*2^k elements can
 * fit into the cache.  Not quite as good as a fully-eager bottom-up
 * mergesort, but it does use 0.2*n fewer comparisons, so is faster in
 * the common case that everything fits into L1.
 *
 *
 * The merging is controlled by "count", the number of elements in the
 * pending lists.  This is beautifully simple code, but rather subtle.
 *
 * Each time we increment "count", we set one bit (bit k) and clear
 * bits k-1 .. 0.  Each time this happens (except the very first time
 * for each bit, when count increments to 2^k), we merge two lists of
 * size 2^k into one list of size 2^(k+1).
 *
 * This merge happens exactly when the count reaches an odd multiple of
 * 2^k, which is when we have 2^k elements pending in smaller lists,
 * so it's safe to merge away two lists of size 2^k.
 *
 * After this happens twice, we have created two lists of size 2^(k+1),
 * which will be merged into a list of size 2^(k+2) before we create
 * a third list of size 2^(k+1), so there are never more than two pending.
 *
 * The number of pending lists of size 2^k is determined by the
 * state of bit k of "count" plus two extra pieces of information:
 *
 * - The state of bit k-1 (when k == 0, consider bit -1 always set), and
 * - Whether the higher-order bits are zero or non-zero (i.e.
 *   is count >= 2^(k+1)).
 *
 * There are six states we distinguish.  "x" represents some arbitrary
 * bits, and "y" represents some arbitrary non-zero bits:
 * 0:  00x: 0 pending of size 2^k;           x pending of sizes < 2^k
 * 1:  01x: 0 pending of size 2^k; 2^(k-1) + x pending of sizes < 2^k
 * 2: x10x: 0 pending of size 2^k; 2^k     + x pending of sizes < 2^k
 * 3: x11x: 1 pending of size 2^k; 2^(k-1) + x pending of sizes < 2^k
 * 4: y00x: 1 pending of size 2^k; 2^k     + x pending of sizes < 2^k
 * 5: y01x: 2 pending of size 2^k; 2^(k-1) + x pending of sizes < 2^k
 * (merge and loop back to state 2)
 *
 * We gain lists of size 2^k in the 2->3 and 4->5 transitions (because
 * bit k-1 is set while the more significant bits are non-zero) and
 * merge them away in the 5->2 transition.  Note in particular that just
 * before the 5->2 transition, all lower-order bits are 11 (state 3),
 * so there is one list of each smaller size.
 *
 * When we reach the end of the input, we merge all the pending
 * lists, from smallest to largest.  If you work through cases 2 to
 * 5 above, you can see that the number of elements we merge with a list
 * of size 2^k varies from 2^(k-1) (cases 3 and 5 when x == 0) to
 * 2^(k+1) - 1 (second merge of case 5 when x == 2^(k-1) - 1).
 */

void q_sort(struct list_head *head, bool descend)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    /*
     * Data structure invariants:
     * - All lists are singly linked and null-terminated; prev
     *   pointers are not maintained.
     * - pending is a prev-linked "list of lists" of sorted
     *   sublists awaiting further merging.
     * - Each of the sorted sublists is power-of-two in size.
     * - Sublists are sorted by size and age, smallest & newest at front.
     * - There are zero to two sublists of each size.
     * - A pair of pending sublists are merged as soon as the number
     *   of following pending elements equals their size (i.e.
     *   each time count reaches an odd multiple of that size).
     *   That ensures each later final merge will be at worst 2:1.
     * - Each round consists of:
     *   - Merging the two sublists selected by the highest bit
     *     which flips when count is incremented, and
     *   - Adding an element from the input as a size-1 sublist.
     */
    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (bits) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(b, a);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge(pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(head, pending, list);

    if (descend)
        q_reverse(head);
}