#ifndef TASK_H
#define TASK_H

#ifndef POLY_H
#include "POLY.h"
#endif
#include "scalar.h"
#include "thread.h"
#include "channel.h"

////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////

typedef struct Future {
	bool    finished;// still not finished?
	Scalar  result;  // memoized result
	Channel port;    // shared message box
} Future;

static Scalar task_get(Future *const this);
static int    task_join(Future *const this);
static int    task_set(Future *const this, Scalar x);
static int    task_spawn(Future *const this, int main(void*), void* argument);

// handy macro
#define spawn_task(F,R,...)\
	task_spawn(F, R,&(struct R){.future=F __VA_OPT__(,)__VA_ARGS__})

////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////

/* atomic(bool) finished?
 * static inline bool finished(Future *const this) { return this->finished; }
 */

static inline int
task_spawn (Future *const this, int main(void*), void* argument)
{
	enum { syncronous=0, asyncronous=1 };
	int err;

	this->finished = false;
	this->result = (Scalar)(Unsigned)0xFabada;
	if ((err=channel_init(&this->port, syncronous)) == STATUS_SUCCESS) {
		if ((err=thread_spawn(main, argument)) == STATUS_SUCCESS) {
			/*skip*/;
		} else {
			channel_destroy(&this->port);
		}
	}
	return err;
}

// to be called once from the client
static inline int
task_join (Future *const this)
{
	assert(!this->finished);
	int status = channel_receive(&this->port, &this->result);
	this->finished = true;
	channel_destroy(&this->port);
	return status;
}

// to be called once from the promise
static ALWAYS inline int
task_set (Future *const this, Scalar x)
{
	assert(!this->finished);
	return channel_send(&this->port, x);
}

// to be called any number of times from the client
static ALWAYS inline Scalar
task_get (Future *const this)
{
	if (!this->finished) { task_join(this); }
	return this->result;
}

#endif // vim:ai:sw=4:ts=4:syntax=cpp
