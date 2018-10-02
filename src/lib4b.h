#ifndef __LIB4B_H__
#define __LIB4B_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _WIN32
//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>

// for struct stat
#include <sys/stat.h>


// for nonblock
#include <fcntl.h>
#include <errno.h>

#if defined(__darwin__) && !defined(__APPLE__)
#define __APPLE__
#endif

#ifdef _WIN32


#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Rpc.h> // for __RPC_FAR ,which used in Rpcdce.h
#include <Rpcdce.h> //for UuidCreateSequential()
#include <iphlpapi.h>

//replacement for <unistd.h> in vs2008
#include <io.h>
#include <process.h> // for _beginthread()

#include <direct.h> //for _getcwd
#else

#include <pthread.h>
#include <signal.h>
// for keepalive
#include <netinet/tcp.h>
#include <sys/syscall.h>  
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#if !defined(__APPLE__)
#include <linux/sockios.h>
//#include <linux/wait.h>
//#else
//#include <arpa/inet.h>
#endif
#include <sys/wait.h>
#include <unistd.h>
#include <strings.h> //strcasecmp, strncasecmp

#endif

#ifdef _WIN32
#define uint unsigned int
#define ssize_t SSIZE_T
#if !defined(getcwd)
#define getcwd _getcwd
#endif
#define popen _popen
#define pclose _pclose
#define strcasecmp _stricmp
//#define strnicmp _strnicmp
#define strncasecmp _strnicmp
//#define strnicmp strncasecmp
#define snprintf _snprintf
//This will make wrong with 'obj.close()' in MSVC !
//#define write _write
//#define close(x) _close(x)
#define putenv _putenv
#define SLASH "\\"
#define SLASH_CHAR '\\'

#define  __func__ __FUNCTION__
#else
#define SOCKADDR_IN struct sockaddr_in
#define SLASH "/"
#define SLASH_CHAR '/'
#endif

#ifndef _WIN32

#define ThreadHandle pthread_t
#define ThreadFunction(func,arg) void * func(void *arg) 
#define ThreadReturn(x) return (void *)x
#define ThreadStart(result,func,arg) { pthread_create(&result,0,func,(void *)arg); }
#define ThreadCancel(tid) pthread_cancel(tid)
#define ThreadWait(tid,n) pthread_join(tid,n)
#define MutexHandle pthread_mutex_t
#define MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define MutexOpen(mtx) pthread_mutex_init(&mtx,0)
#define MutexClose(mtx) pthread_mutex_destroy(&mtx)
#define MUTEX_LOCK(mtx) pthread_mutex_lock(&mtx)
#define MUTEX_UNLOCK(mtx) pthread_mutex_unlock(&mtx)
#define SetSockOpt setsockopt

//#define gettid() syscall(SYS_gettid)
#if !defined(__APPLE__)
#define gettid() syscall(__NR_gettid)
#else
//#define gettid() (int)pthread_self()
#define gettid() syscall(SYS_gettid)
#endif

#define closesocket close
#else

#define ThreadHandle HANDLE
#define ThreadFunction(func,arg) void func(void *arg) 
#define ThreadReturn(x) return
#define ThreadStart(result,func,arg) {result=(HANDLE)_beginthread((void (*)(void *))func,0,(void *)arg); }
#define ThreadCancel(tid) TerminateThread(tid,-1)
#define ThreadWait(tid,n) (WaitForSingleObject((tid), INFINITE) != WAIT_OBJECT_0) 
#define SetSockOpt(a,b,c,d,e) setsockopt(a,b,c,(const char*)d,e)
#define gettid() GetCurrentThreadId()
#define getpid() GetCurrentProcessId()
#if !defined(localtime_r)
//#define localtime_r(a,b) localtime(a)
#define localtime_r( _clock, _result ) \
        ( *(_result) = *localtime( (_clock) ), \
          (_result) )
#endif
#define MutexHandle HANDLE
#define MUTEX_INITIALIZER 0
#define MutexOpen(mtx) mtx=CreateMutex(0,0,0)
#define MutexClose(mtx) CloseHandle(mtx)
//#define MUTEX_LOCK(mtx) { int cnt=0; while( WaitForSingleObject(mtx) != WAIT_OBJECT_0) { if(cnt++>100000){ cnt=0;} } }
#define MUTEX_LOCK(mtx) { if(mtx==0) MutexOpen(mtx); while( WaitForSingleObject(mtx,INFINITE) != WAIT_OBJECT_0) {  } }
#define MUTEX_UNLOCK(mtx) ReleaseMutex(mtx)
static void sleep(int x) { Sleep(x*1000); }

#endif

