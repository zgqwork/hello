#ifndef __NODE_H__
#define __NODE_H__

#include "packet.h"
#include "endpoint.h"
namespace PT{
/*AdapterNode*/
class AdapterNode{
public:
	AdapterNode();
	virtual ~AdapterNode();
	virtual void processLoginServerPacket(LoginServerPacket* pPacket);
	virtual void processLoginServerAckPacket(LoginServerAckPacket* pPacket);
	virtual void processLoginPeerPacket(LoginPeerPacket* pPacket);
	virtual void processLoginPeerAckPacket(LoginPeerAckPacket* pPacket);
	virtual void processHeartPacket(HeartPacket* pPacket);
	virtual void processHeartAckPacket(HeartAckPacket* pPacket);
	virtual void processAckPacket(AckPacket* pPacket);
	virtual void processJudgeNatPacket(JudgeNatPacket* pPacket);
	virtual void processJudgeNatAckPacket(JudgeNatAckPacket* pPacket);
	virtual void processReportPacket(ReportPacket* pPacket);
	virtual void processReportAckPacket(ReportAckPacket* pPacket);
	virtual void processWantHolePacket(WantHolePacket* pPacket);
	virtual void processWantHoleAckPacket(WantHoleAckPacket* pPacket);
	virtual void processNotifyHolePacket(NotifyHolePacket* pPacket);
	virtual void processHolePacket(HolePacket* pPacket);
	virtual void processLogoutPacket(LogoutPacket* pPacket);
	virtual void processDataPacket(DataPacket* pPacket);
};
/*NodeEndPoint*/
class NodeEndPoint:public EndPoint{
public:
	NodeEndPoint(AdapterNode* pNode);
	NodeEndPoint(UInt32 host,UInt32 port,AdapterNode* pNode);
	~NodeEndPoint();
	virtual void receiveData(char* data,int length,UInt32 host,UInt32 port);
private:
	Packet* newControlPacket(char* data,int length,UInt32 host,UInt32 port);
	void processControlPacket(Packet* pPacket);
private:
	AdapterNode* m_pNode;
	SimpleLogger m_logger;
};
/*ProcessDataPacket*/
class ProcessDataPacket:public Runnable{
public:
	ProcessDataPacket(AdapterNode* pNode,DataPacket* pPacket):m_pNode(pNode),m_pPacket(pPacket){
	}
	virtual void run(){
		m_pNode->processDataPacket(m_pPacket);
	}
private:
	AdapterNode* m_pNode;
	DataPacket* m_pPacket;
};
/*NodeHandler*/
class NodeHandler{
public:
	virtual void receiveData(char* data,int len,const PeerInfo& peerInfo)=0;
	virtual void onPeerLogin(const PeerInfo& peerInfo)=0;
	virtual void onPeerLogout(const PeerInfo& peerInfo)=0;
	virtual void onDisconnectToPeer(const PeerInfo& peerInfo)=0;
};
/*PeerNodeHandler*/
class PeerNodeHandler:public NodeHandler{
public:
	virtual void onLoginServerSuccess(const PeerInfo& peerInfo)=0;
	virtual void onDisconnectToServer()=0;
	virtual void onLoginPeerSuccess(const PeerInfo& peerInfo)=0;
	virtual void onLoginPeerFailure(const PeerInfo& peerInfo,int loginMode)=0;
};
/*Node*/
class NodeSession;
class ReleaseSessionWork;
class SenderTimeoutWork;
class NodeHeart;
class TurnHeart;
class Node:public AdapterNode{
public:
	Node();
	virtual ~Node();
	void setHandler(NodeHandler* pHandler);
	NodeHandler* getHandler();
	void addSession(NodeSession* pSession);
	void removeSession(UInt32 wanHost,UInt32 wanPort);
	NodeSession* getSession(UInt32 wanHost,UInt32 wanPort);
	NodeSession* getSessionByLocal(UInt32 localHost,UInt32 localPort);
	void sendPacket(Packet* pPacket);
	virtual void start()=0;
	virtual void shutdown()=0;
	virtual int getLoginMode(const PeerInfo& peerInfo)=0;
	virtual void doReleaseSession()=0;
	virtual void doSenderTimeout()=0;
protected:
	NodeEndPoint* m_pEndPoint;
	NodeHandler* m_pHandler;
	map<string,NodeSession*> m_pSessions;
	bool m_isSendHeart;
	bool m_isReleaseSession;
	SimpleLock m_session_lock;
	ReleaseSessionWork* m_pReleaseSession;
	SenderTimeoutWork* m_pSenderTimeout;
	SimpleLogger m_logger;
};
/*CongesionControl*/
class CongesionControl{
public:
	CongesionControl():m_cwndSize(1),m_rtt(100000),m_rto(1000000),m_ssthresh(40),m_lock(){
	}
	~CongesionControl(){
	}
	inline int getCwndSize(){
		int i;
		m_lock.lock();
		i=m_cwndSize;
		m_lock.unlock();
		return i;
	}
	inline void increaseCwndSize(){
		m_lock.lock();
		if(m_cwndSize<=m_ssthresh&&m_rtt<=3000000){
			m_cwndSize++;
		}
		m_lock.unlock();
	}
	inline void decreaseCwndSize(){
		m_lock.lock();
		m_cwndSize=m_cwndSize/2;
		if(m_cwndSize<1){
			m_cwndSize=1;
		}
		m_lock.unlock();
	}
	inline UInt64 getRtt(){
		UInt64 i;
		m_lock.lock();
		i=m_rtt;
		m_lock.unlock();
		return i;
	}
	inline void setRtt(UInt64 rtt){
		m_lock.lock();
		m_rtt = rtt;
		m_rto=3*m_rtt;
		if(m_rto<500000){//保证rto不小于0.5秒
			m_rto=500000;
		}
		//cout<<"rtt:"<<m_rtt<<",rto:"<<m_rto<<endl;
		m_lock.unlock();
	}
	inline UInt64 getRto(){
		UInt64 i;
		m_lock.lock();
		i=m_rto;
		m_lock.unlock();
		return i;
	}
	inline void setRto(UInt64 rto){
		m_lock.lock();
		m_rto=rto;
		m_lock.unlock();
	}
private:
	int m_cwndSize;
	UInt64 m_rtt;
	UInt64 m_rto;
	int m_ssthresh;
	SimpleLock m_lock;
};
/*GenerateNumber*/
class GenerateNumber{
public:
	GenerateNumber();
	~GenerateNumber();
    UInt32 getPacketSeqNo();
public:
	static const UInt32 uintMaxValue;
private:
	UInt32 m_packetSeqNo;
	SimpleLock m_lock;
};
/*NodeSender*/
class NodeSender:public SimpleObject{
public:
	NodeSender(NodeSession* pSession);
	~NodeSender();
#ifndef WIN32
	void getBeginTime(timeval& beginTime);
#else
	void getBeginTime(LARGE_INTEGER& beginTime);
#endif
	CongesionControl* getControl();
	void destroy();
	void sendData(char* data, int len);
	void continueSend();
	void processAckPacket(AckPacket* pPacket);
private:
	void beginSend();
	void splitData(char* data, int len);
	void getRtPackets();
	bool isEmptyRtPackets();
public:
	static const UInt32 sendBufferSize;
protected:
	CongesionControl m_control;
private:
	list<DataPacket*> m_buffer;
	NodeSession* m_pSession;
	GenerateNumber m_generate;
	list<DataPacket*> m_rtPackets;
#ifndef WIN32
	timeval m_beginTime;
#else
	LARGE_INTEGER m_beginTime;
#endif
	SimpleLock m_lock;
	SimpleLock m_time_lock;
};
/*NodeReceiver*/
class NodeReceiver{
public:
	NodeReceiver(NodeSession* pSession);
	~NodeReceiver();
	void destroy();
	void processDataPacket(DataPacket* pPacket);
private:
	bool isValidDataPacket(DataPacket* pPacket);
	bool bufferContains(UInt32 packetSeqNo);
	void tryJoinDataPacket();
	DataPacket* nextDataPacket();
	UInt32 getNextPacketSeqNo();
private:
	NodeSession* m_pSession;
	list<DataPacket*> m_buffer;
	UInt32 m_lastPacketSeqNo;
	SimpleLock m_lock;
	SimpleLogger m_logger;
};
/*NodeSession*/
class NodeSession{
public:
	NodeSession(Node* pNode,const PeerInfo& peerInfo,int loginMode,bool isSendHeart);
	~NodeSession();
	Node* getNode();
	UInt64 getLastReadTime();
	void setLastReadTime(UInt64 lastReadTime);
	UInt32 getWanHost();
	UInt32 getWanPort();
	UInt32 getLocalHost();
	UInt32 getLocalPort();
	int getLoginMode();
	int getNatType();
	void setNatType(int natType);
	void getPeerInfo(PeerInfo& peerInfo);
	bool isSendHeart();
	NodeSender* getSender();
	void sendData(char* data,int len);
	void processAckPacket(AckPacket* pPacket);
	void processDataPacket(DataPacket* pPacket);
	void destroy();
protected:
	Node* m_pNode;
	NodeHandler* m_pHandler;
	PeerInfo m_peerInfo;
	int m_natType;
	int m_loginMode;
	NodeSender m_sender;
	NodeReceiver m_receiver;
private:
	bool m_isSendHeart;
	UInt64 m_lastReadTime;
	SimpleLock m_lock;
};
/*JudgeNatPoint*/
class ServerNode;
class JudgeNatPoint:public EndPoint{
public:
	JudgeNatPoint(UInt32 host,UInt32 port);
	JudgeNatPoint(UInt32 host,UInt32 port,JudgeNatPoint* pJudgeNatPoint,ServerNode* pServerNode);
	~JudgeNatPoint();
	void sendPacket(Packet* pPacket);
	virtual void receiveData(char* data,int length,UInt32 host,UInt32 port);
private:
	UInt32 m_host;
	UInt32 m_port;
	JudgeNatPoint* m_pJudgeNatPoint;
	ServerNode* m_pServerNode;
	SimpleLogger m_logger;
};
/*ServerNode*/
class ServerNode:public Node{
public:
	ServerNode(UInt32 serverHost,UInt32 serverMainPort,UInt32 serverJudgeNatPort1,UInt32 serverJudgeNatPort2,UInt32 serverTurnPort);
	~ServerNode();
	virtual void start();
	virtual void shutdown();
	virtual void waitForEnd();
	virtual int getLoginMode(const PeerInfo& peerInfo);
	virtual void doReleaseSession();
	virtual void doSenderTimeout();
	int getNatType(const PeerInfo& peerInfo);
private:
	virtual void processLoginServerPacket(LoginServerPacket* pPacket);
	virtual void processHeartPacket(HeartPacket* pPacket);
	virtual void processAckPacket(AckPacket* pPacket);
	virtual void processReportPacket(ReportPacket* pPacket);
	virtual void processWantHolePacket(WantHolePacket* pPacket);
	virtual void processLogoutPacket(LogoutPacket* pPacket);
	virtual void processDataPacket(DataPacket* pPacket);
	int getHoleDirection(int srcNatType,int destNatType);
protected:
	JudgeNatPoint m_judgeNatPoint1;
	JudgeNatPoint m_judgeNatPoint2;
	UInt32 m_serverHost;
	UInt32 m_serverMainPort;
	UInt32 m_serverJudgeNatPort1;
	UInt32 m_serverJudgeNatPort2;
	UInt32 m_serverTurnPort;
	SimpleLogger m_logger;
};
/*PeerNode*/
class PeerNode:public Node{
public:
	PeerNode(bool supportTurn);
	PeerNode(UInt32 localHost,UInt32 localPort,bool supportTurn);
	~PeerNode();
	bool isServerSession(UInt32 host,UInt32 port);
	void getLocalPeerInfo(PeerInfo& peerInfo);
	void getServerPeerInfo(PeerInfo& peerInfo);
	int getLocalNatType();
	NodeSession* getServerSession();
	virtual void start();
	virtual void shutdown();
	virtual void waitForEnd();
	virtual int getLoginMode(const PeerInfo& peerInfo);
	virtual void doReleaseSession();
	virtual void doSenderTimeout();
	void logoutAllAndShutdown();
	void sendTurnControlPacket(Packet* pPacket);
	void sendTurnDataPacket(DataPacket* pPacket);
	void sendTurnHeartPacket();
	void loginServer(UInt32 host,UInt32 port);
	void loginPeer(const PeerInfo& peerInfo,int loginMode);
	void logout(const PeerInfo& peerInfo);
	void doSendHeart();
private:
	virtual void processLoginServerAckPacket(LoginServerAckPacket* pPacket);
	virtual void processLoginPeerPacket(LoginPeerPacket* pPacket);
	virtual void processLoginPeerAckPacket(LoginPeerAckPacket* pPacket);
	virtual void processHeartPacket(HeartPacket* pPacket);
	virtual void processHeartAckPacket(HeartAckPacket* pPacket);
	virtual void processAckPacket(AckPacket* pPacket);
	virtual void processJudgeNatAckPacket(JudgeNatAckPacket* pPacket);
	virtual void processWantHoleAckPacket(WantHoleAckPacket* pPacket);
	virtual void processNotifyHolePacket(NotifyHolePacket* pPacket);
	virtual void processLogoutPacket(LogoutPacket* pPacket);
	virtual void processDataPacket(DataPacket* pPacket);
	void report();
	void trySendHolePacket(UInt32 host,UInt32 port);
	void tryP2PLogin(UInt32 host,UInt32 port);
private:
	bool m_supportTurn;
	UInt32 m_serverHost;
	UInt32 m_serverMainPort;
	UInt32 m_serverJudgeNatPort1;
	UInt32 m_serverJudgeNatPort2;
	UInt32 m_serverTurnPort;
	UInt32 m_wanHost;
	UInt32 m_wanPort;
	UInt32 m_localHost;
	UInt32 m_localPort;
	int m_localNatType;
	NodeSession* m_pServerSession;
	bool m_mainJudgeNatHaveComing;
	SimpleLogger m_logger;
	SimpleLock m_lock;
	TurnHeart* m_pTurnHeart;
	NodeHeart* m_pHeart;
};
/*SendHolePacket*/
class SendHolePacket:public Runnable,SimpleObject{
public:
	SendHolePacket(UInt32 host,UInt32 port,PeerNode* pNode):
		Runnable(),SimpleObject(),m_host(host),m_port(port),m_pNode(pNode){
	}
	~SendHolePacket(){
	}
	virtual void run(){
		int i;
		for(i=0;i<20;i++){
			if(i>0){
				if(m_pNode->getSession(m_host,m_port)!=NULL){
					break;
				}
			}
			HolePacket* pHolePacket=new HolePacket(m_host,m_port);
			m_pNode->sendPacket(pHolePacket);
			SimpleObject::wait(200);
		}
	}
private:
	UInt32 m_host;
	UInt32 m_port;
	PeerNode* m_pNode;
};
/*P2PLogin*/
class P2PLogin:public Runnable,SimpleObject{
public:
	P2PLogin(UInt32 host,UInt32 port,PeerNode* pNode,const PeerInfo& localPeerInfo):
		Runnable(),SimpleObject(),m_host(host),m_port(port),m_pNode(pNode),m_localPeerInfo(localPeerInfo){
	}
	~P2PLogin(){
	}
	virtual void run(){
		int i;
		for(i=0;i<20;i++){
			if(i>0){
				if(m_pNode->getSession(m_host,m_port)!=NULL){
					break;
				}
			}
			LoginPeerPacket* pLoginPeerPacket=new LoginPeerPacket(m_host,m_port);
			pLoginPeerPacket->setWanHost(m_localPeerInfo.m_wanHost);
			pLoginPeerPacket->setWanPort(m_localPeerInfo.m_wanPort);
			pLoginPeerPacket->setLocalHost(m_localPeerInfo.m_localHost);
			pLoginPeerPacket->setLocalPort(m_localPeerInfo.m_localPort);
			pLoginPeerPacket->setLoginMode(P2P);
			m_pNode->sendPacket(pLoginPeerPacket);
			SimpleObject::wait(200);
		}
	}
private:
	UInt32 m_host;
	UInt32 m_port;
	PeerNode* m_pNode;
	PeerInfo m_localPeerInfo;
};
/*TurnHeart*/
class TurnHeart:public DoRepeatWork{
public:
	TurnHeart(PeerNode* pNode);
	~TurnHeart();
	void start();
	void stop();
	void waitForEnd();
	virtual void doRepeatWork();
private:
	PeerNode* m_pNode;
	SimpleLogger m_logger;
	RepeatWork m_repeatWork;
};
/*NodeHeart*/
class NodeHeart:public DoRepeatWork{
public:
	NodeHeart(PeerNode* pNode);
	~NodeHeart();
	void start();
	void stop();
	void waitForEnd();
	virtual void doRepeatWork();
private:
	PeerNode* m_pNode;
	SimpleLogger m_logger;
	RepeatWork m_repeatWork;
};
/*ReleaseSessionWork*/
class ReleaseSessionWork:public DoRepeatWork{
public:
	ReleaseSessionWork(Node* pNode);
	~ReleaseSessionWork();
	void start();
	void stop();
	void waitForEnd();
	virtual void doRepeatWork();
private:
	Node* m_pNode;
	SimpleLogger m_logger;
	RepeatWork m_repeatWork;
};
/*SenderTimeoutWork*/
class SenderTimeoutWork:public DoRepeatWork{
public:
	SenderTimeoutWork(Node* pNode);
	~SenderTimeoutWork();
	void start();
	void stop();
	void waitForEnd();
	virtual void doRepeatWork();
private:
	Node* m_pNode;
	SimpleLogger m_logger;
	RepeatWork m_repeatWork;
};
/*OnDisconnectToServer*/
class OnDisconnectToServer:public Runnable{
public:
	OnDisconnectToServer(PeerNodeHandler* pHandler):Runnable(),m_pHandler(pHandler){
	};
	virtual void run(){
		m_pHandler->onDisconnectToServer();
	}
private:
	PeerNodeHandler* m_pHandler;
};
}
#endif
