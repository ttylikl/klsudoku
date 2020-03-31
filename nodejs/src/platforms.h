#ifndef __PLATFORMS_H__
#define __PLATFORMS_H__

#include <vector>
#include <map>
#include <string>
#include <set>
#include <algorithm>
#include <functional> 
using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifdef ANDROID
#include <android/log.h>
#define LOGCATV(...) __android_log_print(ANDROID_LOG_VERBOSE, "libsudoku", __VA_ARGS__)
#define LOGCATD(...) __android_log_print(ANDROID_LOG_DEBUG , "libsudoku", __VA_ARGS__)
#define LOGCATI(...) __android_log_print(ANDROID_LOG_INFO , "libsudoku", __VA_ARGS__)
#define LOGCATW(...) __android_log_print(ANDROID_LOG_WARN , "libsudoku", __VA_ARGS__)
#define LOGCATE(...) __android_log_print(ANDROID_LOG_ERROR , "libsudoku", __VA_ARGS__)
#endif

#ifndef _WIN32
    #include <sys/socket.h>
    #include <sys/syscall.h>
    #include <unistd.h>
    #include <netinet/tcp.h>
    #include <sys/syscall.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <net/if.h>
#else
	#include <winsock2.h>
	#include <Ws2tcpip.h>
	#include <direct.h> // for _getcwd()
    #ifdef __APPLE__
        #include <pthread.h>
    #endif
	#define strncasecmp _strnicmp
#endif
//
//#ifdef _WIN32
//    #define gettid() GetCurrentThreadId()
//    #define getpid() GetCurrentProcessId()
//	#define strncasecmp _strnicmp
//	#define SetSockOpt(a,b,c,d,e) setsockopt(a,b,c,(const char*)d,e)
//	#define getcwd _getcwd
//#else
//#if defined(Darwin) 
//	#define SOCKADDR_IN struct sockaddr_in
//#endif
//	#define SetSockOpt setsockopt
//    #define SOCKADDR_IN sockaddr_in
//	#ifdef Darwin
//        #define gettid() syscall(SYS_gettid)
//    #else
//        #if !defined(__NR_gettid) && defined(SYS_gettid)
//            #define __NR_gettid SYS_gettid
//        #endif
//        #define gettid() syscall(__NR_gettid)
//    #endif
//#endif
//
//#ifdef _WIN32
//    #define MutexHandle HANDLE
//    //#define MUTEX_LOCK(mtx) { int cnt=0; while( WaitForSingleObject(mtx) != WAIT_OBJECT_0) { if(cnt++>100000){ cnt=0;} } }
//    #define MUTEX_LOCK(mtx) {  while( WaitForSingleObject(mtx,INFINITE) != WAIT_OBJECT_0) {  } }
//    #define MUTEX_UNLOCK(mtx) ReleaseMutex(mtx)
//#else
//    //#define MutexHandle pthread_mutex_t
//    typedef pthread_mutex_t MutexHandle;
//    #define MUTEX_LOCK(mtx) pthread_mutex_lock(&mtx)
//    #define MUTEX_UNLOCK(mtx) pthread_mutex_unlock(&mtx)
//#endif
//
#endif //__PLATFORMS_H__
