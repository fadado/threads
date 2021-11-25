// Channel test

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <threads.h>

#include "channel.h"

// comment next line to enable assertions
//#define NDEBUG
#include <assert.h>

////////////////////////////////////////////////////////////////////////
// FIFO test
////////////////////////////////////////////////////////////////////////

#define N 10
#define M 43

static int producer(void* args)
{
	int err;
#	define catch(X)	if ((err=(X))!=thrd_success) return err

	Channel* channel = args;
	for (int i=0; i < M; ++i) {
		catch (chn_send(channel, '0'+i));
	}
	return thrd_success;
#	undef catch
}

static int consumer(void* args)
{
	int err;
#	define catch(X)	if ((err=(X))!=thrd_success) return err

	Channel* channel = args;
	Scalar s;
	for (int i=0; i < M; ++i) {
		catch (chn_receive(channel, &s));
		char c = chn_cast(s, '@');
		putchar(c);
	}
	putchar('\n');
	return thrd_success;
#	undef catch
}

int main(int argc, char* argv[])
{
	int err;
#	define catch(X)	if ((err=(X))!=thrd_success) goto onerror

	Channel chn;
	catch (chn_init(&chn, N));

	thrd_t p, c;
	catch (thrd_create(&p, producer, &chn));
	catch (thrd_create(&c, consumer, &chn));

	catch (thrd_join(p, &err)); catch (err);
	catch (thrd_join(c, &err)); catch (err);

	chn_destroy(&chn);

	return EXIT_SUCCESS;

onerror:
#	undef catch
	enum { internal_error };
	static const char* ename[] = {
		[thrd_success] = "thrd_success",
		[thrd_nomem] = "thrd_nomem",
		[thrd_busy] = "thrd_busy",
		[thrd_error] = "thrd_error",
		[thrd_timedout] = "thrd_timedout",
	};
	assert(err != thrd_success);
	switch (err) {
		case thrd_nomem:
			error(EXIT_FAILURE, ENOMEM, "%s", ename[err]);
		case thrd_busy:
			error(EXIT_FAILURE, EBUSY, "%s", ename[err]);
		case thrd_error:
			error(EXIT_FAILURE, ECANCELED, "%s", ename[err]);
		case thrd_timedout:
			error(EXIT_FAILURE, ETIMEDOUT, "%s", ename[err]);
		default: assert(internal_error);
	}
	return EXIT_FAILURE;
}

// vim:ai:sw=4:ts=4:syntax=cpp
