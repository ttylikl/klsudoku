#if defined(_WIN32) && defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#include "lib4b.h"

MutexHandle g_mtx=MUTEX_INITIALIZER;
MutexHandle g_logmtx=MUTEX_INITIALIZER;

char gLogUDP[32]={0};
char *g_logpath=NULL;
//exported
//#if defined(DEBUG) ||  defined(_DEBUG) ||defined(ANDROID)
int g_loglvl=LOG_ALL;
//#else
//int g_loglvl=LOG_NONE;
//#endif

int ThreadIsAlive(ThreadHandle tid)
{
#ifdef _WIN32
    DWORD rc=0; 
    if(tid==0) return 0;
	GetExitCodeThread(tid,&rc);
    return (rc==STILL_ACTIVE);
#else
    int rc=0;
    if(tid==0) return 0;
    rc=pthread_kill(tid,0);
    return (rc!=ESRCH && rc!=EINVAL);
#endif
}

char *strLTrim(char *str)
{
	char *p=str;
	while(*p=='\r'||*p=='\n'||*p==' ')
	{
		p++;
	}
	if(p!=str)
		strcpy(str,p);
	return str;
}

char *strRTrim(char *str)
{
	char *p=str+strlen(str)-1;
	while(*p=='\r'||*p=='\n'||*p==' ')
	{
		*p=0;
		p--;
	}
	return str;
}

char *strTrim(char *str)
{
	return strLTrim(strRTrim(str));
}

int strFind(const char *string, const char *strCharSet,  int nPos)
{
	char *pdest;
	int  result=-1;
    if (nPos<(unsigned int)0) nPos=0;
    if (nPos>=(int)strlen(string)) return -1;
    pdest=(char*)strstr( string+nPos, strCharSet );
    result = pdest - string;
    if( pdest != NULL )
        return result;
    else
        return -1;
}
int chrFind(const char *string, const char CharSet, int nPos)
{
	char *pdest;
	int result;
    if (nPos<0) nPos=0;
    if (nPos>=strlen(string)) return -1;
    pdest=(char*)strchr( string+nPos, CharSet );
    result=-1;
    result = pdest - string;
    if( pdest != NULL )
        return result;
    else
        return -1;
}
int getKeyValue(const char *chString,char *dest,const char *chKey,const char ch) 
{
    int pos,pos1,pos2,idx,i;
    pos=strFind(chString,chKey,0);
    if (pos==-1) return -1;
    pos1=chrFind(chString,'=',pos);
    if (pos1==-1) return -1;
    pos2=chrFind(chString,ch,pos1);    
    if (pos2==-1) pos2=strlen(chString);
    idx = 0;
    for (i = pos1 + 1 ; i < pos2; i++)    
    {        
        if (chString[i] != '\0') {
            dest[idx] = chString[i];
            idx++;
        } else
            break;
    }
    dest[idx] = '\0';
    return 0;
}

char* getKV(const char*src,char *val,int len,const char *key,char ch_kv,char ch_lf)
{
	int nkey;
	int nval;
	char *pkey=NULL;
	const char*psrc=src;
	if(!val||!key||!*key) return NULL;
	nkey=strlen(key);
	do
	{
		pkey=strstr(psrc,key);
		if(!pkey) return NULL;
		psrc=pkey+nkey;
		if(pkey!=src && *(pkey-1)!=ch_lf && *(pkey+nkey)!=ch_kv)
		{
			continue;
		}
		psrc++;
		pkey=strchr(psrc,ch_lf);
		if(!pkey)
			nval=strlen(psrc);
		else
			nval=pkey-psrc;
		if(nval>=len)
		{
			plog("Buffer is not enough![%d>=%d]",nval,len);
			nval=len-1;
		}
		strncpy(val,psrc,nval);
		*(val+nval)=0;
		return val;
	}
	while(1);
	return NULL;
}

int get_piport(int fd,char *buf,int *port)
{
    //struct sockaddr_in addr;
	//sockaddr_in addr;
	SOCKADDR_IN addr;
    socklen_t len=sizeof(addr);
    int ret = getpeername(fd, (struct sockaddr *)&addr, &len);
    if(ret<0)
    {
        plog("Can't getpeername");
        return ret; 
    }
    strcpy(buf,inet_ntoa(addr.sin_addr));
    *port=ntohs(addr.sin_port);
    return 0;
}

