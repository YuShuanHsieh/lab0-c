#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q) {
        q->head = NULL;
        q->size = 0;
    }
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q) {
        return;
    }

    list_ele_t *next = NULL, *head = q->head;

    while (head) {
        free(head->value);
        next = head->next;
        free(head);
        head = next;
    }

    free(q);
}

static list_ele_t *element_new(char *s)
{
    size_t s_len = strlen(s);
    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (newh) {
        newh->next = NULL;
        newh->value = NULL;
        newh->prev = NULL;

        if (s_len > 0) {
            newh->value = malloc(s_len + 1);
            if (!newh->value) {
                free(newh);
                return NULL;
            }
            strncpy(newh->value, s, s_len);
            newh->value[s_len] = '\0';
        }
    }

    return newh;
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
    list_ele_t *newh;
    if (!q)
        return false;

    newh = element_new(s);

    if (!newh)
        return false;

    newh->next = q->head;
    if (q->head)
        q->head->prev = &(newh->next);
    q->head = newh;
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
    list_ele_t *newh, *tail;
    if (!q)
        return false;

    tail = q->head;
    newh = element_new(s);

    if (!newh)
        return false;

    if (!tail) {
        q->head = newh;
        q->size++;
        return true;
    }

    while (tail->next)
        tail = tail->next;

    tail->next = newh;
    newh->prev = &(tail->next);
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
    if (!q || !q->head) {
        return false;
    }
    list_ele_t *head = q->head;
    q->head = q->head->next;

    q->size--;

    if (q->head) {
        q->head->prev = NULL;
    }
    if (head->value) {
        if (sp) {
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
    return q ? q->size : 0;
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

    list_ele_t *prev = NULL, *current = q->head, *next = NULL;
    while (current) {
        next = current->next;
        current->next = prev;

        if (prev)
            current->prev = &(prev->next);

        prev = current;
        current = next;
    }

    q->head = prev;
}

void swap(list_ele_t *e1, list_ele_t *e2)
{
    char *temp = e1->value;
    e1->value = e2->value;
    e2->value = temp;
}

void merge(list_ele_t **head1, list_ele_t **head2, list_ele_t **tail)
{
    list_ele_t *next, *head = NULL, *ptr1 = *head1, *ptr2 = *head2;

    while (1) {
        if (strcmp(ptr1->value, ptr2->value) < 0) {
            next = ptr1;
            ptr1 = ptr1->next;
        } else {
            next = ptr2;
            ptr2 = ptr2->next;
        }
        if (!head) {
            next->prev = NULL;
            head = next;
            *head1 = head;
        } else {
            next->prev = &(head->next);
            head->next = next;
            head = head->next;
        }
        if (ptr1 && ptr2) {
            continue;
        } else if (!ptr1) {
            head->next = ptr2;
            while (ptr2 != *tail)
                ptr2 = ptr2->next;
            *tail = ptr2;
            break;
        } else {
            head->next = ptr1;
            while (ptr1) {
                *tail = ptr1;
                ptr1 = ptr1->next;
            }
            break;
        }
    }
}

void merge_sort(list_ele_t **head, list_ele_t **tail, size_t size)
{
    if (*head == *tail) {
        goto end;
    }
    if ((*head)->next == *tail) {
        if (strcmp((*head)->value, (*tail)->value) > 0) {
            swap(*head, *tail);
        }
        goto end;
    }
    list_ele_t *head2 = *head, *head3 = NULL;
    size_t pilot = (size / 2);
    if (size % 2 == 1)
        pilot++;

    for (size_t n = 0; n < pilot - 1; n++)
        head2 = head2->next;
    head3 = head2->next;
    merge_sort(head, &head2, pilot);
    merge_sort(&head3, tail, size - pilot);

    merge(head, &head3, tail);
end:
    (*head)->prev = NULL;
    (*tail)->next = NULL;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || q->size < 2)
        return;
    list_ele_t *tail = q->head;
    while (tail->next)
        tail = tail->next;
    merge_sort(&(q->head), &tail, q->size);
}
