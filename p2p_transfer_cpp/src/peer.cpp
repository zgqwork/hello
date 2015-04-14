#include "peer.h"
/*PTPeerHandler*/
PTPeerHandler::PTPeerHandler(){}
PTPeerHandler::~PTPeerHandler(){}
void PTPeerHandler::onLoginServerSuccess(const PeerInfo& peerInfo){}
void PTPeerHandler::onLoginServerFailure(){}
void PTPeerHandler::onServerData(char* data,int len){}
void PTPeerHandler::onDisconnectToServer(){}
void PTPeerHandler::onLoginPeerSuccess(const PeerInfo& peerInfo){}
void PTPeerHandler::onLoginPeerFailure(const PeerInfo& peerInfo,int loginMode){}
void PTPeerHandler::onPeerLogin(const PeerInfo& peerInfo){}
void PTPeerHandler::onPeerLogout(const PeerInfo& peerInfo){}
void PTPeerHandler::onPeerData(char* data,int len,const PeerInfo& peerInfo){}
void PTPeerHandler::onDisconnectToPeer(const PeerInfo& peerInfo){}
/*DefaultPeer*/
DefaultPeer::DefaultPeer(bool supportTurn):PTPeer(),PeerNodeHandler(),m_pHandler(NULL),m_pNode(NULL),m_supportTurn(supportTurn),m_loginTimeouts(),m_login_lock(),m_pLoginWork(){
}
DefaultPeer::~DefaultPeer(){
	if(m_pLoginWork!=NULL){
		delete m_pLoginWork;
		delete m_pNode;
	}
}
void DefaultPeer::setHandler(PTPeerHandler* pHandler){
	m_pHandler=pHandler;
}
void DefaultPeer::start(){
	m_pLoginWork=new LoginTimeoutWork(this);
	m_pLoginWork->start();
	m_pNode=new PeerNode(m_supportTurn);
	m_pNode->setHandler(this);
	m_pNode->start();
}
void DefaultPeer::start(UInt32 localHost,UInt32 localPort){
	m_pLoginWork=new LoginTimeoutWork(this);
	m_pLoginWork->start();
	m_pNode=new PeerNode(localHost,localPort,m_supportTurn);
	m_pNode->setHandler(this);
	m_pNode->start();
}
void DefaultPeer::shutdown(){
	m_pLoginWork->stop();
	m_pNode->shutdown();
}
void DefaultPeer::waitForEnd(){
	m_pLoginWork->waitForEnd();
	m_pNode->waitForEnd();
}
void DefaultPeer::loginServer(UInt32 serverHost,UInt32 serverPort){
	loginServer(serverHost,serverPort,4000);//4�볬ʱ
}
void DefaultPeer::loginServer(UInt32 serverHost,UInt32 serverPort,int timeout){
    m_pNode->loginServer(serverHost,serverPort);
    PeerInfo peerInfo(serverHost,serverPort,serverHost,serverPort);
    LoginTimeoutInfo* pInfo=new LoginTimeoutInfo(TimeUtils::currentTimeMillis()+timeout,SERVER_CLIENT,peerInfo,false);
    m_login_lock.lock();
    m_loginTimeouts.push_back(pInfo);
    m_login_lock.unlock();
}
void DefaultPeer::sendDataToServer(char* data,int len){
	m_pNode->getServerSession()->sendData(data,len);
}
void DefaultPeer::logoutServerAndShutdown(){
	PeerInfo peerInfo;
	m_pNode->getServerPeerInfo(peerInfo);
	m_pNode->logout(peerInfo);
	shutdown();
}
void DefaultPeer::loginPeer(const PeerInfo& peerInfo){
	int loginMode;
	int timeout;
	PeerInfo localInfo;
	m_pNode->getLocalPeerInfo(localInfo);
	if(localInfo.m_wanHost==peerInfo.m_wanHost){
		loginMode=LAN;
		timeout=4000;
	}else{
		loginMode=P2P;
		timeout=6000;
	}
	m_pNode->loginPeer(peerInfo,loginMode);
	LoginTimeoutInfo* pInfo=new LoginTimeoutInfo(TimeUtils::currentTimeMillis()+timeout,loginMode,peerInfo,true);
	m_login_lock.lock();
	m_loginTimeouts.push_back(pInfo);
	m_login_lock.unlock();
}
void DefaultPeer::loginPeer(const PeerInfo& peerInfo,int loginMode){
	int timeout;
	if(loginMode==P2P){
		timeout=4000;
	}else{
		timeout=6000;
	}
    loginPeer(peerInfo,loginMode,timeout);
}
void DefaultPeer::loginPeer(const PeerInfo& peerInfo,int loginMode,int timeout){
	m_pNode->loginPeer(peerInfo,loginMode);
	LoginTimeoutInfo* pInfo=new LoginTimeoutInfo(TimeUtils::currentTimeMillis()+timeout,loginMode,peerInfo,false);
	m_login_lock.lock();
	m_loginTimeouts.push_back(pInfo);
	m_login_lock.unlock();
}
void DefaultPeer::sendDataToPeer(char* data,int len,const PeerInfo& peerInfo){
	NodeSession* pSession=m_pNode->getSession(peerInfo.m_wanHost,peerInfo.m_wanPort);
	if(pSession!=NULL){
		pSession->sendData(data,len);
	}
}
void DefaultPeer::logoutPeer(const PeerInfo& peerInfo){
	m_pNode->logout(peerInfo);
}
void DefaultPeer::logoutAllAndShutdown(){
	m_pNode->logoutAllAndShutdown();
	shutdown();
}
int DefaultPeer::getLocalNatType(){
	return m_pNode->getLocalNatType();
}
void DefaultPeer::getLocalPeerInfo(PeerInfo& peerInfo){
	m_pNode->getLocalPeerInfo(peerInfo);
}
int DefaultPeer::getLoginMode(const PeerInfo& peerInfo){
	return m_pNode->getLoginMode(peerInfo);
}
void DefaultPeer::receiveData(char* data,int len,const PeerInfo& peerInfo){
	if(m_pNode->isServerSession(peerInfo.m_wanHost,peerInfo.m_wanPort)){
		m_pHandler->onServerData(data,len);
	}else{
		m_pHandler->onPeerData(data,len,peerInfo);
	}
}
void DefaultPeer::onPeerLogin(const PeerInfo& peerInfo){
	CoreExecutorService::getInstance()->execute(new OnPeerLogin(m_pHandler,peerInfo));
}
void DefaultPeer::onPeerLogout(const PeerInfo& peerInfo){
    CoreExecutorService::getInstance()->execute(new OnPeerLogout(m_pHandler,peerInfo));
}
void DefaultPeer::onDisconnectToPeer(const PeerInfo& peerInfo){
    CoreExecutorService::getInstance()->execute(new OnDisconnectToPeer(m_pHandler,peerInfo));
}
void DefaultPeer::onLoginServerSuccess(const PeerInfo& peerInfo){
    CoreExecutorService::getInstance()->execute(new OnLoginServerSuccess(m_pHandler,peerInfo));
    m_login_lock.lock();
    list<LoginTimeoutInfo*>::iterator it;
    for(it=m_loginTimeouts.begin();it!=m_loginTimeouts.end();it++){
    	if((*it)->m_peerInfo==peerInfo){
    		delete *it;
    		it=m_loginTimeouts.erase(it);
    		break;
    	}
    }
    m_login_lock.unlock();
}
void DefaultPeer::onDisconnectToServer(){
	shutdown();
	m_pHandler->onDisconnectToServer();
}
void DefaultPeer::onLoginPeerSuccess(const PeerInfo& peerInfo){
    CoreExecutorService::getInstance()->execute(new OnLoginPeerSuccess(m_pHandler,peerInfo));
    m_login_lock.lock();
    list<LoginTimeoutInfo*>::iterator it;
    for(it=m_loginTimeouts.begin();it!=m_loginTimeouts.end();it++){
    	if((*it)->m_peerInfo==peerInfo){
    		delete *it;
			it=m_loginTimeouts.erase(it);
			break;
		}
	}
	m_login_lock.unlock();
}
void DefaultPeer::onLoginPeerFailure(const PeerInfo& peerInfo,int loginMode){
	m_login_lock.lock();
	list<LoginTimeoutInfo*>::iterator it;
	for(it=m_loginTimeouts.begin();it!=m_loginTimeouts.end();){
		if((*it)->m_peerInfo==peerInfo){
			if((*it)->m_failureContinue){
				if((*it)->m_loginMode==P2P&&m_supportTurn){
					(*it)->m_loginMode=TURN;
					(*it)->m_timeStamp=TimeUtils::currentTimeMillis()+4000;
					m_pNode->loginPeer(peerInfo,TURN);
					it++;
				}else{
					CoreExecutorService::getInstance()->execute(new OnLoginPeerFailure(m_pHandler,peerInfo,(*it)->m_loginMode));
					delete *it;
					m_loginTimeouts.erase(it++);
				}
			}else{
				CoreExecutorService::getInstance()->execute(new OnLoginPeerFailure(m_pHandler,peerInfo,(*it)->m_loginMode));
				delete *it;
				m_loginTimeouts.erase(it++);
			}
			break;
		}else{
			it++;
		}
	}
	m_login_lock.unlock();
}
void DefaultPeer::doLoginTimeoutWork(){
	m_login_lock.lock();
	list<LoginTimeoutInfo*>::iterator it;
	for(it=m_loginTimeouts.begin();it!=m_loginTimeouts.end();){
		if((*it)->m_timeStamp<TimeUtils::currentTimeMillis()){
			if(m_pNode->isServerSession((*it)->m_peerInfo.m_wanHost,(*it)->m_peerInfo.m_wanPort)){
				CoreExecutorService::getInstance()->execute(new OnLoginServerFailure(m_pHandler));
				delete *it;
				m_loginTimeouts.erase(it++);
			}else{
				if((*it)->m_failureContinue){
					if((*it)->m_loginMode==LAN){
						(*it)->m_loginMode=P2P;
						(*it)->m_timeStamp=TimeUtils::currentTimeMillis()+6000;
						m_pNode->loginPeer((*it)->m_peerInfo,P2P);
						it++;
					}else if((*it)->m_loginMode==P2P&&m_supportTurn){
						(*it)->m_loginMode=TURN;
						(*it)->m_timeStamp=TimeUtils::currentTimeMillis()+4000;
						m_pNode->loginPeer((*it)->m_peerInfo,TURN);
						it++;
					}else{
						CoreExecutorService::getInstance()->execute(new OnLoginPeerFailure(m_pHandler,(*it)->m_peerInfo,(*it)->m_loginMode));
						delete *it;
						m_loginTimeouts.erase(it++);
					}
				}else{
					CoreExecutorService::getInstance()->execute(new OnLoginPeerFailure(m_pHandler,(*it)->m_peerInfo,(*it)->m_loginMode));
				    delete *it;
					m_loginTimeouts.erase(it++);
				}
			}
		}else{
			it++;
		}
	}
	m_login_lock.unlock();
}
/*PT*/
void PT::toIpStr(UInt32 ip,string& ipStr){
	CommonUtils::toIpStr(ip,ipStr);
}
UInt32 PT::toIpInt(const string& ip){
	return CommonUtils::toIpInt(ip);
}
PTPeer* PT::createPeer(bool supportTurn){
	return new DefaultPeer(supportTurn);
}
void PT::destroyPeer(PTPeer* pPeer){
	delete pPeer;
}