int get_iport(int fd,char *buf,int *port)
{
    struct sockaddr_in addr;
    socklen_t len=sizeof(addr);
    int ret = getsockname(fd, (struct sockaddr *)&addr, &len);
    if(ret<0)
    {
        plog("Can't getsockname");
        return ret; 
    }
    strcpy(buf,inet_ntoa(addr.sin_addr));
    *port=ntohs(addr.sin_port);
    return 0;
}

char * getTimeStr(const time_t t)
{
    char *buf=(char*)malloc(200);
    struct tm tmnow=*localtime_r(&t,&tmnow);
    sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d",
        tmnow.tm_year+1900,
        tmnow.tm_mon+1,
        tmnow.tm_mday,
        tmnow.tm_hour,
        tmnow.tm_min,
        tmnow.tm_sec);
    return buf;
}

int setLogPath(const char *psz)
{
    MUTEX_LOCK(g_logmtx);
    FREE(g_logpath);
    if(psz)
    {
        g_logpath=(char *)malloc(strlen(psz)+1);
        strcpy(g_logpath,psz);
    }
    MUTEX_UNLOCK(g_logmtx);
    return 0;
}
char *getLogFilename(time_t t)
{
    if(NULL==g_logpath)
    {    
	    const char *sn="plog";
        struct tm tmnow=*localtime_r(&t,&tmnow);
		MUTEX_LOCK(g_logmtx);
        g_logpath=(char *)malloc(1024);
        //we don't need pid in filename

        sprintf(g_logpath,"%s_%04d%02d%02d"
                //"_%d"
                ".log",
            sn,
            tmnow.tm_year+1900,
            tmnow.tm_mon+1,
            tmnow.tm_mday
            //,(int)getpid()
            );
        MUTEX_UNLOCK(g_logmtx);
    }
    return g_logpath;
}


void plog_raw_stdout(const char * buf)
{

    char* tmstr=getTimeStr(time(0));
    unsigned int tid=(unsigned int )gettid();
#ifdef _WIN32
    printf("[%s][%d-%d] : %s\n",tmstr,GetCurrentProcessId(),tid,buf);
#else
	#ifdef ANDROID
		__android_log_print(ANDROID_LOG_INFO,"lib4b","[%s][%d-%d-%d] : %s\n",tmstr,getppid(),getpid(),tid,buf);
	#endif
	//#else
		printf("[%s][%d-%d-%d] : %s\n",tmstr,getppid(),getpid(),tid,buf);
	//#endif
#endif
    FREE(tmstr);
}

void plog_raw(const char * buf)
{
	FILE *fp;
	char *tmstr;
	unsigned int tid;
#ifdef ANDROID
	//just for  DEBUG, should remove this for release @ 20121019
	//char *logfile="/mnt/sdcard/bp.log";
	//char *logfile="/data/local/bp.log";
	const char *logfile=NULL;
#else
    const char *logfile=getLogFilename(time(0));
#endif
    if(!logfile) return ;
       
    fp=fopen(logfile,"a");
    tmstr=getTimeStr(time(0));
    tid=gettid();
    if(fp==NULL)
    {
        printf("Can't open logfile[%s] . quit!\n",logfile);
    }
    else
    {
#ifdef _WIN32
        fprintf(fp, "[%s][%d-%d] : %s\n",tmstr,GetCurrentProcessId(),tid,buf);
#else
        fprintf(fp, "[%s][%d-%d-%d] : %s\n",tmstr,getppid(),getpid(),tid,buf);
#endif
        fflush(fp);
    }
#ifdef _WIN32
    printf("[%s][%d-%d] : %s\n",tmstr,GetCurrentProcessId(),tid,buf);
#else
	#ifdef ANDROID
		__android_log_print(ANDROID_LOG_INFO,"plog","[%s][%d-%d-%d] : %s\n",tmstr,getppid(),getpid(),tid,buf);
	#else
		printf("[%s][%d-%d-%d] : %s\n",tmstr,getppid(),getpid(),tid,buf);
	#endif
#endif
    fclose(fp);
    free(tmstr);
}

