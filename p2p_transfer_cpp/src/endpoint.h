#ifndef __ENDPOINT_H__
#define __ENDPOINT_H__

#include "common.h"

#ifndef WIN32
#define SOCKET int
#endif

#define UDP_PACKET_SIZE 1400
namespace PT{
/*EndPoint定义*/
class ReceiveWork;
class EndPoint{
public:
	EndPoint();
	EndPoint(UInt32 host,UInt32 port);
	virtual ~EndPoint();
	void start();
	void shutdown();
	void waitForEnd();
	virtual void receiveData(char* data,int length,UInt32 host,UInt32 port)=0;
	void sendData(char* data,int length,UInt32 host,UInt32 port);
	inline UInt32 getHost(){
		return m_host;
	}
	inline UInt32 getPort(){
		return m_port;
	}
private:
	void getLocalIp();
#ifdef WIN32
	void initNetwork();
#endif
private:
	UInt32 m_host;
	UInt32 m_port;
	SOCKET m_sockfd;
	ReceiveWork* m_pRw;
};
/*ReceiveWork定义*/
class ReceiveWork:public DoRepeatWork{
public:
	ReceiveWork(SOCKET sockfd,EndPoint* pEp);
	~ReceiveWork();
	void start();
	void stop();
	void waitForEnd();
	virtual void doRepeatWork();
private:
	SOCKET m_sockfd;
    EndPoint* m_pEp;
    SimpleLogger m_logger;
    RepeatWork m_repeatWork;
};
}
#endif
