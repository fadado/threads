/*
 * Events
 *
 * Compile: gcc -O2 -lpthread ...
 *
 */
#ifndef EVENT_H
#define EVENT_H

#ifndef FAILURE_H
#error To cope with failure I need "failure.h"!
#endif

#include <threads.h>

////////////////////////////////////////////////////////////////////////
// Type Event
// Interface
////////////////////////////////////////////////////////////////////////

typedef struct Event {
	int   state;
	cnd_t queue;
} Event;

static inline int  evt_init(Event* self);
static inline void evt_destroy(Event* self);
static inline int  evt_wait(Event* self, mtx_t* mutex);
static inline int  evt_block(Event* self, mtx_t* mutex);
static inline int  evt_signal(Event* self);

////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////

#define ALWAYS __attribute__((always_inline))

static inline int evt_init(Event* self)
{
	self->state = 0;
	return cnd_init(&self->queue);
}

static inline void evt_destroy(Event* self)
{
	cnd_destroy(&self->queue);
}

static ALWAYS inline int evt_wait(Event* self, mtx_t* mutex)
{
	// assume `mutex` is locked
	while (self->state == 0) {
		int err = cnd_wait(&self->queue, mutex);
		if (err != thrd_success) return err;
	}
	--self->state;
	return thrd_success;
}

static ALWAYS inline int evt_block(Event* self, mtx_t* mutex)
{
	// assume `mutex` is locked
	do {
		int err = cnd_wait(&self->queue, mutex);
		if (err != thrd_success) return err;
	} while (self->state == 0);
	--self->state;
	return thrd_success;
}

static ALWAYS inline int evt_signal(Event* self)
{
	++self->state;
	return cnd_signal(&self->queue);
}

#undef ALWAYS

#endif // EVENT_H

// vim:ai:sw=4:ts=4:syntax=cpp