#include "server.h"
/*DefaultTurnServer*/
DefaultTurnServer::DefaultTurnServer():m_pTep(NULL),m_logger("server.DefaultTurnServer"){
}
DefaultTurnServer::~DefaultTurnServer(){
	if(m_pTep!=NULL){
		delete m_pTep;
	}
}
void DefaultTurnServer::start(UInt32 host,UInt32 turnPort){
	m_pTep=new TurnEndPoint(host,turnPort,this);
	m_pTep->start();
}
void DefaultTurnServer::shutdown(){
	m_pTep->shutdown();
}
void DefaultTurnServer::waitForEnd(){
	m_pTep->waitForEnd();
}
void DefaultTurnServer::doTurnControlPacket(TurnControlPacket* pPacket){
	Packet* pP=pPacket->getPacket();
	UInt32 peerHost=pP->getHost();//目标地址
	UInt32 peerPort=pP->getPort();//目标端口
	pP->setHost(pPacket->getHost());
	pP->setPort(pPacket->getPort());
	pPacket->setHost(peerHost);
	pPacket->setPort(peerPort);
	char* temp=new char[pPacket->getBytesLen()];
	pPacket->readBytes(temp);
	m_pTep->sendData(temp,pPacket->getBytesLen(),pPacket->getHost(),pPacket->getPort());
	m_logger.info("send:");pPacket->printInfo();
	delete pP;
	delete pPacket;
	delete temp;
}
void DefaultTurnServer::doTurnDataPacket(TurnDataPacket* pPacket){
	Packet* pP=pPacket->getDataPacket();
	UInt32 peerHost=pP->getHost();//目标地址
	UInt32 peerPort=pP->getPort();//目标端口
	pP->setHost(pPacket->getHost());
	pP->setPort(pPacket->getPort());
	pPacket->setHost(peerHost);
	pPacket->setPort(peerPort);
	char* temp=new char[pPacket->getBytesLen()];
	pPacket->readBytes(temp);
	m_pTep->sendData(temp,pPacket->getBytesLen(),pPacket->getHost(),pPacket->getPort());
	m_logger.info("send:");pPacket->printInfo();
	delete pP;
	delete pPacket;
	delete temp;
}
/*TurnEndPoint*/
TurnEndPoint::TurnEndPoint(UInt32 host,UInt32 port,DefaultTurnServer* pTurnServer):EndPoint(host,port),m_pTurnServer(pTurnServer),m_logger("server.TurnEndPoint"){
}
TurnEndPoint::~TurnEndPoint(){
}
void TurnEndPoint::receiveData(char* data,int length,UInt32 host,UInt32 port){
	bool isTurn=(data[0]&128)!=0;
	bool isControl=(data[0]&64)!=0;
	bool isTurnHeart=(data[0]&32)!=0;
	Packet* pPacket;
	if(isTurn){
		if(!isTurnHeart){
			if(isControl){
				pPacket=new TurnControlPacket(host,port);
				pPacket->writeBytes(data,length);
				m_logger.info("receive:");pPacket->printInfo();
				m_pTurnServer->doTurnControlPacket((TurnControlPacket*)pPacket);
			}else{
				pPacket=new TurnDataPacket(host,port);
				pPacket->writeBytes(data,length);
				m_logger.info("receive:");pPacket->printInfo();
				m_pTurnServer->doTurnDataPacket((TurnDataPacket*)pPacket);
			}
		}else{
			pPacket=new TurnHeartPacket(host,port);
			pPacket->writeBytes(data,length);
			m_logger.info("receive:");pPacket->printInfo();
			delete pPacket;
		}
	}
}
/*PTServerHandler*/
PTServerHandler::PTServerHandler(){}
PTServerHandler::~PTServerHandler(){}
void PTServerHandler::onPeerLogin(const PeerInfo& peerInfo){}
void PTServerHandler::onPeerLogout(const PeerInfo& peerInfo){}
void PTServerHandler::onPeerData(char* data,int len,const PeerInfo& peerInfo){}
void PTServerHandler::onDisconnectToPeer(const PeerInfo& peerInfo){}
/*DefaultServer*/
DefaultServer::DefaultServer(bool supportTurn):PTServer(),NodeHandler(),m_pTurnServer(NULL),m_supportTurn(supportTurn){
}
DefaultServer::~DefaultServer(){
	if(m_pNode!=NULL){
		delete m_pNode;
	}
	if(m_pTurnServer!=NULL){
		delete m_pTurnServer;
	}
}
void DefaultServer::setHandler(PTServerHandler* pHandler){
	m_pHandler=pHandler;
}
void DefaultServer::start(UInt32 host,UInt32 mainPort,UInt32 judgeNatPort1,UInt32 judgeNatPort2,UInt32 turnPort){
	m_pNode=new ServerNode(host,mainPort,judgeNatPort1,judgeNatPort2,turnPort);
	m_pNode->setHandler(this);
	m_pNode->start();
	if(m_supportTurn){
		m_pTurnServer=new DefaultTurnServer();
		m_pTurnServer->start(host,turnPort);
	}
}
void DefaultServer::shutdown(){
	m_pNode->shutdown();
	if(m_pTurnServer!=NULL){
		m_pTurnServer->shutdown();
	}
}
void DefaultServer::waitForEnd(){
	m_pNode->waitForEnd();
	if(m_pTurnServer!=NULL){
		m_pTurnServer->waitForEnd();
	}
}
void DefaultServer::sendDataToPeer(char* data,int len,const PeerInfo& peerInfo){
	UInt32 wanHost=peerInfo.m_wanHost;
	UInt32 wanPort=peerInfo.m_wanPort;
	NodeSession* pSession=m_pNode->getSession(wanHost,wanPort);
	if(pSession!=NULL){
		pSession->sendData(data,len);
	}
}
int DefaultServer::getNatType(const PeerInfo& peerInfo){
	return m_pNode->getNatType(peerInfo);
}
void DefaultServer::receiveData(char* data,int len,const PeerInfo& peerInfo){
	if(m_pHandler!=NULL){
		m_pHandler->onPeerData(data,len,peerInfo);
	}
}
void DefaultServer::onPeerLogin(const PeerInfo& peerInfo){
	if(m_pHandler!=NULL){
        CoreExecutorService::getInstance()->execute(new OnPeerLoginForServer(m_pHandler,peerInfo));
	}
}
void DefaultServer::onPeerLogout(const PeerInfo& peerInfo){
	if(m_pHandler!=NULL){
		CoreExecutorService::getInstance()->execute(new OnPeerLogoutForServer(m_pHandler,peerInfo));
	}
}
void DefaultServer::onDisconnectToPeer(const PeerInfo& peerInfo){
	if(m_pHandler!=NULL){
		CoreExecutorService::getInstance()->execute(new OnDisconnectToPeerForServer(m_pHandler,peerInfo));
	}
}
/*PT*/
PTServer* PT::createServer(bool supportTurn){
	return new DefaultServer(supportTurn);
}
void PT::destroyServer(PTServer* pServer){
	delete pServer;
}
