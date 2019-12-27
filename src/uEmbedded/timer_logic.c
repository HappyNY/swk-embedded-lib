#include "timer_logic.h"
#include "uassert.h"

size_t timer_init(timer_logic_t *s, void *buff, size_t buffSize)
{
    size_t retval =
        fslist_init(&s->nodes, buff, buffSize, sizeof(timer_info_t));
    s->idGen = 0;
    return retval;
}

static inline struct fslist_node *timer_find(timer_logic_t *s, size_t tick)
{
    struct fslist_node *head;

    if (s->nodes.size == 0)
        return NULL;

    head = s->nodes.get + s->nodes.head;

    while (head) {
        // Finds place to insert new timer.
        if (((timer_info_t *)fslist_data(&s->nodes, head))->triggerTime >= tick)
            return head;

        head = fslist_next(&s->nodes, head);
    }

    return NULL;
}

timer_handle_t timer_add(timer_logic_t *s, size_t        whenToTrigger,
                         void (*callback)(void *), void *callbackObj)
{
    struct fslist_node *n;
    timer_info_t *      info;
    timer_handle_t      ret;

    n = fslist_insert(&s->nodes, timer_find(s, whenToTrigger));
    uassert(n);
    uassert(callback);

    info = (timer_info_t *)fslist_data(&s->nodes, n);

    info->callback    = callback;
    info->callbackObj = callbackObj;
    info->timerId     = s->idGen++;
    info->triggerTime = whenToTrigger;

    ret.n       = n;
    ret.timerId = info->timerId;
    return ret;
}

size_t timer_update(timer_logic_t *s, size_t curTime)
{
    struct fslist_node *head;
    timer_info_t *      info;
    void (*cb)(void *);
    void *obj;

    if (s->nodes.size == 0)
        return (size_t)-1;

    for (; s->nodes.head != FSLIST_NODEIDX_NONE;) {
        head = &s->nodes.get[s->nodes.head];
        info = (timer_info_t *)fslist_data(&s->nodes, head);

        // Timer update done.
        if (info->triggerTime > curTime)
            return info->triggerTime;

        cb  = info->callback;
        obj = info->callbackObj;

        // Erase head node
        fslist_erase(&s->nodes, head);
        cb(obj);
    }

    return (size_t)-1;
}
