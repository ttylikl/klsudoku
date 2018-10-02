#ifndef __FUNCS_H__
#define __FUNCS_H__

#include "common.h"

int LIBAPI initLibrary();
int LIBAPI uninitLibrary();
char * LIBAPI getTimeStr(const time_t t, char *buf, int len);
int LIBAPI isValidNum(int num);
int LIBAPI isValidCord(int cord);
int LIBAPI num2cord(int num);
int LIBAPI cord2num(int cord);
int LIBAPI cord2boxid(int cx,int cy);
class SPoint;
int LIBAPI cord2boxid(SPoint &pt);

//// Networks
int LIBAPI create_multicast_socket(const char *group,unsigned int port);
int LIBAPI create_udp_socket(const char *group,unsigned int port);
string LIBAPI getMacAddress();

int LIBAPI c_generate(char *out,char *result_out,int len);

#if !defined(localtime_r)
//#define localtime_r(a,b) localtime(a)
#define localtime_r( _clock, _result ) \
	(*(_result) = *localtime((_clock)), \
	(_result))
#endif

#ifndef __func__
#define __func__ __FUNCTION__
#endif

#define KDEBUG() fplog("DEBUG:%s:%d",__FILE__,__LINE__);


#define KAssert(chk,act) { if(!chk) { act;} }
#define KAssertRet(chk,act,ret) { if(!chk ) { act;  return ret;} }

//extern MutexHandle LOCK_VIEW;
//extern MutexHandle LOCK_LOG;
//extern MutexHandle LOCK_SOCK;
//
//class AutoLock
//{
//public:
//	MutexHandle *mtx;
//	AutoLock(MutexHandle *lock)
//	{
//		mtx=lock;
//		MUTEX_LOCK(*mtx);
//	}
//	~AutoLock()
//	{
//		MUTEX_UNLOCK(*mtx);
//	}
//};

#endif