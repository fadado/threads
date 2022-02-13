// Semaphore test
// gcc -Wall -O2 -lpthread filename.c

#include <stdlib.h>
//#include <time.h>

// uncomment next line to enable assertions
#define DEBUG
#include "poly/scalar.h"
#include "poly/thread.h"
#include "poly/semaphore.h"

static Semaphore test_lock_mutex;
static Integer test_lock_counter;
enum { N=10, M=10000 };

int thread_test(void* arg)
{
	assert(arg == (void*)0);
#ifdef DEBUG
	//warn("Enter %s", __func__);
#endif
	for (int i=0; i < M; ++i) {
		semaphore_acquire(&test_lock_mutex);
		Integer* pi = &test_lock_counter;
		Integer** ppi = &pi;
		Integer tmp = (**ppi-1) + 2;
		**ppi = tmp;
		semaphore_release(&test_lock_mutex);
	}
	thread_yield();

	return 0;
}

static void test_lock(void)
{
#ifdef DEBUG
	warn("Enter %s", __func__);
#endif
	test_lock_counter = 0;

	semaphore_init(&test_lock_mutex, 1);

	Thread t[N];
	for (int i=0; i < N; ++i) {
		int e = thread_fork(thread_test, (void*)0, &t[i]);
		assert(e == STATUS_SUCCESS);
	}
	for (int i=0; i < N; ++i) {
		int e = thread_join(t[i], (int*)0);
		assert(e == STATUS_SUCCESS);
	}
	assert(test_lock_counter == N*M);
	warn("COUNT: %lld\n", test_lock_counter);
}

int main(int argc, char* argv[argc+1])
{
	test_lock();

	return EXIT_SUCCESS;
}

// vim:ai:sw=4:ts=4:syntax=cpp
