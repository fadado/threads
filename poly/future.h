#ifndef FUTURE_H
#define FUTURE_H

#ifndef POLY_H
#include "POLY.h"
#endif
#include "scalar.h"
#include "thread.h"
#include "passing/port.h"

////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////

typedef struct Future {
	bool    finished;// still not finished?
	Scalar  result;  // memoized result
	Port    mbox;    // message box
} Future;

static int    future_fork(int main(void*), void* argument, Future *const this);
static Scalar future_get(Future *const this);
static int    future_join(Future *const this);
static int    future_set(Future *const this, Scalar x);

////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////

/* atomic(bool) finished ?
 * static inline bool future_finished(Future *const this) { return this->finished; }
 */

static inline int
future_fork (int main(void*), void* argument, Future *const this)
{
	int err;

	this->finished = false;
	this->result = Unsigned(0xFabada);
	if ((err=port_init(&this->mbox)) != STATUS_SUCCESS) {
		return err;
	}
	if ((err=thread_fork(main, argument, &(Thread){0})) != STATUS_SUCCESS) {
		port_destroy(&this->mbox);
		return err;
	}
	return STATUS_SUCCESS;
}

// to be called once from the client
static inline int
future_join (Future *const this)
{
	assert(!this->finished);
	const int status = port_receive(&this->mbox, &this->result);
	this->finished = true;
	port_destroy(&this->mbox);
	return status;
}

// to be called once from the promise
static ALWAYS inline int
future_set (Future *const this, Scalar x)
{
	assert(!this->finished);
	return port_send(&this->mbox, x);
}

// to be called any number of times from the client
static ALWAYS inline Scalar
future_get (Future *const this)
{
	if (!this->finished) { future_join(this); }
	return this->result;
}

#endif // vim:ai:sw=4:ts=4:syntax=cpp
