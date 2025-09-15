#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Check if empty or not */
static inline bool q_empty(struct list_head *head)
{
    return head->next == head;
}

/* copy the string to the caller's buffer*/
static inline void copy_out(char *dst, size_t n, const char *src)
{
    if (!dst || n == 0)
        return;
    if (!src) {
        dst[0] = '\0';
        return;
    }
    strncpy(dst, src, n - 1);
    dst[n - 1] = '\0';
}


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *pos, *n;
    list_for_each_safe(pos, n, head) {
        element_t *e = list_entry(pos, element_t, list);
        list_del(pos);         // remove first
        q_release_element(e);  // then free e-> value and e
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *e = malloc(sizeof(*e));
    if (!e)
        return false;

    char *dup = s ? strdup(s) : strdup("");
    if (!dup) {
        free(e);
        return false;
    }
    e->value = dup;
    INIT_LIST_HEAD(&e->list);
    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *e = malloc(sizeof(*e));
    if (!e)
        return false;

    char *dup = s ? strdup(s) : strdup("");
    if (!dup) {
        free(e);
        return false;
    }
    e->value = dup;
    INIT_LIST_HEAD(&e->list);
    list_add_tail(&e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || q_empty(head))
        return NULL;

    struct list_head *node = head->next;
    element_t *e = list_entry(node, element_t, list);
    list_del(node);
    copy_out(sp, bufsize, e->value);
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || q_empty(head))
        return NULL;

    struct list_head *node = head->prev;
    element_t *e = list_entry(node, element_t, list);
    list_del(node);
    copy_out(sp, bufsize, e->value);
    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || q_empty(head))
        return 0;

    int cnt = 0;
    struct list_head *pos;
    list_for_each(pos, head)
        cnt++;
    return cnt;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
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