void setLogUDP(char *ip)
{
	if(!ip|| *ip==0)
		*gLogUDP=0;
	else
		strcpy(gLogUDP,ip);
}
char *getLogUDP()
{
	if(*gLogUDP==0)
	{
		const char* p=getenv("UDP_LOG_IP");
		if(p)
		{
			strncpy(gLogUDP,p,sizeof(gLogUDP)-1);
		}
#if (defined(DEBUG) || defined(_DEBUG))
		else
		{
			strcpy(gLogUDP,"255.255.255.255");
		}
#endif
	}
	return gLogUDP;
}

void plog_udp(char *ip,short port,const char *buf)
{
	char sbuf[20000];
	static int sockfd=0;
	int nret = 0;
	struct sockaddr_in servaddr;
	char* tmstr;
	
#ifdef _WIN32
	memset(&servaddr,0,sizeof(servaddr));
#else
	bzero(&servaddr, sizeof(servaddr));
#endif
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr=inet_addr(ip);
	if(!sockfd)
	{
		int so_broadcast=1;
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd == -1) {
			printf ("creat socket error\n");
			return ;
		}
		if(SetSockOpt(sockfd,SOL_SOCKET,SO_BROADCAST,&so_broadcast,sizeof(so_broadcast))<0)
		{
			printf("Can't set broadcast option\n");
		}
	}
	tmstr=getTimeStr(time(0));
	sprintf(sbuf,"[%s][%d-%d] : %s\n",tmstr,getpid(),gettid(),buf);
	nret = sendto(sockfd, sbuf, strlen(sbuf) ,0,(struct sockaddr *)&servaddr, sizeof(servaddr));
	/* sockfd is a static variable: is threadsafe now????
#ifdef _WIN32
	closesocket(sockfd);
#else
	close(sockfd);
#endif
	*/
	free(tmstr);
}

void set_loglvl(int loglvl)
{
	g_loglvl=loglvl;
}
int get_loglvl()
{
	return g_loglvl;
}

/*
1.	loglvl control udp,file, stdout
2.	loglvl default to LOG_NONE
3.  if udp ip was set, then call plog_udp
4.  if log2file is non-zero, we will call getService() to retrive logfile, so DO NOT CALL any plog_xxx with log2file is NON-ZERO!!!!
5.  if compiled with MSVC in DEBUG mode, we will also output to debug view
6.  if log2file is non-zero, and compiled with ANDROID, we use TestMode as logfile, otherwise, we use Service->logpath
*/
void plog_wrap_fmt(int log2file,int loglevel,const char *file,const char *func,uint line,const char *fmt,...)
{
    char buf[20000];
    char fl[1000];
    va_list vl;
	char *uip=getLogUDP();
    
	if(g_loglvl < loglevel) return ;

    va_start(vl,fmt);
    vsnprintf(buf,sizeof(buf)-1000,fmt,vl);
    va_end(vl);
    sprintf(fl," (%s:%s:%d)",file,func,line);
    strcat(buf,fl);
#if defined(_WIN32) && defined(_DEBUG)
	{
		char wbuf[20000];
		char* tmstr=getTimeStr(time(0));
		unsigned int tid=(unsigned int )gettid();
        sprintf(wbuf, "[%s][%d-%d] : %s\n",tmstr,GetCurrentProcessId(),tid,buf);
		//OutputDebugString(wbuf);
		OutputDebugStringA(wbuf);
		FREE(tmstr);
	}
#endif

    if(log2file)
        plog_raw(buf);
    else
        plog_raw_stdout(buf);

	if (uip && *uip!=0)
	{
		plog_udp(uip,PORT_UDP_LOG,buf);
	}
}

unsigned long get_file_size(const char *filename)
{
    struct stat buf;
    if(stat(filename, &buf)!=0)
    {
        return 0;
    }
    return (unsigned long)buf.st_size;
}


int isExist(const char *filename)
{
    struct stat buf;
    if(stat(filename, &buf)!=0)
    {
        return 0;
    }
    return 1;
}

