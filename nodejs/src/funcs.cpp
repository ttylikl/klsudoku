#include "common.h"

#ifdef _WIN32
#pragma comment (lib,"ws2_32.lib")
#endif

static int gInit=initLibrary();

//MutexHandle LOCK_VIEW;
//MutexHandle LOCK_LOG;
//MutexHandle LOCK_SOCK;

int initLibrary()
{
#ifdef _WIN32
    WSADATA WsaData;
    WSAStartup(MAKEWORD(2,2),&WsaData);
    //LOCK_VIEW=CreateMutex(0,0,0);
    //LOCK_LOG=CreateMutex(0,0,0);
    //LOCK_SOCK=CreateMutex(0,0,0);
#else
    //pthread_mutex_init(&LOCK_VIEW,0);
    //pthread_mutex_init(&LOCK_LOG,0);
    //pthread_mutex_init(&LOCK_SOCK,0);
#endif
    return 0;
}

int uninitLibrary()
{
    return 0;
}

int isValidNum(int num)
{
    return (1 <=num && num<=NUM );
}

int isValidCord(int cord)
{
    return (0 <=cord&& cord<NUM );
}

int num2cord(int num)
{
    KAssertRet(isValidNum(num),plog("Trying to convert number[%d] to coordinate",num),-1);
    return num-1;
}
int cord2num(int cord)
{
    KAssertRet(isValidCord(cord),plog("Trying to convert coordinate[%d] to number",cord),-1);
    return cord+1;
}

int cord2boxid(SPoint &pt)
{
	return cord2boxid(pt.getx(),pt.gety());
}

int cord2boxid(int cx,int cy)
{
	int cb = (cx / BOXNUM) * BOXNUM + (cy / BOXNUM);
    //plog("(cx,cy)[%d,%d] -> cb[%d]",cx,cy,cb);
    return cb;
}

char * getTimeStr(const time_t t,char *buf,int len)
{
#ifdef _WIN32
    #ifdef _MSC_VER
        struct tm tmnow;
        localtime_s(&tmnow,&t);
    #else
        struct tm tmnow=*localtime(&t);
    #endif
#else
    struct tm tmnow=*localtime_r(&t,&tmnow);
#endif
	sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d",
		tmnow.tm_year+1900,
		tmnow.tm_mon+1,
		tmnow.tm_mday,
		tmnow.tm_hour,
		tmnow.tm_min,
		tmnow.tm_sec);
	return buf;
}


int create_multicast_socket(const char *group,unsigned int port)
{
	SOCKADDR_IN addr;
	struct ip_mreq mreq;
	int fd;

	u_int yes=1;            /*** MODIFICATION TO ORIGINAL */

	/* create what looks like an ordinary UDP socket */
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		plog("socket");
		return 0;
	}
	/**** MODIFICATION TO ORIGINAL */
	/* allow multiple sockets to use the same PORT number */
	if (SetSockOpt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
		plog("Reusing ADDR failed");
		return 0;
	}
	/*** END OF MODIFICATION TO ORIGINAL */

	/* set up destination address */
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
	addr.sin_port=htons(port);

	/* bind to receive address */
	if (::bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
		plog("bind");
		return 0;
	}

	/* use SetSockOpt() to request that the kernel join a multicast group */
	mreq.imr_multiaddr.s_addr=inet_addr(group);
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	if (SetSockOpt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
		plog("SetSockOpt: IP_ADD_MEMBERSHIP");
		return 0;
	}

	if (SetSockOpt(fd,IPPROTO_IP,IP_MULTICAST_LOOP,&yes,sizeof(yes)) < 0) {
		plog("SetSockOpt");
		return 0;
	}
	return fd; 
}


//IMPORTANT: do not use any plog functions here, because plog will call this!!!!!!!!!!!!!!!
//IMPORTANT: do not use any plog functions here, because plog will call this!!!!!!!!!!!!!!!
//IMPORTANT: do not use any plog functions here, because plog will call this!!!!!!!!!!!!!!!
//IMPORTANT: do not use any plog functions here, because plog will call this!!!!!!!!!!!!!!!
int create_udp_socket(const char *group,unsigned int port)
{
	SOCKADDR_IN addr;
	//struct ip_mreq mreq;
	//ip_mreq mreq;
	int fd;

	u_int yes=1;            /*** MODIFICATION TO ORIGINAL */

	/* create what looks like an ordinary UDP socket */
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		printf("socket");
		return 0;
	}
	/**** MODIFICATION TO ORIGINAL */
	/* allow multiple sockets to use the same PORT number */
	if (SetSockOpt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
		printf("Reusing ADDR failed");
		return 0;
	}
	/*** END OF MODIFICATION TO ORIGINAL */

	if(group && port)
	{
		/* set up destination address */
		memset(&addr,0,sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_addr.s_addr=inet_addr(group); /* N.B.: differs from sender */ //INADDR_ANY
		addr.sin_port=htons(port);

		/* bind to receive address */
		if (::bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
			printf("bind error");
			return fd;
		}
	}
	int so_broadcast=1;
	if(SetSockOpt(fd,SOL_SOCKET,SO_BROADCAST,&so_broadcast,sizeof(so_broadcast))<0)
	{
		printf("Can't set broadcast option\n");
	}
	return fd; 
}

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

