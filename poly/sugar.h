#ifndef SUGAR_H
#define SUGAR_H

////////////////////////////////////////////////////////////////////////
// Declaring and defining threads
////////////////////////////////////////////////////////////////////////

/*
 *  THREAD_TYPE (name [,linkage])
 *      slots
 *      ...
 *  END_TYPE
 */
#define THREAD_TYPE(T,...)\
    __VA_ARGS__ int T(void*);\
    struct T {

#define END_TYPE\
    };

/*
 *  THREAD_BODY (name)
 *      code
 *      ...
 *  END_BODY
 */
#define THREAD_BODY(T)\
    int T(void* arg_)\
    {   /*assert(arg_ != (void*)0);*/\
        struct T const this = *((struct T*)arg_);\
        thread_detach(thread_current());

#define END_BODY\
        return 0;\
    }

////////////////////////////////////////////////////////////////////////
// Spawning threads, futures, filters...
////////////////////////////////////////////////////////////////////////

/*
 *  THREAD_TYPE (name)
 *      ...
 *  END_TYPE
 *
 *  THREAD_BODY (name)
 *      ...
 *  END_BODY
 */
#define spawn_thread(T,...)\
    thread_fork(T, &(struct T){__VA_ARGS__}, &(Thread){0})

/*
 *  THREAD_TYPE (name)
 *      Task* future;  // future slots
 *      ...
 *  END_TYPE
 *
 *  THREAD_BODY (name)
 *      ...
 *  END_BODY
 */
#define spawn_future(F,T,...)\
    future_fork(T,\
        &(struct T){.future=(F)__VA_OPT__(,)__VA_ARGS__}, (F))

#define FUTURE_SLOTS\
    Future* future;

/*
 *  THREAD_TYPE (name)
 *      Channel* input;  // filter slots
 *      Channel* output;
 *      ...
 *  END_TYPE
 *
 *  THREAD_BODY (name)
 *      ...
 *  END_BODY
 */
#define spawn_filter(I,O,T,...)\
    thread_fork(T, \
        &(struct T){.input=(I), .output=(O)__VA_OPT__(,)__VA_ARGS__},\
        &(Thread){0})

#define FILTER_SLOTS\
    Channel* input;\
    Channel* output;

#endif // vim:ai:sw=4:ts=4:et:syntax=cpp