int isAbsolutePath(const char*path)
{
#ifdef _WIN32
	return path && ( *(path+1)==':' || (*path=='\\' && *(path+1)=='\\'));
#else
	return path && *path=='/';
#endif
}

int isDirectory(const char *filename)
{
    struct stat buf;
    if(stat(filename, &buf)!=0)
    {
        return 0;
    }
#ifdef _WIN32
	return (buf.st_mode & _S_IFDIR);
#else
    return S_ISDIR(buf.st_mode);
#endif
}

int select_socket(int sockfd, int timeout)
{
    int rc;
    struct timeval tv; 
	fd_set rfds,wfds;
	tv.tv_sec=(long) (timeout / 1000);
    tv.tv_usec=(long) (timeout % 1000 * 1000);

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_SET(sockfd,&rfds);
    FD_SET(sockfd,&wfds);
    rc=select(sockfd+1,&rfds,&wfds,0,&tv);
    if(rc==0) return 0;
    return (FD_ISSET(sockfd,&rfds)?READY_RECV:0) | (FD_ISSET(sockfd,&wfds)?READY_SEND:0);
}


int do_listen(unsigned short port)
{
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    int flag=1;
    int len=sizeof(int);
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port=htons(port);
    if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, len) == -1)
    {
        plog("setsockopt error!");
        exit(1);
    }
    bind(sockfd,(struct sockaddr *) &addr,sizeof(addr));
    listen(sockfd,1);
    plog("Listen @ %u Port,fd=%d",(unsigned int)port,sockfd);
    return sockfd;
}

int do_connect(int sockfd,const char *h,unsigned int p)
{
    /*
     * from: http://msdn.microsoft.com/en-us/library/windows/desktop/ms737625(v=vs.85).aspx
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(27015);
    */
    struct sockaddr_in addr;
	int len;
	int ret;
	int yes=1;
	// if (SetSockOpt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
        // plog("Reusing ADDR failed");
        // return -1;
    // }

    plog("Trying to connect to %s:%d ...",h,p);
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    //inet_pton(AF_INET,h,&addr.sin_addr.s_addr); //not ok for all platform
	//addr.sin_addr.S_un.S_addr = inet_addr(h);  //This is also OK
    addr.sin_addr.s_addr = inet_addr(h); //It's OK
    addr.sin_port=htons(p);
    len=sizeof(addr);
    ret=connect(sockfd,(const struct sockaddr*)&addr,len);
    if(ret!=0)
    {
#ifdef _WIN32
		plog("Connect to %s:%d failed!<%d>:<%d>",h,p,ret,WSAGetLastError());
#else
        plog("Connect to %s:%d failed!<%d>:<%s>",h,p,ret,strerror(errno));
#endif
	}
    else
    {
        plog("Connect to %s:%d ok!",h,p);
    }
    return ret;
}

#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year     = wtm.wYear - 1900;
    tm.tm_mon     = wtm.wMonth - 1;
    tm.tm_mday     = wtm.wDay;
    tm.tm_hour     = wtm.wHour;
    tm.tm_min     = wtm.wMinute;
    tm.tm_sec     = wtm.wSecond;
    tm. tm_isdst    = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif
long long getCurrentTimeMS()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    //printf("tv:%lld,%ld\n",(long long)tv.tv_sec*1000,tv.tv_usec/1000);
    return (long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000;
}

int set_timeout(int sockfd,int s,int us)
{
    struct timeval timeout;
    int ret=0;
    timeout.tv_sec = s;
    timeout.tv_usec = us;
    ret=SetSockOpt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
    ret=SetSockOpt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    return ret;
}

int set_keepalive(int fd)
{
    int optval=1;
    int optlen=sizeof(optval);
    int ret=SetSockOpt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen); 
#if !defined(_WIN32) && !defined(__APPLE__)
    optval=10;
    SetSockOpt(fd,SOL_TCP,TCP_KEEPCNT,&optval,optlen);
    optval=1;
    SetSockOpt(fd,SOL_TCP,TCP_KEEPIDLE,&optval,optlen);
    optval=1;
    SetSockOpt(fd,SOL_TCP,TCP_KEEPINTVL,&optval,optlen);
