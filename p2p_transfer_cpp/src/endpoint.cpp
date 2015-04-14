#ifndef WIN32
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include <errno.h>
#include "endpoint.h"
#ifdef WIN32
#pragma comment(lib,"Ws2_32.lib")
#endif

/*EndPoint实现*/
EndPoint::EndPoint():m_host(0),m_port(0),m_sockfd(0),m_pRw(NULL){
}
EndPoint::EndPoint(UInt32 host,UInt32 port):m_host(host),m_port(port),m_sockfd(0),m_pRw(NULL){
}
EndPoint::~EndPoint(){
	if(m_pRw!=NULL){
		delete m_pRw;
	}
}
void EndPoint::start(){
#ifdef WIN32
	initNetwork();
#endif
	m_sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(m_sockfd!=-1){
    	if(m_host==0){
    		getLocalIp();
    	}
		struct sockaddr_in addr={0};
		const char opt=1;
    	setsockopt(m_sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
		int nRecvBuf=400*1024;
        setsockopt(m_sockfd,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
    	addr.sin_family=AF_INET;
    	addr.sin_addr.s_addr=m_host;
    	addr.sin_port=htons(m_port);
    	int rt=bind(m_sockfd,(struct sockaddr *)&addr,sizeof(addr));
    	if(m_port==0){
#ifndef WIN32
		    socklen_t len;
#else
		    int len;
#endif
    		len=sizeof(struct sockaddr_in);
    	    getsockname(m_sockfd,(struct sockaddr *)&addr,&len);
    	    m_port=htons(addr.sin_port);
    	}
    	if(rt!=-1){
    		m_pRw=new ReceiveWork(m_sockfd,this);
    		m_pRw->start();
    	}else{
#ifndef WIN32
    		string errorMsg="bind socket error:"+string(strerror(errno));
#else
    		string errorMsg="bind socket error";
#endif
    		PTException ex(PTException::BIND_SOCKET_ERROR,errorMsg);
    		throw ex;
    	}
    }else{
#ifndef WIN32
    	string errorMsg="create socket error:"+string(strerror(errno));
#else
    	string errorMsg="create socket error";
#endif
    	PTException ex(PTException::BIND_SOCKET_ERROR,errorMsg);
    	throw ex;
    }
}
void EndPoint::shutdown(){
	if(m_pRw!=NULL){
		m_pRw->stop();
	}
}
void EndPoint::waitForEnd(){
	m_pRw->waitForEnd();
}
void EndPoint::sendData(char* data,int length,UInt32 host,UInt32 port){
	struct sockaddr_in addr={0};
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=host;
	addr.sin_port=htons(port);
	sendto(m_sockfd,data,length,0,(struct sockaddr*)&addr,sizeof(addr));
}
void EndPoint::getLocalIp(){
#ifndef WIN32
	struct ifreq buf[100];
	struct ifconf ifc;
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t) buf;
	ioctl (m_sockfd, SIOCGIFCONF, (char *) &ifc);
	int count;
	count = ifc.ifc_len/sizeof(struct ifreq);
	int i;
	string ip;
	for(i=0;i<count;i++){
		sockaddr_in addr;
		memcpy(&addr,&buf[i].ifr_addr,sizeof(buf[i].ifr_addr));
		ip=inet_ntoa(addr.sin_addr);
		if(ip!="127.0.0.1"){
			m_host=CommonUtils::toIpInt(ip);
			break;
		}
	}
#else
	char local[255] = {0};  
    gethostname(local, sizeof(local));
    hostent* ph = gethostbyname(local);  
    if (ph == NULL)return;
    in_addr addr;  
    memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr));
    m_host=CommonUtils::toIpInt(inet_ntoa(addr));
#endif
}
#ifdef WIN32
void EndPoint::initNetwork(){
   WORD wVersionRequested = MAKEWORD( 2, 2 );
   WSADATA wsaData;
   int err;
   err = WSAStartup( wVersionRequested, &wsaData );
   if ( err != 0 ){
	   string errorMsg="could not load win sock";
	   PTException ex(PTException::CREATE_SOCKET_ERROR,errorMsg);
       throw ex;
   }
   if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) {
	   WSACleanup();
       string errorMsg="Bad win sock version";
       PTException ex(PTException::CREATE_SOCKET_ERROR,errorMsg);
       throw ex;
   }   
}
#endif
/*ReceiveWork实现*/
ReceiveWork::ReceiveWork(SOCKET sockfd,EndPoint* pEp):DoRepeatWork(),m_sockfd(sockfd),m_pEp(pEp),m_logger("endpoint.ReceiveWork"),m_repeatWork(this,&m_logger){
}
ReceiveWork::~ReceiveWork(){
}
void ReceiveWork::start(){
	m_repeatWork.start();
}
void ReceiveWork::stop(){
	m_repeatWork.stop();
	char closeData[1];
	closeData[0]=16;//00010000,第4位为1代表关闭socket,协议中第4位未用
	m_pEp->sendData(closeData,1,m_pEp->getHost(),m_pEp->getPort());//recvfrom方法处于阻塞状态，通过向其发数据，通知结束
}
void ReceiveWork::waitForEnd(){
	m_repeatWork.waitForEnd();
}
void ReceiveWork::doRepeatWork(){
	struct sockaddr_in addr;
#ifndef WIN32
	socklen_t addrlen;
#else
	int addrlen;
#endif
	addrlen=sizeof(addr);
	char buf[UDP_PACKET_SIZE];
	int len;
	memset(buf,0,UDP_PACKET_SIZE);
	len=recvfrom(m_sockfd,buf,sizeof(buf),0,(struct sockaddr*)&addr,&addrlen);
	if (len>0){
		if(len==1&&buf[0]==16){
			if(addr.sin_addr.s_addr==m_pEp->getHost()&&htons(addr.sin_port)==m_pEp->getPort()){
#ifndef WIN32
	    close(m_sockfd);
#else
	    closesocket(m_sockfd);
#endif
			}
		}else{
			m_pEp->receiveData(buf,len,addr.sin_addr.s_addr,htons(addr.sin_port));
		}
	}
}
