/*
 * Future
 *
 * Compile: gcc -O2 -lpthread ...
 *
 */
#ifndef FUTURE_H
#define FUTURE_H

#ifndef POLY_H
#include "POLY.h"
#endif
#include "task.h"
#include "channel.h"
#include "scalar.h"

////////////////////////////////////////////////////////////////////////
// Type Future
// Interface
////////////////////////////////////////////////////////////////////////

typedef struct Future {
	bool    pending;
	Scalar  result;  // memoized result
	Channel port;
} Future;

static inline Scalar ftr_get(Future* self);
static inline int    ftr_spawn(Future* self, int(*root)(void*), void* argument);
static inline int    ftr_set_(Future* self, Scalar x);
static inline int    ftr_join(Future* self);

// Accept any scalar type
#define ftr_set(FUTURE,EXPRESSION) ftr_set_((FUTURE), coerce(EXPRESSION))

// handy macro
#define spawn_future(F,R,...)\
	ftr_spawn(F,R,&(struct R){.future=F __VA_OPT__(,)__VA_ARGS__})

////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////

static inline int ftr_spawn(Future* self, int(*root)(void*), void* argument)
{
	enum { syncronous=0, asyncronous=1 };
	int err;

	self->pending = true;
	self->result.word = 0x1aFabada;
	if ((err=chn_init(&self->port, asyncronous)) == STATUS_SUCCESS) {
		if ((err=tsk_spawn(root, argument)) == STATUS_SUCCESS) {
			return STATUS_SUCCESS;
		} else {
			chn_destroy(&self->port);
		}
	}
	return err;
}

// to be called once from the promise
static ALWAYS inline int ftr_set_(Future* self, Scalar x)
{
	assert(self->pending);
	int status = chn_send_(&self->port, x);               /*ASYNC*/
	return status;
}

// to be called once from the client
static inline int ftr_join(Future* self)
{
	assert(self->pending);
	int status = chn_receive(&self->port, &self->result); /*ASYNC*/
	self->pending = false;
	chn_destroy(&self->port);
	return status;
}

// to be called any number of times from the client
static ALWAYS inline Scalar ftr_get(Future* self)
{
	if (self->pending) { ftr_join(self); }
	assert(!self->pending);
	return self->result;
}

#endif // FUTURE_H

// vim:ai:sw=4:ts=4:syntax=cpp