#define FREE(x) {if(x) { free(x); x=NULL; } }
#define ZEROSTR(x) memset(x,0,sizeof(x))
#define STRNCPY(dst,src) if(src){strncpy(dst,src,sizeof(dst)-1);*(dst+sizeof(dst)-1)=0;}

extern char *g_logpath;
extern int g_loglvl;
extern char gLogUDP[32];
extern MutexHandle g_logmtx;
extern MutexHandle g_mtx;

enum {READY_RECV=0x01, READY_SEND=0x02};
enum {PORT_UDP_LOG=4444,PORT_SCREEN=2222,PORT_XZSERVER=3222,PORT_APP=4222};
enum {LOG_NONE,LOG_ERROR,LOG_WARN,LOG_INF,LOG_DBG,LOG_TRACE,LOG_ALL};

int ThreadIsAlive(ThreadHandle tid);

typedef int (*CallbackFunction)(void *caller,void *arg);

int strFind(const char *string, const char *strCharSet,  int nPos);
char *strLTrim(char *str);
char *strRTrim(char *str);
char *strTrim(char *str);
int chrFind(const char *string, const char CharSet, int nPos);
int getKeyValue(const char *chString,char *dest,const char *chKey,const char ch);
char* getKV(const char*src,char *val,int len,const char *key,char ch_kv,char ch_lf);
int get_piport(int fd,char *buf,int *port);
int get_iport(int fd,char *buf,int *port);

long long getCurrentTimeMS();
char * getTimeStr(const time_t t);
int setLogPath(const char *psz);
char *getLogFilename(time_t t);
void plog_raw_stdout(const char * buf);
void plog_raw(const char * buf);
void setLogUDP(char *ip);
void plog_udp(char *ip,short port,const char *buf);
void set_loglvl(int loglvl);
int get_loglvl();
void plog_wrap_fmt(int log2file,int loglevel,const char *file,const char *func,uint line,const char *fmt,...);

unsigned long get_file_size(const char *filename);
int isAbsolutePath(const char*path);
int isExist(const char *filename);
int isDirectory(const char *filename);

int select_socket(int sockfd, int timeout);
int do_listen(unsigned short port);
int do_connect(int sockfd,const char *h,unsigned int p);
int set_timeout(int sockfd,int s,int us);
int set_keepalive(int fd);
int set_nonblock(int fd);
int do_send(int sockfd,const char * buf,int len);


#ifdef ANDROID
#include <android/log.h>
//int __android_log_write(int prio, const char *tag, const char *text);
//int __android_log_print(int prio, const char *tag,  const char *fmt, ...)
#endif

#define plog(...) plog_wrap_fmt(0,LOG_DBG,__FILE__,__func__,__LINE__,__VA_ARGS__)
#define perr(...) plog_wrap_fmt(0,LOG_ERROR,__FILE__,__func__,__LINE__,__VA_ARGS__)
#define pwrn(...) plog_wrap_fmt(0,LOG_WARN,__FILE__,__func__,__LINE__,__VA_ARGS__)
#define pinf(...) plog_wrap_fmt(0,LOG_INF,__FILE__,__func__,__LINE__,__VA_ARGS__)
#define ptrace(...) plog_wrap_fmt(0,LOG_TRACE,__FILE__,__func__,__LINE__,__VA_ARGS__)

#define fplog(...) plog_wrap_fmt(1,LOG_DBG,__FILE__,__func__,__LINE__,__VA_ARGS__)
#define fperr(...) plog_wrap_fmt(1,LOG_ERROR,__FILE__,__func__,__LINE__,__VA_ARGS__)
#define fpwrn(...) plog_wrap_fmt(1,LOG_WARN,__FILE__,__func__,__LINE__,__VA_ARGS__)
#define fpinf(...) plog_wrap_fmt(1,LOG_INF,__FILE__,__func__,__LINE__,__VA_ARGS__)
#define fptrace(...) plog_wrap_fmt(1,LOG_TRACE,__FILE__,__func__,__LINE__,__VA_ARGS__)

#define MYDEBUG() plog("DEBUG:%s:%d",__FILE__,__LINE__);
//#ifndef DEBUG
////deprecated
//#define DEBUG MYDEBUG
//#endif


struct slist
{
	char key[100];
	void *data;
	int len;
	struct slist *next;
};

typedef  struct slist slist;

void slist_put(slist **pHead,const char *key,const void *val,int len);
int slist_size(slist **pHead);
slist * slist_find(slist **pHead,const char *key);
void slist_dumps(slist **pHead);
slist * slist_get(slist **pHead,const char *key);
void slist_free(slist *node);

#ifdef __cplusplus
}
#endif

#endif