int isPhysicalLocalMac( const char *szDesc )
{
	HKEY hKEY;
	TCHAR strKey[1024];
	TCHAR wbuf[100];
	long nRet;
	TCHAR szData[256] = { 0 };
	unsigned long dwType = REG_SZ;
	unsigned long dwDataSize = 256;
	lstrcpy(strKey, TEXT("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\"));
#if defined(UNICODE)
	MultiByteToWideChar(CP_ACP, 0, szDesc, -1, wbuf , 100);
#else
	strcpy(wbuf,szDesc);
#endif
	lstrcat(strKey, wbuf);
	lstrcat(strKey, TEXT("\\Connection"));
	nRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hKEY );

	if ( nRet != ERROR_SUCCESS )
	{
		return FALSE;
	}

	nRet = RegQueryValueEx( hKEY, TEXT("PnPInstanceId"), NULL, &dwType, (LPBYTE)szData, &dwDataSize );
	RegCloseKey(hKEY);
	if ( nRet != ERROR_SUCCESS )
	{            
		return FALSE;
	}
	
	TCHAR pci[10];
	lstrcpyn(pci, szData, 4);
	if(lstrcmpi(pci,TEXT("PCI"))==0)
		return TRUE;
	return FALSE;
}


int getMacAddressWindows(char * buf)
{
    char buffer[32];
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    UINT i;
    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);

    *buf='\0';

/* variables used to print DHCP time info */

    pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        plog("Error allocating memory needed to call GetAdaptersinfo");
        return 1;
    }
// Make an initial call to GetAdaptersInfo to get
// the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            plog("Error allocating memory needed to call GetAdaptersinfo");
            return 1;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
			if (pAdapter->Type != MIB_IF_TYPE_ETHERNET && pAdapter->Type != IF_TYPE_IEEE80211)
            {
                pAdapter = pAdapter->Next;
                continue;
            }
			if (strstr(pAdapter->Description, "TAP-Win32") != NULL)
			{
				pAdapter = pAdapter->Next;
                continue;
			}
			if (strstr(pAdapter->Description, "Virtual") != NULL)
			{
				pAdapter = pAdapter->Next;
				continue;
			}
			if(!isPhysicalLocalMac(pAdapter->AdapterName))
			{
				pAdapter = pAdapter->Next;
				continue;
			}
            for (i = 0; i < pAdapter->AddressLength; i++) {
                sprintf(buffer,"%.2X", (int) pAdapter->Address[i]);
                strcat(buf,buffer);
            }
            //pAdapter = pAdapter->Next;
            break;
        }
        free(pAdapterInfo);
        return 0;
    }
    return 1;
}
#elif defined(ANDROID)
int getMacAddressLinux(char *buf)
{
    //plog("getMacAddressLinux");
    struct ifreq ifr;
    struct ifconf ifc;

    //int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) { 
        plog("Can't create socket! ");
        return -1;
    }

    //SIOCGSIZIFCONF  Gets the memory required to get configuration information for all interfaces returned by SIOCGIFCONF.
    //int ifconfsize;
    //ioctl(sock, SIOCGSIZIFCONF, (caddr_t)&ifconfsize);

    int ifconfsize=1024*10;
    char * ibuf=(char *) malloc(ifconfsize);

    //ifc.ifc_len = sizeof(buf);
    ifc.ifc_len = ifconfsize;
    
    ifc.ifc_buf = ibuf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
        plog("Can't ioctl with SIOCGIFCONF !");
        close(sock);
        free(ibuf);
        return -2;
    }
    struct ifreq *it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));
    char mac[32];
    int i;
    for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (!(ifr.ifr_flags & IFF_LOOPBACK)) {
                //except tun/tap interface
                if(strncmp(ifr.ifr_name,"tun",3) ==0 ||
                    strncmp(ifr.ifr_name,"tap",3) ==0 
                  )
                {
                    plog("Skipped interface:%s",ifr.ifr_name);
                    continue;
                }
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    unsigned char mac_address[6];
                    memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
                    for (i=0; i<6; i++)
                        sprintf(mac+2*i, "%02x", (unsigned char)ifr.ifr_hwaddr.sa_data[i]);
                    plog("%s=%s",ifr.ifr_name,mac);
                    if(strcmp(mac,"000000000000")!=0)
                    {
                        strcpy(buf,mac);
                        close(sock);
                        free(ibuf);
                        return 0;
                    }
                    continue;
                }
                plog("can't get HW:%s",ifr.ifr_name);
            }
        }
        else { 
            plog("%s ioctl failed",ifr.ifr_name);
        }
    }
    plog("Can't retrive MAC address!");
    close(sock);
    free(ibuf);
    return -3; 
}
#else
//TODO:for Darwin
#endif

string getMacAddress()
{
    char buf[128];
#ifdef _WIN32
    /*
    UUID uuid;
    if(UuidCreateSequential(&uuid) == RPC_S_UUID_NO_ADDRESS)
        strcpy(buf,"UnknownID");
    else
        memcpy(buf, (char*)(uuid.Data4+2), 6);
        */
    getMacAddressWindows(buf);
#elif defined (ANDROID)
    getMacAddressLinux(buf);
    /*FILE *f=popen("cat /sys/class/net/eth0/address|sed -e 's/://g'","r");
    int r=read(fileno(f),buf,32);
    *(buf+12)=0;
    pclose(f);
    */
#else
    strcpy(buf,"TODO");
#endif
    return buf;
}
