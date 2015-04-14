#ifndef __PEER_H__
#define __PEER_H__
#include "node.h"
namespace PT{
/*LoginTimeoutInfo*/
class LoginTimeoutInfo{
public:
	LoginTimeoutInfo(UInt64 timeStamp, int loginMode,const PeerInfo& peerInfo,bool failureContinue)
        :m_timeStamp(timeStamp),m_loginMode(loginMode),m_peerInfo(peerInfo),m_failureContinue(failureContinue){
	}
public:
	UInt64 m_timeStamp;
	int m_loginMode;
	PeerInfo m_peerInfo;
	bool m_failureContinue;
};
/*DefaultPeer*/
class LoginTimeoutWork;
class DefaultPeer:public PTPeer,PeerNodeHandler{
public:
	DefaultPeer(bool supportTurn);
	~DefaultPeer();
	virtual void setHandler(PTPeerHandler* pHandler);
	virtual void start();
	virtual void start(UInt32 localHost,UInt32 localPort);
	virtual void shutdown();
	virtual void waitForEnd();
	virtual void loginServer(UInt32 serverHost,UInt32 serverPort);
	virtual void loginServer(UInt32 serverHost,UInt32 serverPort,int timeout);
	virtual void sendDataToServer(char* data,int len);
	virtual void logoutServerAndShutdown();
	virtual void loginPeer(const PeerInfo& peerInfo);
	virtual void loginPeer(const PeerInfo& peerInfo,int loginMode);
	virtual void loginPeer(const PeerInfo& peerInfo,int loginMode,int timeout);
	virtual void sendDataToPeer(char* data,int len,const PeerInfo& peerInfo);
	virtual void logoutPeer(const PeerInfo& peerInfo);
	virtual void logoutAllAndShutdown();
	virtual int getLocalNatType();
	virtual void getLocalPeerInfo(PeerInfo& peerInfo);
	virtual int getLoginMode(const PeerInfo& peerInfo);

	virtual void receiveData(char* data,int len,const PeerInfo& peerInfo);
	virtual void onPeerLogin(const PeerInfo& peerInfo);
	virtual void onPeerLogout(const PeerInfo& peerInfo);
	virtual void onDisconnectToPeer(const PeerInfo& peerInfo);
	virtual void onLoginServerSuccess(const PeerInfo& peerInfo);
	virtual void onDisconnectToServer();
	virtual void onLoginPeerSuccess(const PeerInfo& peerInfo);
	virtual void onLoginPeerFailure(const PeerInfo& peerInfo,int loginMode);

	void doLoginTimeoutWork();
private:
	PTPeerHandler* m_pHandler;
	PeerNode* m_pNode;
	bool m_supportTurn;
	list<LoginTimeoutInfo*> m_loginTimeouts;
	SimpleLock m_login_lock;
	LoginTimeoutWork* m_pLoginWork;
};
/*LoginTimeoutWork*/
class LoginTimeoutWork:public DoRepeatWork{
public:
	LoginTimeoutWork(DefaultPeer* pPeer):DoRepeatWork(),m_pPeer(pPeer),m_logger("peer.LoginTimeoutWork"),m_repeatWork(this,&m_logger){
	}
	~LoginTimeoutWork(){
	}
	void start(){
		m_repeatWork.start();
	}
	void stop(){
		m_repeatWork.stop();
	}
	void waitForEnd(){
		m_repeatWork.waitForEnd();
	}
	virtual void doRepeatWork(){
		m_pPeer->doLoginTimeoutWork();
		m_repeatWork.wait(100);
	}
private:
	DefaultPeer* m_pPeer;
	SimpleLogger m_logger;
	RepeatWork m_repeatWork;
};
/*OnPeerLogin*/
class OnPeerLogin:public Runnable{
public:
	OnPeerLogin(PTPeerHandler* pHandler,const PeerInfo& peerInfo):Runnable(),m_pHandler(pHandler),m_peerInfo(peerInfo){
	};
	virtual void run(){
		m_pHandler->onPeerLogin(m_peerInfo);
	}
private:
	PTPeerHandler* m_pHandler;
	PeerInfo m_peerInfo;
};
/*OnPeerLogout*/
class OnPeerLogout:public Runnable{
public:
	OnPeerLogout(PTPeerHandler* pHandler,const PeerInfo& peerInfo):Runnable(),m_pHandler(pHandler),m_peerInfo(peerInfo){
	};
	virtual void run(){
		m_pHandler->onPeerLogout(m_peerInfo);
	}
private:
	PTPeerHandler* m_pHandler;
	PeerInfo m_peerInfo;
};
/*OnDisconnectToPeer*/
class OnDisconnectToPeer:public Runnable{
public:
	OnDisconnectToPeer(PTPeerHandler* pHandler,const PeerInfo& peerInfo):Runnable(),m_pHandler(pHandler),m_peerInfo(peerInfo){
	};
	virtual void run(){
		m_pHandler->onDisconnectToPeer(m_peerInfo);
	}
private:
	PTPeerHandler* m_pHandler;
	PeerInfo m_peerInfo;
};
/*OnLoginServerSuccess*/
class OnLoginServerSuccess:public Runnable{
public:
	OnLoginServerSuccess(PTPeerHandler* pHandler,const PeerInfo& peerInfo):Runnable(),m_pHandler(pHandler),m_peerInfo(peerInfo){
	};
	virtual void run(){
		m_pHandler->onLoginServerSuccess(m_peerInfo);
	}
private:
	PTPeerHandler* m_pHandler;
	PeerInfo m_peerInfo;
};
/*OnLoginServerFailure*/
class OnLoginServerFailure:public Runnable{
public:
	OnLoginServerFailure(PTPeerHandler* pHandler):Runnable(),m_pHandler(pHandler){
	};
	virtual void run(){
		m_pHandler->onLoginServerFailure();
	}
private:
	PTPeerHandler* m_pHandler;
};
/*OnLoginPeerSuccess*/
class OnLoginPeerSuccess:public Runnable{
public:
	OnLoginPeerSuccess(PTPeerHandler* pHandler,const PeerInfo& peerInfo):Runnable(),m_pHandler(pHandler),m_peerInfo(peerInfo){
	};
	virtual void run(){
		m_pHandler->onLoginPeerSuccess(m_peerInfo);
	}
private:
	PTPeerHandler* m_pHandler;
	PeerInfo m_peerInfo;
};
/*OnLoginPeerFailure*/
class OnLoginPeerFailure:public Runnable{
public:
	OnLoginPeerFailure(PTPeerHandler* pHandler,const PeerInfo& peerInfo,int loginMode):Runnable(),m_pHandler(pHandler),m_peerInfo(peerInfo),m_loginMode(loginMode){
	};
	virtual void run(){
		m_pHandler->onLoginPeerFailure(m_peerInfo,m_loginMode);
	}
private:
	PTPeerHandler* m_pHandler;
	PeerInfo m_peerInfo;
	int m_loginMode;
};
}
#endif
