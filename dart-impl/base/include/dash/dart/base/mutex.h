#ifndef DASH_DART_BASE_MUTEX__H_
#define DASH_DART_BASE_MUTEX__H_

#include <dash/dart/base/logging.h>
#include <dash/dart/base/config.h>
#include <dash/dart/if/dart_types.h>
#include <dash/dart/if/dart_util.h>

#if defined(DART_ENABLE_THREADSUPPORT) && !defined(DART_HAVE_PTHREADS)
#error "Thread support has been enabled but PTHREADS support is not available!"
#endif

#if !defined(DART_ENABLE_THREADSUPPORT) && defined(DART_HAVE_PTHREADS)
#undef DART_HAVE_PTHREADS
#endif

#ifdef DART_HAVE_PTHREADS
#include <pthread.h>
#define DART_MUTEX_INITIALIZER { PTHREAD_MUTEX_INITIALIZER }
#else
#define DART_MUTEX_INITIALIZER { 0 }
#endif


typedef struct dart_mutex {
#ifdef DART_HAVE_PTHREADS
pthread_mutex_t mutex;
#else 
// required since C99 does not allow empty structs
// TODO: this could be used for correctness checking
char __dummy;
#endif
} dart_mutex_t;

DART_INLINE
dart_ret_t
dart__base__mutex_init(dart_mutex_t *mutex)
{
#ifdef DART_HAVE_PTHREADS
  // pthread_mutex_init always succeeds
  pthread_mutex_init(&mutex->mutex, NULL);
  DART_LOG_TRACE("%s: Initialized fast mutex %p", __FUNCTION__, mutex);
  return DART_OK;
#else
  DART_LOG_INFO("%s: thread-support disabled", __FUNCTION__);
  return DART_OK;
#endif
}

DART_INLINE
dart_ret_t
dart__base__mutex_init_recursive(dart_mutex_t *mutex)
{
#ifdef DART_HAVE_PTHREADS
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  int ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
  if (ret != 0) {
    DART_LOG_WARN("dart__base__mutex_init_recursive: Failed to initialize "
                  "recursive pthread mutex! "
                  "Falling back to standard mutex...");
    pthread_mutexattr_destroy(&attr);
    return dart__base__mutex_init(mutex);
  }
  pthread_mutex_init(&mutex->mutex, &attr);
  pthread_mutexattr_destroy(&attr);
  DART_LOG_TRACE("%s: Initialized recursive mutex %p", __FUNCTION__, mutex);
  return DART_OK;
#else
  DART_LOG_INFO("%s: thread-support disabled", __FUNCTION__);
  return DART_OK;
#endif
}

DART_INLINE
dart_ret_t
dart__base__mutex_lock(dart_mutex_t *mutex)
{
#ifdef DART_HAVE_PTHREADS
  int ret = pthread_mutex_lock(&mutex->mutex);
  if (ret != 0) {
    DART_LOG_TRACE("%s: Failed to lock mutex (%i)", __FUNCTION__, ret);
    return DART_ERR_OTHER;
  }
  return DART_OK;
#else
  return DART_OK;
#endif
}

DART_INLINE
dart_ret_t
dart__base__mutex_unlock(dart_mutex_t *mutex)
{
#ifdef DART_HAVE_PTHREADS
  int ret = pthread_mutex_unlock(&mutex->mutex);
  if (ret != 0) {
    DART_LOG_TRACE("%s: Failed to unlock mutex (%i)", __FUNCTION__, ret);
    return DART_ERR_OTHER;
  }
  return DART_OK;
#else
  return DART_OK;
#endif
}

DART_INLINE
dart_ret_t
dart__base__mutex_trylock(dart_mutex_t *mutex)
{
#ifdef DART_HAVE_PTHREADS
  int ret = pthread_mutex_trylock(&mutex->mutex);
  DART_LOG_TRACE("dart__base__mutex_trylock: lock %p aqcuired: %s",
                 mutex, (ret == 0) ? "yes" : "no");
  return (ret == 0) ? DART_OK : DART_PENDING;
#else
  return DART_OK;
#endif
}

DART_INLINE
dart_ret_t
dart__base__mutex_destroy(dart_mutex_t *mutex)
{
#ifdef DART_HAVE_PTHREADS
  int ret = pthread_mutex_destroy(&mutex->mutex);
  if (ret != 0) {
    DART_LOG_TRACE("%s: Failed to destroy mutex (%i)", __FUNCTION__, ret);
    return DART_ERR_OTHER;
  }
  return DART_OK;
#else
  return DART_OK;
#endif
}

#endif /* DASH_DART_BASE_MUTEX__H_ */