#endif
    return ret;
}

int set_nonblock(int fd)
{
#ifdef _WIN32
    int flags=1;
    int retval = ioctlsocket( fd, FIONBIO, (unsigned long *)&flags);
    if( retval == SOCKET_ERROR)
    {
        return -1;
    }
    return 0;
#else
    int flags;
    flags = fcntl(fd, F_GETFL);
    if (flags < 0)
    {
        return flags;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0)
    {
        return -1;
    }
    return 0;
#endif
}


int do_send(int sockfd,const char * buf,int len)
{
    int l=0;
    int err;
    //plog("Trying to send buffer[%s]\n",buf);
    while(l<len)
    {
        //ssize_t c=write(sockfd,buf+l,len-l);
		ssize_t c=send(sockfd,buf+l,len-l,0);
        if(c>0)
        {
            l+=c;
            continue;
        }
        if(c<0)
        {
            err=errno;
            plog("Write<%s><%d> sock<%d> error[%d]:[%s]!",buf,len,sockfd,err,strerror(err));
            return -1;
        }
        if(c==0)
        {
            err=errno;
            plog("Write<%s><%d> sock<%d> return 0,error[%s]!",buf,len,sockfd,strerror(err));
            continue;
        }
    }
    //plog("do_send(%s) is ok<%d>",buf,l);
    return l;
}


void slist_put( slist **ppHead,const char *key,const void *val,int len)
{
	if( !val || !ppHead)
	{
		plog("call %s(): Wrong arguments !",__FUNCTION__);
		return ;
	}
	else
	{
		slist *pHead=*ppHead;
		slist *node=( slist*)malloc(sizeof( slist));
		memset(node,0,sizeof(slist));
		if(len)
		{
			node->data=(void*)malloc(len);
			memset(node->data,0,len);
			memcpy(node->data,val,len);
		}
		else
			node->data=(void*)val;
		strncpy(node->key,key,sizeof(node->key)-1);
		node->len=len;

		node->next=pHead;
		*ppHead=node;
	}
}

void slist_free(struct slist *node)
{
	if(!node) return;
	if(node->next)
		slist_free(node->next);
	if(node->len)
		free(node->data);
	free(node);
}


int slist_size(slist **pHead)
{	
	slist *cnode;
	int ret=0;
	if(!pHead || !*pHead) return 0;
	cnode=*pHead;
	while(cnode)
	{
		ret++;
		cnode=cnode->next;
	}
	return ret;
}

slist * slist_find(slist **pHead,const char *key)
{
	slist *cnode;
	if(!pHead || !*pHead || !key || !*key) return NULL;
	cnode=*pHead;
	while(cnode)
	{
		if(strcasecmp(cnode->key,key)==0)
			return cnode;
		cnode=cnode->next;
	}
	return NULL;
}

void slist_dumps(slist **pHead)
{
	slist *cnode=*pHead;
	while(cnode)
	{
		plog("slist_node(0x%x): key=%s data=0x%x next:0x%x",cnode,cnode->key,cnode->data,cnode->next);
		cnode=cnode->next;
	}
}

//code is ugly, but I won't modify it.
struct slist * slist_get(struct slist **ppHead,const char *key)
{
	struct slist *node=*ppHead;
	//if don't have any nodes
	if(ppHead==NULL || node == NULL)
		return NULL;
	
	//make key '' same as null pointer
	if(key && *key!=0 && strcasecmp(key,node->key)==0)
	{
		//return the head node
		*ppHead=NULL;
		return node;
	}
	//if only one node
	if(node->next==NULL)
	{
		return NULL;
	}
	//at least two nodes
	while(1)
	{
		if(!node) break;
		if(key && *key!=0 )
		{
			if(node->next && strcasecmp(node->next->key,key)==0)
			{
				struct slist *ret=node->next;
				node->next=ret->next;
				ret->next=NULL;
				return ret;
			}
		}
		else //if key is empty or nullptr, return last node
		{
			if(node->next && node->next->next==NULL)
			{
				struct slist *ret=node->next;
				node->next=ret->next;
				ret->next=NULL;
				return ret;
			}
		}
		node=node->next;
	}
	return NULL;
}
