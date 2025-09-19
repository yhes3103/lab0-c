#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* *
 * Check if empty or not
 * Simply detect whether if head points to itself, avoiding NULL pointer
 * comparison. Furthermore, when inserting or deleting, no more extra cases such
 * as "first node, last node" needs to be aware of, just regarded every node as
 * in front/behind of head. Check q_new() and INIT_LIST_HEAD().
 */
static inline bool q_empty(struct list_head *head)
{
    return head->next == head;
}

/* copy the string to the caller's buffer when removing*/
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
/**
 * #define list_entry(node, type, member) container_of(node, type, member)
 * container of is a LINUX MACRO shows as follow

 * #define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
 *
 * calculate the real address of value(char*) by calculating the offset of
 others variable
 *
 */

/**
 * element_t (整個結構體)
   +--------------------+
   | value  (char*)     |
   | list   (list_head) |  <--- pos 指到這裡
   +--------------------+

 *  你手上只有 pos（指向 list 成員）
 *  list_entry(pos, element_t, list) 會「往回算 offset」，得到這個 element_t
 的起點地址
 *  結果就是 e，你就能用 e->value 拿到字串
 */
void q_free(struct list_head *head)
{
    if (!head)  // head malloc failed
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

    // copy s to dup, if s is NULL, copy " " to dup
    char *dup = s ? strdup(s) : strdup("");
    if (!dup) {
        free(e);
        return false;
    }
    e->value = dup;

    // initialize the element, points to itself
    INIT_LIST_HEAD(&e->list);

    // insert the element e to next of the head
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

    // Remember that head is a dummy head, the real firts element is head->next
    struct list_head *node = head->next;

    // cheange list haed* pointer to element_t* pointer
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
    if (!head || q_empty(head))
        return false;

    struct list_head *slow = head->next, *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    element_t *e = list_entry(slow, element_t, list);
    list_del(slow);
    q_release_element(e);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || q_empty(head))
        return false;

    // sort the list first
    q_sort(head, false);

    bool deleted = false;
    struct list_head *cur = head->next;
    while (cur != head && cur->next != head) {
        element_t *e = list_entry(cur, element_t, list);
        element_t *n = list_entry(cur->next, element_t, list);

        if (strcmp(e->value, n->value) == 0) {
            char *dup_val = e->value;
            while (cur != head) {
                element_t *tmp = list_entry(cur, element_t, list);
                if (strcmp(tmp->value, dup_val) != 0)
                    break;
                struct list_head *next = cur->next;
                list_del(&tmp->list);
                q_release_element(tmp);
                cur = next;
            }
            deleted = true;
        } else {
            cur = cur->next;
        }
    }
    return deleted;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || q_empty(head) || head->next->next == head)
        return;

    struct list_head *cur = head->next;
    while (cur != head && cur->next != head) {
        struct list_head *first = cur;
        struct list_head *second = cur->next;
        cur = second->next;

        /* swap first and second */
        list_del(first);
        list_del(second);
        list_add(second, first->prev);
        list_add(first, second);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || q_empty(head))
        return;

    struct list_head *cur = head, *tmp;
    do {
        tmp = cur->next;
        cur->next = cur->prev;
        cur->prev = tmp;
        cur = tmp;
    } while (cur != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || q_empty(head) || k <= 1)
        return;

    struct list_head *cur = head->next;
    while (cur != head) {
        /* check if there are at least k nodes */
        struct list_head *check = cur;
        int count = 0;
        for (; count < k && check != head; count++, check = check->next)
            ;
        if (count < k)
            break;

        /* reverse this block of k nodes */
        struct list_head *prev = cur->prev, *next = check, *p = cur;
        struct list_head *tail = cur;
        for (int i = 0; i < k; i++) {
            struct list_head *tmp = p->next;
            p->next = p->prev;
            p->prev = tmp;
            p = tmp;
        }
        prev->next = p->prev;
        p->prev->prev = prev;
        tail->next = next;
        next->prev = tail;
        cur = next;
    }
}

/* Sort elements of queue in ascending/descending order */

void q_sort(struct list_head *head, bool descend)
{
    if (!head || q_empty(head) || head->next->next == head)
        return;

    /* cut the circular list to make it a  linked list */
    struct list_head *list = head->next;
    head->prev->next = NULL;

    /* bottom-up merge sort */
    int insize = 1;
    while (1) {
        struct list_head *p = list;
        list = NULL;
        struct list_head *tail = NULL;
        int nmerges = 0;

        while (p) {
            nmerges++;
            struct list_head *q = p;
            int psize = 0;
            for (int i = 0; i < insize; i++) {
                psize++;
                q = q->next;
                if (!q)
                    break;
            }

            int qsize = insize;
            while (psize > 0 || (qsize > 0 && q)) {
                struct list_head *e;
                if (psize == 0) {
                    e = q;
                    q = q->next;
                    qsize--;
                } else if (qsize == 0 || !q) {
                    e = p;
                    p = p->next;
                    psize--;
                } else {
                    element_t *ep = list_entry(p, element_t, list);
                    element_t *eq = list_entry(q, element_t, list);
                    int cmp = strcmp(ep->value, eq->value);

                    if ((descend && cmp > 0) || (!descend && cmp <= 0)) {
                        e = p;
                        p = p->next;
                        psize--;
                    } else {
                        e = q;
                        q = q->next;
                        qsize--;
                    }
                }

                if (tail)
                    tail->next = e;
                else
                    list = e;

                e->prev = tail;
                tail = e;
            }
            p = q;
        }

        tail->next = NULL;

        if (nmerges <= 1)
            break;

        insize *= 2;
    }

    /* rebuild circular list */
    INIT_LIST_HEAD(head);
    struct list_head *cur = list;
    while (cur) {
        struct list_head *next = cur->next;
        list_add_tail(cur, head);
        cur = next;
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || q_empty(head))
        return 0;

    struct list_head *cur = head->prev;
    char *max_val = list_entry(cur, element_t, list)->value;
    cur = cur->prev;
    while (cur != head) {
        struct list_head *tmp = cur->prev;
        element_t *e = list_entry(cur, element_t, list);
        if (strcmp(e->value, max_val) < 0) {
            list_del(cur);
            q_release_element(e);
        } else {
            max_val = e->value;
        }
        cur = tmp;
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || q_empty(head))
        return 0;

    struct list_head *cur = head->prev;
    char *min_val = list_entry(cur, element_t, list)->value;
    cur = cur->prev;
    while (cur != head) {
        struct list_head *tmp = cur->prev;
        element_t *e = list_entry(cur, element_t, list);
        if (strcmp(e->value, min_val) > 0) {
            list_del(cur);
            q_release_element(e);
        } else {
            min_val = e->value;
        }
        cur = tmp;
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || q_empty(head))
        return 0;

    struct list_head *pos, *n;
    LIST_HEAD(all);

    list_for_each_safe(pos, n, head) {
        queue_contex_t *ctx = list_entry(pos, queue_contex_t, chain);
        list_splice_tail(ctx->q, &all);
        INIT_LIST_HEAD(ctx->q);
    }

    q_sort(&all, descend);
    list_splice(&all, head);
    return q_size(head);
}
