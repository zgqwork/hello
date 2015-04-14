#ifndef __SERVER_H__
#define __SERVER_H__
#include "node.h"
namespace PT{
/*DefaultTurnServer*/
class TurnEndPoint;
class DefaultTurnServer{
public:
	DefaultTurnServer();
	~DefaultTurnServer();
	void start(UInt32 host,UInt32 turnPort);
	void shutdown();
	void waitForEnd();
	void doTurnControlPacket(TurnControlPacket* pPacket);
	void doTurnDataPacket(TurnDataPacket* pPacket);
private:
	TurnEndPoint* m_pTep;
	SimpleLogger m_logger;
};
/*TurnEndPoint*/
class TurnEndPoint:public EndPoint{
public:
	TurnEndPoint(UInt32 host,UInt32 port,DefaultTurnServer* pTurnServer);
	~TurnEndPoint();
	virtual void receiveData(char* data,int length,UInt32 host,UInt32 port);
private:
	DefaultTurnServer* m_pTurnServer;
	SimpleLogger m_logger;
};
/*DefaultServer*/
class DefaultServer:public PTServer,NodeHandler{
public:
	DefaultServer(bool supportTurn);
	virtual ~DefaultServer();
	virtual void setHandler(PTServerHandler* pHandler);
	virtual void start(UInt32 host,UInt32 mainPort,UInt32 judgeNatPort1,UInt32 judgeNatPort2,UInt32 turnPort);
	virtual void shutdown();
	virtual void waitForEnd();
	virtual void sendDataToPeer(char* data,int len,const PeerInfo& peerInfo);
	virtual int getNatType(const PeerInfo& peerInfo);

	virtual void receiveData(char* data,int len,const PeerInfo& peerInfo);
	virtual void onPeerLogin(const PeerInfo& peerInfo);
	virtual void onPeerLogout(const PeerInfo& peerInfo);
	virtual void onDisconnectToPeer(const PeerInfo& peerInfo);
private:
	ServerNode* m_pNode;
	PTServerHandler* m_pHandler;
	DefaultTurnServer* m_pTurnServer;
	bool m_supportTurn;
};
/*OnPeerLoginForServer*/
class OnPeerLoginForServer:public Runnable{
public:
	OnPeerLoginForServer(PTServerHandler* pHandler,const PeerInfo& peerInfo):Runnable(),m_pHandler(pHandler),m_peerInfo(peerInfo){
	};
	virtual void run(){
		m_pHandler->onPeerLogin(m_peerInfo);
	}
private:
	PTServerHandler* m_pHandler;
	PeerInfo m_peerInfo;
};
/*OnPeerLogoutForServer*/
class OnPeerLogoutForServer:public Runnable{
public:
	OnPeerLogoutForServer(PTServerHandler* pHandler,const PeerInfo& peerInfo):Runnable(),m_pHandler(pHandler),m_peerInfo(peerInfo){
	};
	virtual void run(){
		m_pHandler->onPeerLogout(m_peerInfo);
	}
private:
	PTServerHandler* m_pHandler;
	PeerInfo m_peerInfo;
};
/*OnDisconnectToPeerForServer*/
class OnDisconnectToPeerForServer:public Runnable{
public:
	OnDisconnectToPeerForServer(PTServerHandler* pHandler,const PeerInfo& peerInfo):Runnable(),m_pHandler(pHandler),m_peerInfo(peerInfo){
	};
	virtual void run(){
		m_pHandler->onDisconnectToPeer(m_peerInfo);
	}
private:
	PTServerHandler* m_pHandler;
	PeerInfo m_peerInfo;
};
}
#endif
