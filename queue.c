#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

static list_ele_t *element_new(char *s)
{
    size_t s_len = strlen(s);
    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (newh == NULL) {
        return newh;
    }

    newh->next = NULL;
    newh->value = NULL;

    if (s_len > 0) {
        newh->value = malloc(s_len + 1);
        if (newh->value == NULL) {
            free(newh);
            return NULL;
        }
        strncpy(newh->value, s, s_len);
        newh->value[s_len] = '\0';
    }

    return newh;
}

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL) {
        perror("failed to malloc");
    } else {
        q->head = NULL;
        q->tail = q->head;
        q->size = 0;
    }
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q == NULL) {
        return;
    }

    list_ele_t *next = NULL, *head = q->head;

    while (head != NULL) {
        free(head->value);
        next = head->next;
        free(head);
        head = next;
    }

    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (q == NULL) {
        return false;
    }

    list_ele_t *newh = element_new(s);
    if (newh == NULL) {
        return false;
    }

    newh->next = q->head;
    q->head = newh;

    if (newh->next == NULL) {
        q->tail = newh;
    }

    q->size++;

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (q == NULL) {
        return false;
    }
    list_ele_t *newh = element_new(s);
    if (newh == NULL) {
        return false;
    }

    if (q->tail == NULL) {
        q->tail = newh;
        q->head = q->tail;
    } else {
        q->tail->next = newh;
        q->tail = newh;
    }
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (q == NULL || q->head == NULL) {
        return false;
    }
    list_ele_t *head = q->head;
    q->head = q->head->next;

    q->size--;

    if (q->head == NULL) {
        q->tail = NULL;
    }
    if (head->value != NULL) {
        if (sp != NULL) {
            size_t size = (strlen(head->value) < bufsize - 1)
                              ? strlen(head->value)
                              : bufsize - 1;
            strncpy(sp, head->value, size);
            sp[size] = '\0';
        }
        free(head->value);
    }
    free(head);

    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q == NULL) {
        return 0;
    }
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q == NULL || q->size < 2) {
        return;
    }

    list_ele_t *head = q->head, *prev = NULL, *current = q->head, *next = NULL;
    while (current != NULL) {
        next = current->next;
        current->next = prev;

        prev = current;
        current = next;
    }

    q->head = prev;
    q->tail = head;
}

void swap(list_ele_t *e1, list_ele_t *e2)
{
    char *temp = e1->value;
    e1->value = e2->value;
    e2->value = temp;
}

list_ele_t *partition(list_ele_t *head, list_ele_t *tail)
{
    list_ele_t *pivot = tail, *low = NULL, *high = head;

    while (high != NULL && high != tail) {
        if (strcmp(high->value, pivot->value) < 0) {
            if (low == NULL) {
                low = head;
            } else {
                low = low->next;
            }
            swap(low, high);
        }
        high = high->next;
    }
    list_ele_t *prev = low;
    if (low == NULL) {
        low = head;
        prev = head;
    } else if (low->next != NULL) {
        low = low->next;
    }
    swap(low, pivot);
    return prev;
}

void quickSort(list_ele_t *head, list_ele_t *tail)
{
    if (head != tail) {
        list_ele_t *pi = partition(head, tail), *pi_after;
        pi_after =
            (pi->next == NULL || pi->next == tail || pi->next->next == NULL)
                ? tail
                : pi->next->next;

        quickSort(head, pi);
        quickSort(pi_after, tail);
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (q == NULL || q->size < 2) {
        return;
    }
    quickSort(q->head, q->tail);
}
