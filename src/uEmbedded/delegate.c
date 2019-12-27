#include "delegate.h"
#include "uassert.h"
#include <stdlib.h>
#include <string.h>

typedef struct node {
    struct node *       next;
    refhandle_t         objref;
    delegate_event_cb_t cb;
} node_t;

struct delegate {
    node_t *head;
    size_t  cnt;
    bool    bMulticast;
    bool    bEraseLock;
};

void delegate_assign(delegate_t *s, refhandle_t const *objref,
                     delegate_event_cb_t callback)
{
    uassert(s && callback && objref);

    if (s->bMulticast) {
        node_t *newNode;
        newNode       = malloc(sizeof(node_t));
        newNode->next = s->head;
        s->head       = newNode;
        ++s->cnt;
    }
    else if (s->head == NULL) {
        s->head = malloc(sizeof(node_t));
        s->cnt  = 1;
    }

    s->head->objref = *objref;
    s->head->cb     = callback;
}

void delegate_clear(delegate_t *s)
{
    uassert(s);
    node_t *head = s->head;
    node_t *next;
    int     bEraseLock = s->bEraseLock;

    for (; head; head = next) {
        next = head->next;
        if (bEraseLock)
            head->cb = NULL;
        else
            free(head);
    }
    s->cnt = 0;

    if (bEraseLock == false)
        s->head = NULL;
}

size_t delegate_size(delegate_t *s) { return s->cnt; }

size_t delegate_delete(delegate_t *s, refhandle_t const *objref,
                       delegate_event_cb_t callback)
{
    uassert(s && objref);
    node_t *head = s->head;
    node_t *prev = NULL;
    node_t *next;
    int     bMulticast = s->bMulticast;
    int     bEraseLock = s->bEraseLock;
    size_t  numDelete  = 0;

    for (; head; head = next) {
        next = head->next;
        if (head->cb == callback &&
            memcmp(&head->objref, objref, sizeof(*objref)) == 0) {
            if (prev)
                prev->next = next;
            else // if prev is null, means erasing node is the first node ...
                s->head = next;

            if (bEraseLock)
                head->cb = NULL;
            else
                free(head);

            ++numDelete;
            s->cnt -= bMulticast;
        }
        else {
            // If any node deleted, previous node stay as previous ...
            prev = head;
        }
    }
}

void delegate_call(delegate_t *s, void *event_args)
{
    uassert(s);
    // To keep iteration solid ...
    s->bEraseLock          = true;
    node_t * pending_erase = NULL;
    node_t **ppnext        = &s->head;

    // Callback each element iterating all nodes
    // Remove any node with its callback is NULL, or its object reference is
    // expired.
    for (; *ppnext;) {
        node_t *head = *ppnext;

        // When this node is pending erase ...
        if (head->cb == NULL || ref_is_valid(&head->objref) == false) {
            // Append to pending erase list
            head->next    = pending_erase;
            pending_erase = head;
            *ppnext       = head->next;
            s->cnt--;
            continue;
        }

        // When not erased ...
        // Execute callback.
        head->cb(&head->objref, event_args);

        ppnext = &head->next;
    }

    // Erase all pending erased nodes
    node_t *next;
    while (pending_erase) {
        next = pending_erase->next;
        free(pending_erase);
        pending_erase = next;
    }
    s->bEraseLock = false;
}

void delegate_destroy(delegate_t *s)
{
    uassert(s);
    delegate_clear(s);
    free(s);
}

delegate_t *delegate_create(bool bMulticast)
{
    delegate_t *ret = malloc(sizeof(delegate_t));
    ret->cnt        = 0;
    ret->head       = NULL;
    ret->bEraseLock = false;
    ret->bMulticast = bMulticast;
    return ret;
}
