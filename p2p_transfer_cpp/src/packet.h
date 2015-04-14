#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
namespace PT{
enum CotrolPacketType{
	LOGIN_SERVER=0,
	LOGIN_SERVER_ACK,
	LOGIN_PEER,
	LOGIN_PEER_ACK,
	HEART,
	HEART_ACK,
	ACK,
	JUDGE_NAT,
	JUDGE_NAT_ACK,
	REPORT,
	REPORT_ACK,
	WANT_HOLE,
	WANT_HOLE_ACK,
	NOTIFY_HOLE,
	HOLE,
	LOGOUT,
};
enum HoleDirection {
	To_SRC,//向发起方打洞
	To_DEST,//向目标方打洞
	CAN_NOT_HOLE,//不能打洞,打洞也无法建立连接的情况
};
/*Packet定义*/
class Packet{
public:
	Packet(UInt32 host,UInt32 port);
	virtual ~Packet();
	virtual int getBytesLen()=0;
	virtual void readBytes(char* dest)=0;
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getHost(){
		return m_host;
	}
	inline void setHost(UInt32 host){
		m_host=host;
	}
	inline UInt32 getPort(){
		return m_port;
	}
	inline void setPort(UInt32 port){
		m_port=port;
	}
	inline bool isTurn(){
		return m_isTurn;
	}
	inline bool isControl(){
		return m_isControl;
	}
	inline bool isTurnHeart(){
		return isTurnHeart();
	}
	inline int getCotrolPacketType(){
		return m_controlPacketType;
	}
protected:
	UInt32 m_host;
	UInt32 m_port;
	bool m_isTurn;
	bool m_isControl;
	bool m_isTurnHeart;
	int m_controlPacketType;
	SimpleLogger m_logger;
};
/*DataPacket定义*/
class DataPacket:public Packet{
public:
	DataPacket(UInt32 host,UInt32 port);
	~DataPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getPacketSeqNo(){
		return m_packetSeqNo;
	}
	inline void setPacketSeqNo(UInt32 packetSeqNo){
		m_packetSeqNo=packetSeqNo;
	}
	inline int getWrapDataLen(){
		return m_wrapDataLen;
	}
	inline void readWrapData(char* dest,int start){
        memcpy(dest+start,m_wrapData,m_wrapDataLen);
	}
	inline void writeWrapData(const char* src,int start,int len){
		m_wrapDataLen=len;
		m_wrapData=new char[len];
		memcpy(m_wrapData,src+start,len);
	}
public:
	static const int dataPacketHeadSize;
private:
	UInt32 m_packetSeqNo;
	char* m_wrapData;
	int m_wrapDataLen;
};
/*LoginServerPacket定义*/
class LoginServerPacket:public Packet{
public:
	LoginServerPacket(UInt32 host,UInt32 port);
	~LoginServerPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline void setLocalHost(UInt32 localHost){
		m_localHost=localHost;
	}
	inline UInt32 getLocalHost(){
		return m_localHost;
	}
	inline void setLocalPort(UInt32 localPort){
		m_localPort=localPort;
	}
	inline UInt32 getLocalPort(){
		return m_localPort;
	}
public:
	static const int loginServerPacketSize;
private:
	UInt32 m_localHost;
	UInt32 m_localPort;
};
/*LoginServerAckPacket定义*/
class LoginServerAckPacket:public Packet{
public:
	LoginServerAckPacket(UInt32 host,UInt32 port);
	~LoginServerAckPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getServerJudgeNatPort1(){
		return m_serverJudgeNatPort1;
	}
	inline void setServerJudgeNatPort1(UInt32 serverJudgeNatPort1){
		m_serverJudgeNatPort1=serverJudgeNatPort1;
	}
	inline UInt32 getServerJudgeNatPort2(){
		return m_serverJudgeNatPort2;
	}
	inline void setServerJudgeNatPort2(UInt32 serverJudgeNatPort2){
		m_serverJudgeNatPort2=serverJudgeNatPort2;
	}
	inline UInt32 getServerTurnPort(){
		return m_serverTurnPort;
	}
	inline void setServerTurnPort(UInt32 serverTurnPort){
		m_serverTurnPort=serverTurnPort;
	}
	inline UInt32 getWanHost(){
		return m_wanHost;
	}
	inline void setWanHost(UInt32 wanHost){
		m_wanHost=wanHost;
	}
	inline UInt32 getWanPort(){
		return m_wanPort;
	}
	inline void setWanPort(UInt32 wanPort){
		m_wanPort=wanPort;
	}
public:
	static const int loginServerAckPacketSize;
private:
	UInt32 m_serverJudgeNatPort1;
	UInt32 m_serverJudgeNatPort2;
	UInt32 m_serverTurnPort;
	UInt32 m_wanHost;
	UInt32 m_wanPort;
};
/*LoginPeerPacket定义*/
class LoginPeerPacket:public Packet{
public:
	LoginPeerPacket(UInt32 host,UInt32 port);
	~LoginPeerPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getWanHost(){
		return m_wanHost;
	}
	inline void setWanHost(UInt32 wanHost){
		m_wanHost=wanHost;
	}
	inline UInt32 getWanPort(){
		return m_wanPort;
	}
	inline void setWanPort(UInt32 wanPort){
		m_wanPort=wanPort;
	}
	inline UInt32 getLocalHost(){
		return m_localHost;
	}
	inline void setLocalHost(UInt32 localHost){
		m_localHost=localHost;
	}
	inline UInt32 getLocalPort(){
		return m_localPort;
	}
	inline void setLocalPort(UInt32 localPort){
		m_localPort=localPort;
	}
	inline UInt8 getLoginMode(){
		return m_loginMode;
	}
	inline void setLoginMode(UInt8 loginMode){
		m_loginMode=loginMode;
	}
public:
	static const int loginPeerPacketSize;
private:
	UInt32 m_wanHost;
	UInt32 m_wanPort;
	UInt32 m_localHost;
	UInt32 m_localPort;
	UInt8 m_loginMode;
};
/*LoginPeerAckPacket定义*/
class LoginPeerAckPacket:public Packet{
public:
	LoginPeerAckPacket(UInt32 host,UInt32 port);
	~LoginPeerAckPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getWanHost(){
		return m_wanHost;
	}
	inline void setWanHost(UInt32 wanHost){
		m_wanHost=wanHost;
	}
	inline UInt32 getWanPort(){
		return m_wanPort;
	}
	inline void setWanPort(UInt32 wanPort){
		m_wanPort=wanPort;
	}
	inline UInt32 getLocalHost(){
		return m_localHost;
	}
	inline void setLocalHost(UInt32 localHost){
		m_localHost=localHost;
	}
	inline UInt32 getLocalPort(){
		return m_localPort;
	}
	inline void setLocalPort(UInt32 localPort){
		m_localPort=localPort;
	}
	inline UInt8 getLoginMode(){
		return m_loginMode;
	}
	inline void setLoginMode(UInt8 loginMode){
		m_loginMode=loginMode;
	}
public:
	static const int loginPeerAckPacketSize;
private:
	UInt32 m_wanHost;
	UInt32 m_wanPort;
	UInt32 m_localHost;
	UInt32 m_localPort;
	UInt8 m_loginMode;
};
/*HeartPacket定义*/
class HeartPacket:public Packet{
public:
	HeartPacket(UInt32 host,UInt32 port);
	~HeartPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
public:
	static const int heartPacketSize;
};
/*HeartAckPacket定义*/
class HeartAckPacket:public Packet{
public:
	HeartAckPacket(UInt32 host,UInt32 port);
	~HeartAckPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
public:
	static const int heartAckPacketSize;
};
/*AckPacket定义*/
class AckPacket:public Packet{
public:
	AckPacket(UInt32 host,UInt32 port);
	~AckPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getPacketSeqNo(){
		return m_packetSeqNo;
	}
	inline void setPacketSeqNo(UInt32 packetSeqNo){
		m_packetSeqNo=packetSeqNo;
	}
public:
	static const int ackPacketSize;
private:
	UInt32 m_packetSeqNo;
};
/*JudgeNatPacket定义*/
class JudgeNatPacket:public Packet{
public:
	JudgeNatPacket(UInt32 host,UInt32 port);
	~JudgeNatPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getWanHost(){
		return m_wanHost;
	}
	inline void setWanHost(UInt32 wanHost){
		m_wanHost=wanHost;
	}
	inline UInt32 getWanPort(){
		return m_wanPort;
	}
	inline void setWanPort(UInt32 wanPort){
		m_wanPort=wanPort;
	}
public:
	static const int judgeNatPacketSize;
private:
	UInt32 m_wanHost;
	UInt32 m_wanPort;
};
/*JudgeNatAckPacket定义*/
class JudgeNatAckPacket:public Packet{
public:
	JudgeNatAckPacket(UInt32 host,UInt32 port);
	~JudgeNatAckPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getWanHost(){
		return m_wanHost;
	}
	inline void setWanHost(UInt32 wanHost){
		m_wanHost=wanHost;
	}
	inline UInt32 getWanPort(){
		return m_wanPort;
	}
	inline void setWanPort(UInt32 wanPort){
		m_wanPort=wanPort;
	}
public:
	static const int judgeNatAckPacketSize;
private:
	UInt32 m_wanHost;
	UInt32 m_wanPort;
};
/*ReportPacket定义*/
class ReportPacket:public Packet{
public:
	ReportPacket(UInt32 host,UInt32 port);
	~ReportPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt8 getNatType(){
		return m_natType;
	}
	inline void setNatType(UInt8 natType){
		m_natType=natType;
	}
public:
	static const int reportPacketSize;
private:
	UInt8 m_natType;
};
/*ReportAckPacket定义*/
class ReportAckPacket:public Packet{
public:
	ReportAckPacket(UInt32 host,UInt32 port);
	~ReportAckPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
public:
	static const int reportAckPacketSize;
};
/*WantHolePacket定义*/
class WantHolePacket:public Packet{
public:
	WantHolePacket(UInt32 host,UInt32 port);
	~WantHolePacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getPeerHost(){
		return m_peerHost;
	}
	inline void setPeerHost(UInt32 peerHost){
		m_peerHost=peerHost;
	}
	inline UInt32 getPeerPort(){
		return m_peerPort;
	}
	inline void setPeerPort(UInt32 peerPort){
		m_peerPort=peerPort;
	}
	inline UInt32 getPeerLocalHost(){
		return m_peerLocalHost;
	}
	inline void setPeerLocalHost(UInt32 peerLocalHost){
		m_peerLocalHost=peerLocalHost;
	}
	inline UInt32 getPeerLocalPort(){
		return m_peerLocalPort;
	}
	inline void setPeerLocalPort(UInt32 peerLocalPort){
		m_peerLocalPort=peerLocalPort;
	}
public:
	static const int wantHolePacketSize;
private:
	UInt32 m_peerHost;
	UInt32 m_peerPort;
	UInt32 m_peerLocalHost;
	UInt32 m_peerLocalPort;
};
/*WantHolePacket定义*/
class WantHoleAckPacket:public Packet{
public:
	WantHoleAckPacket(UInt32 host,UInt32 port);
	~WantHoleAckPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getPeerHost(){
		return m_peerHost;
	}
	inline void setPeerHost(UInt32 peerHost){
		m_peerHost=peerHost;
	}
	inline UInt32 getPeerPort(){
		return m_peerPort;
	}
	inline void setPeerPort(UInt32 peerPort){
		m_peerPort=peerPort;
	}
	inline UInt32 getPeerLocalHost(){
		return m_peerLocalHost;
	}
	inline void setPeerLocalHost(UInt32 peerLocalHost){
		m_peerLocalHost=peerLocalHost;
	}
	inline UInt32 getPeerLocalPort(){
		return m_peerLocalPort;
	}
	inline void setPeerLocalPort(UInt32 peerLocalPort){
		m_peerLocalPort=peerLocalPort;
	}
	inline UInt8 getDirection(){
		return m_direction;
	}
	inline void setDirection(UInt8 direction){
		m_direction=direction;
	}
public:
	static const int wantHoleAckPacketSize;
private:
	UInt32 m_peerHost;
	UInt32 m_peerPort;
	UInt32 m_peerLocalHost;
	UInt32 m_peerLocalPort;
	UInt8 m_direction;
};
/*NotifyHolePacket定义*/
class NotifyHolePacket:public Packet{
public:
	NotifyHolePacket(UInt32 host,UInt32 port);
	~NotifyHolePacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline UInt32 getPeerHost(){
		return m_peerHost;
	}
	inline void setPeerHost(UInt32 peerHost){
		m_peerHost=peerHost;
	}
	inline UInt32 getPeerPort(){
		return m_peerPort;
	}
	inline void setPeerPort(UInt32 peerPort){
		m_peerPort=peerPort;
	}
	inline UInt32 getPeerLocalHost(){
		return m_peerLocalHost;
	}
	inline void setPeerLocalHost(UInt32 peerLocalHost){
		m_peerLocalHost=peerLocalHost;
	}
	inline UInt32 getPeerLocalPort(){
		return m_peerLocalPort;
	}
	inline void setPeerLocalPort(UInt32 peerLocalPort){
		m_peerLocalPort=peerLocalPort;
	}
	inline UInt8 getDirection(){
		return m_direction;
	}
	inline void setDirection(UInt8 direction){
		m_direction=direction;
	}
public:
	static const int notifyHolePacketSize;
private:
	UInt32 m_peerHost;
	UInt32 m_peerPort;
	UInt32 m_peerLocalHost;
	UInt32 m_peerLocalPort;
	UInt8 m_direction;
};
/*HolePacket定义*/
class HolePacket:public Packet{
public:
	HolePacket(UInt32 host,UInt32 port);
	~HolePacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
public:
	static const int holePacketSize;
};
/*LogoutPacket定义*/
class LogoutPacket:public Packet{
public:
	LogoutPacket(UInt32 host,UInt32 port);
	~LogoutPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
public:
	static const int logoutPacketSize;
};
/*TurnHeartPacket定义*/
class TurnHeartPacket:public Packet{
public:
	TurnHeartPacket(UInt32 host,UInt32 port);
	~TurnHeartPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
public:
	static const int turnHeartPacketSize;
};
/*TurnDataPacket定义*/
class TurnDataPacket:public Packet{
public:
	TurnDataPacket(UInt32 host,UInt32 port);
	~TurnDataPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline void setDataPacket(DataPacket* pDp){
		m_pDp=pDp;
	}
	inline DataPacket* getDataPacket(){
		return m_pDp;
	}
public:
	static const int turnDataPacketHeadSize;
private:
	DataPacket* m_pDp;
};
/*TurnControlPacket定义*/
class TurnControlPacket:public Packet{
public:
	TurnControlPacket(UInt32 host,UInt32 port);
	~TurnControlPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline void setPacket(Packet* pP){
		m_pP=pP;
	}
	inline Packet* getPacket(){
		return m_pP;
	}
private:
	void newPacket(UInt32 peerHost,UInt32 peerPort);
private:
	Packet* m_pP;
};
}
#endif
