#include "node.h"
/*AdapterNode*/
AdapterNode::AdapterNode(){
}
AdapterNode::~AdapterNode(){
}
void AdapterNode::processLoginServerPacket(LoginServerPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processLoginServerAckPacket(LoginServerAckPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processLoginPeerPacket(LoginPeerPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processLoginPeerAckPacket(LoginPeerAckPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processHeartPacket(HeartPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processHeartAckPacket(HeartAckPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processAckPacket(AckPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processJudgeNatPacket(JudgeNatPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processJudgeNatAckPacket(JudgeNatAckPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processReportPacket(ReportPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processReportAckPacket(ReportAckPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processWantHolePacket(WantHolePacket* pPacket){
	delete pPacket;
}
void AdapterNode::processWantHoleAckPacket(WantHoleAckPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processNotifyHolePacket(NotifyHolePacket* pPacket){
	delete pPacket;
}
void AdapterNode::processHolePacket(HolePacket* pPacket){
	delete pPacket;
}
void AdapterNode::processLogoutPacket(LogoutPacket* pPacket){
	delete pPacket;
}
void AdapterNode::processDataPacket(DataPacket* pPacket){
	delete pPacket;
}
/*NodeEndPoint*/
NodeEndPoint::NodeEndPoint(AdapterNode* pNode):EndPoint(),m_pNode(pNode),m_logger("node.NodeEndPoint"){
}
NodeEndPoint::NodeEndPoint(UInt32 host,UInt32 port,AdapterNode* pNode):EndPoint(host,port),m_pNode(pNode),m_logger("node.NodeEndPoint"){
}
NodeEndPoint::~NodeEndPoint(){
}
void NodeEndPoint::receiveData(char* data,int length,UInt32 host,UInt32 port){
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
    			processControlPacket(((TurnControlPacket*)pPacket)->getPacket());
    		}else{
    			pPacket=new TurnDataPacket(host,port);
    			pPacket->writeBytes(data,length);
    			m_logger.info("receive:");pPacket->printInfo();
    			m_pNode->processDataPacket(((TurnDataPacket*)pPacket)->getDataPacket());
    		}
    		delete pPacket;
    	}
    }else{
    	if(isControl){
    		pPacket=newControlPacket(data,length,host,port);
    		m_logger.info("receive:");pPacket->printInfo();
    		processControlPacket(pPacket);
    	}else{
    		pPacket=new DataPacket(host,port);
    		pPacket->writeBytes(data,length);
    		m_logger.info("receive:");pPacket->printInfo();
    		m_pNode->processDataPacket((DataPacket*)pPacket);
    	}
    }
}
Packet* NodeEndPoint::newControlPacket(char* data,int length,UInt32 host,UInt32 port){
	Packet* pPacket=NULL;
	int controlPacketType=data[1];
	if(controlPacketType==LOGIN_SERVER){
		pPacket=new LoginServerPacket(host,port);
	}else if(controlPacketType==LOGIN_SERVER_ACK){
		pPacket=new LoginServerAckPacket(host,port);
	}else if(controlPacketType==LOGIN_PEER){
		pPacket=new LoginPeerPacket(host,port);
	}else if(controlPacketType==LOGIN_PEER_ACK){
		pPacket=new LoginPeerAckPacket(host,port);
	}else if(controlPacketType==HEART){
		pPacket=new HeartPacket(host,port);
	}else if(controlPacketType==HEART_ACK){
		pPacket=new HeartAckPacket(host,port);
	}else if(controlPacketType==ACK){
		pPacket=new AckPacket(host,port);
	}else if(controlPacketType==JUDGE_NAT){
		pPacket=new JudgeNatPacket(host,port);
	}else if(controlPacketType==JUDGE_NAT_ACK){
		pPacket=new JudgeNatAckPacket(host,port);
	}else if(controlPacketType==REPORT){
		pPacket=new ReportPacket(host,port);
	}else if(controlPacketType==REPORT_ACK){
		pPacket=new ReportAckPacket(host,port);
	}else if(controlPacketType==WANT_HOLE){
		pPacket=new WantHolePacket(host,port);
	}else if(controlPacketType==WANT_HOLE_ACK){
		pPacket=new WantHoleAckPacket(host,port);
	}else if(controlPacketType==NOTIFY_HOLE){
		pPacket=new NotifyHolePacket(host,port);
	}else if(controlPacketType==HOLE){
		pPacket=new HolePacket(host,port);
	}else if(controlPacketType==LOGOUT){
		pPacket=new LogoutPacket(host,port);
	}
	if(pPacket!=NULL){
		pPacket->writeBytes(data,length);
	}
	return pPacket;
}
void NodeEndPoint::processControlPacket(Packet* pPacket){
	int controlPacketType=pPacket->getCotrolPacketType();
	if(controlPacketType==LOGIN_SERVER){
		m_pNode->processLoginServerPacket((LoginServerPacket*)pPacket);
	}else if(controlPacketType==LOGIN_SERVER_ACK){
		m_pNode->processLoginServerAckPacket((LoginServerAckPacket*)pPacket);
	}else if(controlPacketType==LOGIN_PEER){
		m_pNode->processLoginPeerPacket((LoginPeerPacket*)pPacket);
	}else if(controlPacketType==LOGIN_PEER_ACK){
		m_pNode->processLoginPeerAckPacket((LoginPeerAckPacket*)pPacket);
	}else if(controlPacketType==HEART){
		m_pNode->processHeartPacket((HeartPacket*)pPacket);
	}else if(controlPacketType==HEART_ACK){
		m_pNode->processHeartAckPacket((HeartAckPacket*)pPacket);
	}else if(controlPacketType==ACK){
		m_pNode->processAckPacket((AckPacket*)pPacket);
	}else if(controlPacketType==JUDGE_NAT){
		m_pNode->processJudgeNatPacket((JudgeNatPacket*)pPacket);
	}else if(controlPacketType==JUDGE_NAT_ACK){
		m_pNode->processJudgeNatAckPacket((JudgeNatAckPacket*)pPacket);
	}else if(controlPacketType==REPORT){
		m_pNode->processReportPacket((ReportPacket*)pPacket);
	}else if(controlPacketType==REPORT_ACK){
		m_pNode->processReportAckPacket((ReportAckPacket*)pPacket);
	}else if(controlPacketType==WANT_HOLE){
		m_pNode->processWantHolePacket((WantHolePacket*)pPacket);
	}else if(controlPacketType==WANT_HOLE_ACK){
		m_pNode->processWantHoleAckPacket((WantHoleAckPacket*)pPacket);
	}else if(controlPacketType==NOTIFY_HOLE){
		m_pNode->processNotifyHolePacket((NotifyHolePacket*)pPacket);
	}else if(controlPacketType==HOLE){
		m_pNode->processHolePacket((HolePacket*)pPacket);
	}else if(controlPacketType==LOGOUT){
		m_pNode->processLogoutPacket((LogoutPacket*)pPacket);
	}else{
		delete pPacket;
	}
}
/*Node*/
Node::Node():AdapterNode(),m_pEndPoint(NULL),m_pHandler(NULL),m_pSessions(),m_isSendHeart(__isSendHeart),m_isReleaseSession(__isReleaseSession),
	m_session_lock(),m_pReleaseSession(NULL),m_pSenderTimeout(NULL),m_logger("node.Node"){
}
Node::~Node(){
}
void Node::setHandler(NodeHandler* pHandler){
	m_pHandler=pHandler;
}
NodeHandler* Node::getHandler(){
	return m_pHandler;
}
void Node::addSession(NodeSession* pSession){
	string key;
	CommonUtils::toIpPortKey(pSession->getWanHost(),pSession->getWanPort(),key);
	m_session_lock.lock();
	map<string,NodeSession*>::iterator it;
	it=m_pSessions.find(key);
	if(it!=m_pSessions.end()){
		it->second->destroy();
		delete it->second;
		m_pSessions.erase(it);
	}
	m_pSessions.insert(pair<string,NodeSession*>(key,pSession));
	m_session_lock.unlock();
}
void Node::removeSession(UInt32 wanHost,UInt32 wanPort){
	string key;
	CommonUtils::toIpPortKey(wanHost,wanPort,key);
	m_session_lock.lock();
	map<string,NodeSession*>::iterator it;
	it=m_pSessions.find(key);
	if(it!=m_pSessions.end()){
		it->second->destroy();
		delete it->second;
		m_pSessions.erase(it);
	}
	m_session_lock.unlock();
}
NodeSession* Node::getSession(UInt32 wanHost,UInt32 wanPort){
	NodeSession* pSession=NULL;
	string key;
	CommonUtils::toIpPortKey(wanHost,wanPort,key);
	m_session_lock.lock();
	map<string,NodeSession*>::iterator it=m_pSessions.find(key);
	if(it!=m_pSessions.end()){
		pSession=it->second;
	}
	m_session_lock.unlock();
	return pSession;
}
NodeSession* Node::getSessionByLocal(UInt32 localHost,UInt32 localPort){
	NodeSession* pSession=NULL;
	NodeSession* pS=NULL;
	m_session_lock.lock();
	map<string,NodeSession*>::iterator it;
	for(it=m_pSessions.begin();it!=m_pSessions.end();it++){
		pS=it->second;
		if(pS->getLocalHost()==localHost&&(pS->getLocalPort())==localPort){
			pSession=pS;
			break;
		}
	}
	m_session_lock.unlock();
	return pSession;
}
void Node::sendPacket(Packet* pPacket){
	if(!pPacket->isTurn()){
		m_logger.info("send:");pPacket->printInfo();
		char* data=new char[pPacket->getBytesLen()];
		pPacket->readBytes(data);
		m_pEndPoint->sendData(data,pPacket->getBytesLen(),pPacket->getHost(),pPacket->getPort());
		if(pPacket->isControl()){
			delete pPacket;
		}
		delete data;
	}
}
/*GenerateNumber*/
const UInt32 GenerateNumber::uintMaxValue=__uintMaxValue;
GenerateNumber::GenerateNumber():m_packetSeqNo(0),m_lock(){
}
GenerateNumber::~GenerateNumber(){}
UInt32 GenerateNumber::getPacketSeqNo(){
	m_lock.lock();
	m_packetSeqNo++;
	if(m_packetSeqNo>uintMaxValue){
		m_packetSeqNo=1;
	}
	m_lock.unlock();
	return m_packetSeqNo;
}
/*NodeSender*/
const UInt32 NodeSender::sendBufferSize=200;
NodeSender::NodeSender(NodeSession* pSession):SimpleObject(),m_control(),m_buffer(),m_pSession(pSession),m_generate(),m_rtPackets(),m_lock(),m_time_lock(){
#ifndef WIN32
	gettimeofday(&m_beginTime,0);
#else
	QueryPerformanceCounter(&m_beginTime);
#endif
}
NodeSender::~NodeSender(){
}
#ifndef WIN32
	void NodeSender::getBeginTime(timeval& beginTime){
#else
	void NodeSender::getBeginTime(LARGE_INTEGER& beginTime){
#endif
    m_time_lock.lock();
    memcpy(&beginTime,&m_beginTime,sizeof(m_beginTime));
    m_time_lock.unlock();
}
CongesionControl* NodeSender::getControl(){
	return &m_control;
}
void NodeSender::destroy(){
	list<DataPacket*>::iterator it;
	m_lock.lock();
	for(it=m_rtPackets.begin();it!=m_rtPackets.end();it++){
		delete *it;
	}
	m_rtPackets.clear();
	for(it=m_buffer.begin();it!=m_buffer.end();it++){
		delete *it;
	}
	m_buffer.clear();
	m_lock.unlock();
}
void NodeSender::sendData(char* data, int len){
	splitData(data,len);
	m_lock.lock();
	if(isEmptyRtPackets()) {
		getRtPackets();
		beginSend();
	}
	m_lock.unlock();
}
void NodeSender::continueSend(){
	m_lock.lock();
	if(!isEmptyRtPackets()){
		m_control.decreaseCwndSize();//减小拥塞窗口大小
		getRtPackets();
		beginSend();
	}
	m_lock.unlock();
}
void NodeSender::processAckPacket(AckPacket* pPacket){
	list<DataPacket*>::iterator it;
	m_lock.lock();
	for(it=m_rtPackets.begin();it!=m_rtPackets.end();it++){
		if((*it)->getPacketSeqNo()==pPacket->getPacketSeqNo()){
			delete *it;
			m_rtPackets.erase(it);
			break;
		}
	}
	if(isEmptyRtPackets()){
		m_control.increaseCwndSize();//增大拥塞窗口大小
		getRtPackets();
		beginSend();
	}
	m_lock.unlock();
	delete pPacket;
}
void NodeSender::beginSend(){
    m_time_lock.lock();
    m_control.setRtt(TimeUtils::microsecondTime(m_beginTime));//更改RoundTripTime
#ifndef WIN32
	gettimeofday(&m_beginTime,0);
#else
	QueryPerformanceCounter(&m_beginTime);
#endif
    m_time_lock.unlock();
    list<DataPacket*>::iterator it;
	for(it=m_rtPackets.begin();it!=m_rtPackets.end();it++){
		if(m_pSession->getLoginMode()==LAN){
			m_pSession->getNode()->sendPacket(*it);
		}else if(m_pSession->getLoginMode()==P2P){
			m_pSession->getNode()->sendPacket(*it);
		}else if(m_pSession->getLoginMode()==TURN){
			((PeerNode*)m_pSession->getNode())->sendTurnDataPacket(*it);
		}else if(m_pSession->getLoginMode()==SERVER_CLIENT){
			m_pSession->getNode()->sendPacket(*it);
		}
	}
}
void NodeSender::splitData(char* data, int len){
	int wrapDataLen = UDP_PACKET_SIZE - DataPacket::dataPacketHeadSize;
	DataPacket* pPacket;
	int offset = 0;
	while (offset < len) {
		if(m_pSession->getLoginMode()==LAN){
			pPacket = new DataPacket(m_pSession->getLocalHost(),m_pSession->getLocalPort());
		}else{
			pPacket = new DataPacket(m_pSession->getWanHost(),m_pSession->getWanPort());
		}
		pPacket->setPacketSeqNo(m_generate.getPacketSeqNo());
		if ((len - offset) >= wrapDataLen) {
			pPacket->writeWrapData(data,offset,wrapDataLen);
			offset = offset + wrapDataLen;
		} else {
			pPacket->writeWrapData(data,offset,len - offset);
			offset = len;
		}
		while(m_buffer.size()>=sendBufferSize){
			SimpleObject::wait();
		}
		m_lock.lock();
		m_buffer.push_back(pPacket);
		m_lock.unlock();
	}
}
void NodeSender::getRtPackets(){
	int i = m_rtPackets.size();
	int size = m_control.getCwndSize();
	DataPacket* pPacket=NULL;
	while (m_buffer.size()!=0 && i < size) {
		pPacket=m_buffer.front();
		m_buffer.pop_front();
		m_rtPackets.push_back(pPacket);
		i++;
	}
	if(m_buffer.size()==0){
		SimpleObject::notify();
	}
}
bool NodeSender::isEmptyRtPackets(){
	int size=m_rtPackets.size();
	return size==0?true:false;
}
/*NodeReceiver*/
NodeReceiver::NodeReceiver(NodeSession* pSession):m_pSession(pSession),m_buffer(),m_lastPacketSeqNo(0),m_lock(),m_logger("node.NodeReceiver"){
}
NodeReceiver::~NodeReceiver(){
}
void NodeReceiver::destroy(){
	m_lock.lock();
	list<DataPacket*>::iterator it;
	for(it=m_buffer.begin();it!=m_buffer.end();it++){
		delete *it;
	}
	m_buffer.clear();
	m_lock.unlock();
}
void NodeReceiver::processDataPacket(DataPacket* pPacket){
	m_lock.lock();
	if(isValidDataPacket(pPacket)){
		m_buffer.push_back(pPacket);
		if(getNextPacketSeqNo()==pPacket->getPacketSeqNo()){
			tryJoinDataPacket();
		}
	}else{
		delete pPacket;
		m_logger.info("接受到重复数据包，丢弃包\n");
	}
	m_lock.unlock();

}
bool NodeReceiver::isValidDataPacket(DataPacket* pPacket){
	if(m_lastPacketSeqNo==0){
		return true;
	}else{
		if(bufferContains(pPacket->getPacketSeqNo())){
			return false;
		}else{
			if(pPacket->getPacketSeqNo()>m_lastPacketSeqNo){
				return true;
			}else{
				if(m_lastPacketSeqNo>(GenerateNumber::uintMaxValue/2)
					&&pPacket->getPacketSeqNo()<(GenerateNumber::uintMaxValue/2)){
					return true;
				}else{
					return false;
				}
			}
		}
	}
}
bool NodeReceiver::bufferContains(UInt32 packetSeqNo){
	list<DataPacket*>::iterator it;
	for(it=m_buffer.begin();it!=m_buffer.end();it++){
		if((*it)->getPacketSeqNo()==packetSeqNo){
			return true;
		}
	}
	return false;
}
void NodeReceiver::tryJoinDataPacket(){
	list<DataPacket*> l;
	int len=0;
	DataPacket* pPacket;
	while((pPacket=nextDataPacket())!=NULL){
		len=len+pPacket->getWrapDataLen();
		l.push_back(pPacket);
	}
	char* data=new char[len+1];
	int offset=0;
	list<DataPacket*>::iterator it;
	for(it=l.begin();it!=l.end();it++){
		(*it)->readWrapData(data,offset);
		offset+=(*it)->getWrapDataLen();
		delete *it;
	}
	data[len]='\0';
	PeerInfo peerInfo;
    m_pSession->getPeerInfo(peerInfo);
    m_pSession->getNode()->getHandler()->receiveData(data,len,peerInfo);//数据处理完成,回调
	delete data;
}
DataPacket* NodeReceiver::nextDataPacket(){
	UInt32 nextPacketSeqNo=getNextPacketSeqNo();
	list<DataPacket*>::iterator it;
	DataPacket* pPacket=NULL;
	for(it=m_buffer.begin();it!=m_buffer.end();it++){
		if((*it)->getPacketSeqNo()==nextPacketSeqNo){
			pPacket=*it;
			m_buffer.erase(it);
			m_lastPacketSeqNo=nextPacketSeqNo;
			break;
		}
	}
	return pPacket;
}
UInt32 NodeReceiver::getNextPacketSeqNo(){
	UInt32 nextPacketSeqNo=m_lastPacketSeqNo+1;
	return nextPacketSeqNo>GenerateNumber::uintMaxValue?1:nextPacketSeqNo;
}
/*NodeSession*/
NodeSession::NodeSession(Node* pNode,const PeerInfo& peerInfo,int loginMode,bool isSendHeart):
	m_pNode(pNode),m_pHandler(pNode->getHandler()),m_peerInfo(peerInfo),m_natType(Symmetric_NAT),m_loginMode(loginMode),
	m_sender(this),m_receiver(this),m_isSendHeart(isSendHeart),m_lock(){
	m_lastReadTime=TimeUtils::currentTimeMillis();
}
NodeSession::~NodeSession(){
}
Node* NodeSession::getNode(){
	return m_pNode;
}
UInt64 NodeSession::getLastReadTime(){
	UInt64 i;
	m_lock.lock();
	i=m_lastReadTime;
	m_lock.unlock();
	return i;
}
void NodeSession::setLastReadTime(UInt64 lastReadTime){
	m_lock.lock();
	m_lastReadTime=lastReadTime;
	m_lock.unlock();
}
UInt32 NodeSession::getWanHost(){
	return m_peerInfo.m_wanHost;
}
UInt32 NodeSession::getWanPort(){
	return m_peerInfo.m_wanPort;
}
UInt32 NodeSession::getLocalHost(){
	return m_peerInfo.m_localHost;
}
UInt32 NodeSession::getLocalPort(){
	return m_peerInfo.m_localPort;
}
int NodeSession::getLoginMode(){
	return m_loginMode;
}
int NodeSession::getNatType(){
	return m_natType;
}
void NodeSession::setNatType(int natType){
	m_natType=natType;
}
void NodeSession::getPeerInfo(PeerInfo& peerInfo){
	peerInfo=m_peerInfo;
}
bool NodeSession::isSendHeart(){
	return m_isSendHeart;
}
NodeSender* NodeSession::getSender(){
	return &m_sender;
}
void NodeSession::sendData(char* data,int len){
    m_sender.sendData(data,len);
}
void NodeSession::processAckPacket(AckPacket* pPacket){
    m_sender.processAckPacket(pPacket);
}
void NodeSession::processDataPacket(DataPacket* pPacket){
    m_receiver.processDataPacket(pPacket);
}
void NodeSession::destroy(){
	m_sender.destroy();
	m_receiver.destroy();
}
/*JudgeNatPoint*/
JudgeNatPoint::JudgeNatPoint(UInt32 host,UInt32 port):
	EndPoint(host,port),m_host(host),m_port(port),m_pJudgeNatPoint(NULL),m_pServerNode(NULL),m_logger("node.JudgeNatPoint"){
}
JudgeNatPoint::JudgeNatPoint(UInt32 host,UInt32 port,JudgeNatPoint* pJudgeNatPoint,ServerNode* pServerNode):
	EndPoint(host,port),m_host(host),m_port(port),m_pJudgeNatPoint(pJudgeNatPoint),m_pServerNode(pServerNode),m_logger("node.JudgeNatPoint"){
}
JudgeNatPoint::~JudgeNatPoint(){
}
void JudgeNatPoint::sendPacket(Packet* pPacket){
	char* data=new char[pPacket->getBytesLen()];
	pPacket->readBytes(data);
	m_logger.info("send:");pPacket->printInfo();
	EndPoint::sendData(data,pPacket->getBytesLen(),pPacket->getHost(),pPacket->getPort());
	delete pPacket;
	delete data;
}
void JudgeNatPoint::receiveData(char* data,int length,UInt32 host,UInt32 port){
	bool isTurn=(data[0]&128)!=0;
	bool isControl=(data[0]&64)!=0;
	if(!isTurn){
		if(isControl){
			int controlPacketType=data[1];
			if(controlPacketType==JUDGE_NAT){
				JudgeNatPacket* pPacket=new JudgeNatPacket(host,port);
				pPacket->writeBytes(data,length);
				m_logger.info("receive:");pPacket->printInfo();
				JudgeNatAckPacket* pJudgeNatAckPacket=new JudgeNatAckPacket(pPacket->getWanHost(),pPacket->getWanPort());
				pJudgeNatAckPacket->setWanHost(pPacket->getWanHost());
				pJudgeNatAckPacket->setWanPort(pPacket->getWanPort());
				m_pJudgeNatPoint->sendPacket(pJudgeNatAckPacket);
				JudgeNatAckPacket* pJudgeNatAckPacket2=new JudgeNatAckPacket(pPacket->getWanHost(),pPacket->getWanPort());
				pJudgeNatAckPacket2->setWanHost(pPacket->getWanHost());
				pJudgeNatAckPacket2->setWanPort(pPacket->getWanPort());
				m_pServerNode->sendPacket(pJudgeNatAckPacket2);
				delete pPacket;
			}
		}
	}
}
/*ServerNode*/
ServerNode::ServerNode(UInt32 serverHost,UInt32 serverMainPort,UInt32 serverJudgeNatPort1,UInt32 serverJudgeNatPort2,UInt32 serverTurnPort):
	Node(),m_judgeNatPoint1(serverHost,serverJudgeNatPort1),m_judgeNatPoint2(serverHost,serverJudgeNatPort2,&m_judgeNatPoint1,this),
	m_serverHost(serverHost),m_serverMainPort(serverMainPort),m_serverJudgeNatPort1(serverJudgeNatPort1),m_serverJudgeNatPort2(serverJudgeNatPort2),
	m_serverTurnPort(serverTurnPort),m_logger("node.ServerNode"){
}
ServerNode::~ServerNode(){
	if(m_pEndPoint!=NULL){
		delete m_pEndPoint;
		delete m_pReleaseSession;
		delete m_pSenderTimeout;
	}
}
void ServerNode::start(){
	CoreExecutorService::newInstance();
	m_pEndPoint=new NodeEndPoint(m_serverHost,m_serverMainPort,this);
	m_pEndPoint->start();
	m_judgeNatPoint1.start();
	m_judgeNatPoint2.start();
	m_pReleaseSession=new ReleaseSessionWork(this);
	if(m_isReleaseSession){
		m_pReleaseSession->start();
	}
	m_pSenderTimeout=new SenderTimeoutWork(this);
	m_pSenderTimeout->start();
}
void ServerNode::shutdown(){
    m_pEndPoint->shutdown();
    m_judgeNatPoint1.shutdown();
    m_judgeNatPoint2.shutdown();
    m_pReleaseSession->stop();
    m_pSenderTimeout->stop();
    m_session_lock.lock();
    map<string,NodeSession*>::iterator it;
    for(it=m_pSessions.begin();it!=m_pSessions.end();){
    	delete it->second;
    	m_pSessions.erase(it++);
    }
    m_session_lock.unlock();
}
void ServerNode::waitForEnd(){
	m_pEndPoint->waitForEnd();
	m_judgeNatPoint1.waitForEnd();
	m_judgeNatPoint2.waitForEnd();
	m_pReleaseSession->waitForEnd();
	m_pSenderTimeout->waitForEnd();
}
int ServerNode::getLoginMode(const PeerInfo& peerInfo){
	NodeSession* pNode=NULL;
	pNode=getSession(peerInfo.m_wanHost,peerInfo.m_wanPort);
	if(pNode!=NULL){
		return pNode->getLoginMode();
	}
	return -1;
}
void ServerNode::doReleaseSession(){
	map<string,NodeSession*>::iterator it;
	NodeSession* pSession;
	m_session_lock.lock();
	for(it=m_pSessions.begin();it!=m_pSessions.end();){
		pSession=it->second;
		if((pSession->getLastReadTime()+60000)<TimeUtils::currentTimeMillis()){
			PeerInfo peerInfo;
			pSession->getPeerInfo(peerInfo);
			pSession->destroy();
			delete pSession;
			m_pSessions.erase(it++);
			m_pHandler->onDisconnectToPeer(peerInfo);
		}else{
			it++;
		}
	}
	m_session_lock.unlock();
}
void ServerNode::doSenderTimeout(){
	map<string,NodeSession*>::iterator it;
	NodeSession* pSession;
#ifndef WIN32
	timeval beginTime;
#else
	LARGE_INTEGER beginTime;
#endif
	m_session_lock.lock();
	for(it=m_pSessions.begin();it!=m_pSessions.end();it++){
		pSession=it->second;
		pSession->getSender()->getBeginTime(beginTime);
		if(TimeUtils::microsecondTime(beginTime)>pSession->getSender()->getControl()->getRto()){
			pSession->getSender()->continueSend();//超时重发
		}
	}
	m_session_lock.unlock();
}
void ServerNode::processLoginServerPacket(LoginServerPacket* pPacket){
    PeerInfo peerInfo(pPacket->getHost(),pPacket->getPort(),pPacket->getLocalHost(),pPacket->getLocalPort());
    NodeSession* pSession=new NodeSession(this,peerInfo,SERVER_CLIENT,false);
    addSession(pSession);
    LoginServerAckPacket* pAckPacket=new LoginServerAckPacket(peerInfo.m_wanHost,peerInfo.m_wanPort);
    pAckPacket->setWanHost(peerInfo.m_wanHost);
    pAckPacket->setWanPort(peerInfo.m_wanPort);
    pAckPacket->setServerJudgeNatPort1(m_serverJudgeNatPort1);
    pAckPacket->setServerJudgeNatPort2(m_serverJudgeNatPort2);
    pAckPacket->setServerTurnPort(m_serverTurnPort);
    sendPacket(pAckPacket);
    m_pHandler->onPeerLogin(peerInfo);
    delete pPacket;
}
void ServerNode::processHeartPacket(HeartPacket* pPacket){
    NodeSession* pSession=getSession(pPacket->getHost(),pPacket->getPort());
    if(pSession!=NULL){
    	pSession->setLastReadTime(TimeUtils::currentTimeMillis());
    	HeartAckPacket* pAckPacket=new HeartAckPacket(pPacket->getHost(),pPacket->getPort());
    	sendPacket(pAckPacket);
    }
    delete pPacket;
}
void ServerNode::processAckPacket(AckPacket* pPacket){
	NodeSession* pSession=getSession(pPacket->getHost(),pPacket->getPort());
	if(pSession!=NULL){
		pSession->processAckPacket(pPacket);
	}
}
void ServerNode::processReportPacket(ReportPacket* pPacket){
	NodeSession* pSession=getSession(pPacket->getHost(),pPacket->getPort());
	if(pSession!=NULL){
		pSession->setNatType(pPacket->getNatType());
		ReportAckPacket* pAckPacket=new ReportAckPacket(pPacket->getHost(),pPacket->getPort());
		sendPacket(pAckPacket);
	}
	delete pPacket;
}
void ServerNode::processWantHolePacket(WantHolePacket* pPacket){
	UInt32 wanHost=pPacket->getHost();
	UInt32 wanPort=pPacket->getPort();
	NodeSession* pSession=getSession(wanHost,wanPort);
	if(pSession!=NULL){
		UInt32 peerHost;//目标地址
		UInt32 peerPort;//目标端口
		peerHost=pPacket->getPeerHost();
		peerPort=pPacket->getPeerPort();
		NodeSession* pDestSession=getSession(peerHost,peerPort);
		if(pDestSession!=NULL){
			int direction=getHoleDirection(pSession->getNatType(),pDestSession->getNatType());
			NotifyHolePacket* pNotifyHolePacket=new NotifyHolePacket(peerHost,peerPort);
			pNotifyHolePacket->setPeerHost(wanHost);
			pNotifyHolePacket->setPeerPort(wanPort);
			pNotifyHolePacket->setPeerLocalHost(pSession->getLocalHost());
			pNotifyHolePacket->setPeerLocalPort(pSession->getLocalPort());
			pNotifyHolePacket->setDirection(direction);
			sendPacket(pNotifyHolePacket);
			WantHoleAckPacket* pWantHoleAckPacket=new WantHoleAckPacket(wanHost,wanPort);
			pWantHoleAckPacket->setPeerHost(peerHost);
			pWantHoleAckPacket->setPeerPort(peerPort);
			pWantHoleAckPacket->setPeerLocalHost(pPacket->getPeerLocalHost());
			pWantHoleAckPacket->setPeerLocalPort(pPacket->getPeerLocalPort());
			pWantHoleAckPacket->setDirection(direction);
			sendPacket(pWantHoleAckPacket);
		}
	}
	delete pPacket;
}
void ServerNode::processLogoutPacket(LogoutPacket* pPacket){
	NodeSession* pSession=getSession(pPacket->getHost(),pPacket->getPort());
	if(pSession!=NULL){
		PeerInfo peerInfo;
		pSession->getPeerInfo(peerInfo);
		removeSession(pPacket->getHost(),pPacket->getPort());
		m_pHandler->onPeerLogout(peerInfo);
	}
	delete pPacket;
}
void ServerNode::processDataPacket(DataPacket* pPacket){
	NodeSession* pSession=getSession(pPacket->getHost(),pPacket->getPort());
	if(pSession!=NULL){
		AckPacket* pAckPacket=new AckPacket(pPacket->getHost(),pPacket->getPort());
		pAckPacket->setPacketSeqNo(pPacket->getPacketSeqNo());
		sendPacket(pAckPacket);
		pSession->setLastReadTime(TimeUtils::currentTimeMillis());
		pSession->processDataPacket(pPacket);
	}
}
int ServerNode::getNatType(const PeerInfo& peerInfo){
	NodeSession* pSession=getSession(peerInfo.m_wanHost,peerInfo.m_wanPort);
	if(pSession!=NULL){
		return pSession->getNatType();
	}
    return Symmetric_NAT;
}
int ServerNode::getHoleDirection(int srcNatType,int destNatType){
	if(srcNatType==No_NAT){
		return To_SRC;
	}else if(srcNatType==Full_Cone_NAT_OR_Restricted_Cone_NAT){
		return To_SRC;
	}else if(srcNatType==Port_Restricted_Cone_NAT){
		if(destNatType!=Symmetric_NAT){
			return To_SRC;
		}
	}else if(srcNatType==Symmetric_NAT){
		if(destNatType!=Symmetric_NAT&&destNatType!=Port_Restricted_Cone_NAT){
			return To_DEST;
		}
	}
	return CAN_NOT_HOLE;
}
/*PeerNode*/
PeerNode::PeerNode(bool supportTurn):Node(),m_supportTurn(supportTurn),m_serverHost(0),m_serverMainPort(0),
	m_serverJudgeNatPort1(0),m_serverJudgeNatPort2(0),m_serverTurnPort(0),
	m_wanHost(0),m_wanPort(0),m_localHost(0),m_localPort(0),m_localNatType(Symmetric_NAT),
	m_pServerSession(NULL),m_mainJudgeNatHaveComing(false),m_logger("node.PeerNode"),m_lock(),m_pTurnHeart(NULL),m_pHeart(NULL){
}
PeerNode::PeerNode(UInt32 localHost,UInt32 localPort,bool supportTurn):Node(),m_supportTurn(supportTurn),m_serverHost(0),
	m_serverMainPort(0),m_serverJudgeNatPort1(0),m_serverJudgeNatPort2(0),m_serverTurnPort(0),
	m_wanHost(0),m_wanPort(0),m_localHost(localHost),m_localPort(localPort),m_localNatType(Symmetric_NAT),
	m_pServerSession(NULL),m_mainJudgeNatHaveComing(false),m_logger("node.PeerNode"),m_lock(),m_pTurnHeart(NULL),m_pHeart(NULL){
}
PeerNode::~PeerNode(){
	if(m_pEndPoint!=NULL){
		delete m_pEndPoint;
		delete m_pReleaseSession;
		delete m_pSenderTimeout;
		delete m_pHeart;
	}
	if(m_pTurnHeart!=NULL){
		delete m_pTurnHeart;
	}
	if(m_pServerSession!=NULL){
	    delete m_pServerSession;
	}
}
bool PeerNode::isServerSession(UInt32 host,UInt32 port){
    if(host==m_serverHost&&port==m_serverMainPort){
    	return true;
    }
	return false;
}
void PeerNode::getLocalPeerInfo(PeerInfo& peerInfo){
    peerInfo.m_wanHost=m_wanHost;
    peerInfo.m_wanPort=m_wanPort;
    peerInfo.m_localHost=m_localHost;
    peerInfo.m_localPort=m_localPort;
}
void PeerNode::getServerPeerInfo(PeerInfo& peerInfo){
	peerInfo.m_wanHost=m_serverHost;
	peerInfo.m_wanPort=m_serverMainPort;
	peerInfo.m_localHost=m_serverHost;
	peerInfo.m_localPort=m_serverMainPort;
}
int PeerNode::getLocalNatType(){
	return m_localNatType;
}
NodeSession* PeerNode::getServerSession(){
	return m_pServerSession;
}
void PeerNode::start(){
	CoreExecutorService::newInstance();
	if(m_localPort!=0){
		m_pEndPoint=new NodeEndPoint(m_localHost,m_localPort,this);
		m_pEndPoint->start();
	}else{
		m_pEndPoint=new NodeEndPoint(this);
		m_pEndPoint->start();
		m_localHost=m_pEndPoint->getHost();
		m_localPort=m_pEndPoint->getPort();
	}
	m_pHeart=new NodeHeart(this);
	if(m_isSendHeart){
		m_pHeart->start();
	}
	m_pReleaseSession=new ReleaseSessionWork(this);
	if(m_isReleaseSession){
		m_pReleaseSession->start();
	}
	m_pSenderTimeout=new SenderTimeoutWork(this);
	m_pSenderTimeout->start();
}
void PeerNode::shutdown(){
	m_pEndPoint->shutdown();
	m_pHeart->stop();
	m_pSenderTimeout->stop();
	if(m_pTurnHeart!=NULL){
		m_pTurnHeart->stop();
	}
	m_session_lock.lock();
	map<string,NodeSession*>::iterator it;
	for(it=m_pSessions.begin();it!=m_pSessions.end();it++){
		delete it->second;
	}
	m_pSessions.clear();
	m_session_lock.unlock();
	m_pReleaseSession->stop();
}
void PeerNode::waitForEnd(){
	m_pEndPoint->waitForEnd();
	m_pReleaseSession->waitForEnd();
	m_pSenderTimeout->waitForEnd();
	m_pHeart->waitForEnd();
	if(m_pTurnHeart!=NULL){
		m_pTurnHeart->waitForEnd();
	}
}
int PeerNode::getLoginMode(const PeerInfo& peerInfo){
	if(isServerSession(peerInfo.m_wanHost,peerInfo.m_wanPort)){
		return m_pServerSession->getLoginMode();
	}else{
		NodeSession* pSession=getSession(peerInfo.m_wanHost,peerInfo.m_wanPort);
		if(pSession!=NULL){
			return pSession->getLoginMode();
		}
	}
	return -1;
}
void PeerNode::doReleaseSession(){
	if(m_pServerSession!=NULL){
		if((m_pServerSession->getLastReadTime()+60000)<TimeUtils::currentTimeMillis()){//1分钟未收到数据,则认为过期
			CoreExecutorService::getInstance()->execute(new OnDisconnectToServer((PeerNodeHandler*)m_pHandler));
			return;
		}
	}
	map<string,NodeSession*>::iterator it;
	NodeSession* pSession;
	m_session_lock.lock();
	for(it=m_pSessions.begin();it!=m_pSessions.end();){
		pSession=it->second;
		if((pSession->getLastReadTime()+60000)<TimeUtils::currentTimeMillis()){
			PeerInfo peerInfo;
			pSession->getPeerInfo(peerInfo);
			pSession->destroy();
			delete pSession;
			m_pSessions.erase(it++);
			m_pHandler->onDisconnectToPeer(peerInfo);
		}else{
			it++;
		}
	}
	m_session_lock.unlock();
}
void PeerNode::doSenderTimeout(){
#ifndef WIN32
	timeval beginTime;
#else
	LARGE_INTEGER beginTime;
#endif
	if(m_pServerSession!=NULL){
		m_pServerSession->getSender()->getBeginTime(beginTime);
		if(TimeUtils::microsecondTime(beginTime)>m_pServerSession->getSender()->getControl()->getRto()){
			m_pServerSession->getSender()->continueSend();//超时重发
		}
	}
	map<string,NodeSession*>::iterator it;
	NodeSession* pSession;
	m_session_lock.lock();
	for(it=m_pSessions.begin();it!=m_pSessions.end();it++){
		pSession=it->second;
		pSession->getSender()->getBeginTime(beginTime);
		if(TimeUtils::microsecondTime(beginTime)>pSession->getSender()->getControl()->getRto()){
			pSession->getSender()->continueSend();//超时重发
		}
	}
	m_session_lock.unlock();
}
void PeerNode::logoutAllAndShutdown(){
	m_session_lock.lock();
	map<string,NodeSession*>::iterator it;
	LogoutPacket* pLogoutPacket=NULL;
	for(it=m_pSessions.begin();it!=m_pSessions.end();it++){
		PeerInfo peerInfo;
		it->second->getPeerInfo(peerInfo);
		if(it->second->getLoginMode()==LAN){
			pLogoutPacket=new LogoutPacket(peerInfo.m_localHost,peerInfo.m_localPort);
			sendPacket(pLogoutPacket);
		}else if(it->second->getLoginMode()==TURN){
			pLogoutPacket=new LogoutPacket(peerInfo.m_wanHost,peerInfo.m_wanPort);
			sendTurnControlPacket(pLogoutPacket);
		}else if(it->second->getLoginMode()==P2P){
			pLogoutPacket=new LogoutPacket(peerInfo.m_wanHost,peerInfo.m_wanPort);
			sendPacket(pLogoutPacket);
		}
	}
	m_session_lock.unlock();
	pLogoutPacket=new LogoutPacket(m_serverHost,m_serverMainPort);
	sendPacket(pLogoutPacket);
	shutdown();
}
void PeerNode::sendTurnControlPacket(Packet* pPacket){
	TurnControlPacket* pControlPacket=new TurnControlPacket(m_serverHost,m_serverTurnPort);
	pControlPacket->setPacket(pPacket);
	char* data=new char[pControlPacket->getBytesLen()];
	pControlPacket->readBytes(data);
	m_logger.info("send:");pControlPacket->printInfo();
	m_pEndPoint->sendData(data,pControlPacket->getBytesLen(),m_serverHost,m_serverTurnPort);
	delete pPacket;
	delete pControlPacket;
	delete data;
}
void PeerNode::sendTurnDataPacket(DataPacket* pPacket){
	TurnDataPacket* pDataPacket=new TurnDataPacket(m_serverHost,m_serverTurnPort);
	pDataPacket->setDataPacket(pPacket);
	char* data=new char[pDataPacket->getBytesLen()];
	pDataPacket->readBytes(data);
	m_logger.info("send:");pDataPacket->printInfo();
	m_pEndPoint->sendData(data,pDataPacket->getBytesLen(),m_serverHost,m_serverTurnPort);
	delete pDataPacket;
	delete data;
}
void PeerNode::sendTurnHeartPacket(){
	TurnHeartPacket* pPacket=new TurnHeartPacket(m_serverHost,m_serverTurnPort);
	char* data=new char[pPacket->getBytesLen()];
	pPacket->readBytes(data);
	m_logger.info("send:");pPacket->printInfo();
	m_pEndPoint->sendData(data,pPacket->getBytesLen(),m_serverHost,m_serverTurnPort);
	delete pPacket;
	delete data;
}
void PeerNode::loginServer(UInt32 host,UInt32 port){
	m_mainJudgeNatHaveComing=false;
	m_serverHost=host;
	m_serverMainPort=port;
	LoginServerPacket* pLoginServerPacket=new LoginServerPacket(host,port);
	pLoginServerPacket->setLocalHost(m_localHost);
	pLoginServerPacket->setLocalPort(m_localPort);
	sendPacket(pLoginServerPacket);
}
void PeerNode::loginPeer(const PeerInfo& peerInfo,int loginMode){
	LoginPeerPacket* pLoginPeerPacket=NULL;
	if(loginMode==LAN){
		pLoginPeerPacket=new LoginPeerPacket(peerInfo.m_localHost,peerInfo.m_localPort);
		pLoginPeerPacket->setLoginMode(LAN);
	}else if(loginMode==TURN){
		pLoginPeerPacket=new LoginPeerPacket(peerInfo.m_wanHost,peerInfo.m_wanPort);
		pLoginPeerPacket->setLoginMode(TURN);
	}
	if(pLoginPeerPacket!=NULL){
		pLoginPeerPacket->setWanHost(m_wanHost);
		pLoginPeerPacket->setWanPort(m_wanPort);
		pLoginPeerPacket->setLocalHost(m_localHost);
		pLoginPeerPacket->setLocalPort(m_localPort);
	}
	if(loginMode==LAN){
		sendPacket(pLoginPeerPacket);
	}else if(loginMode==P2P){
		WantHolePacket* pWantHolePacket=new WantHolePacket(m_serverHost,m_serverMainPort);
		pWantHolePacket->setPeerHost(peerInfo.m_wanHost);
		pWantHolePacket->setPeerPort(peerInfo.m_wanPort);
		pWantHolePacket->setPeerLocalHost(peerInfo.m_localHost);
		pWantHolePacket->setPeerLocalPort(peerInfo.m_localPort);
		sendPacket(pWantHolePacket);
	}else if(loginMode==TURN){
		sendTurnControlPacket(pLoginPeerPacket);
	}
}
void PeerNode::logout(const PeerInfo& peerInfo){
	LogoutPacket* pLogoutPacket=NULL;
	if(isServerSession(peerInfo.m_wanHost,peerInfo.m_wanPort)&&m_pServerSession!=NULL){
		pLogoutPacket=new LogoutPacket(m_serverHost,m_serverMainPort);
		sendPacket(pLogoutPacket);
		shutdown();
	}else{
		NodeSession* pSession=getSession(peerInfo.m_wanHost,peerInfo.m_wanPort);
		if(pSession!=NULL){
			if(pSession->getLoginMode()==LAN){
				pLogoutPacket=new LogoutPacket(peerInfo.m_localHost,peerInfo.m_localPort);
				sendPacket(pLogoutPacket);
			}else if(pSession->getLoginMode()==TURN){
				pLogoutPacket=new LogoutPacket(peerInfo.m_wanHost,peerInfo.m_wanPort);
				sendTurnControlPacket(pLogoutPacket);
			}else if(pSession->getLoginMode()==P2P){
				pLogoutPacket=new LogoutPacket(peerInfo.m_wanHost,peerInfo.m_wanPort);
				sendPacket(pLogoutPacket);
			}
			removeSession(peerInfo.m_wanHost,peerInfo.m_wanPort);
		}
	}
}
void PeerNode::doSendHeart(){
	HeartPacket* pHeartPacket;
	if(m_pServerSession!=NULL){
		pHeartPacket=new HeartPacket(m_pServerSession->getWanHost(),m_pServerSession->getWanPort());
		sendPacket(pHeartPacket);
	}
	map<string,NodeSession*>::iterator it;
	NodeSession* pSession;
	m_session_lock.lock();
	for(it=m_pSessions.begin();it!=m_pSessions.end();it++){
		pSession=it->second;
		if(pSession->isSendHeart()){
			if(pSession->getLoginMode()==LAN){
				pHeartPacket=new HeartPacket(pSession->getLocalHost(),pSession->getLocalPort());
				sendPacket(pHeartPacket);
			}else if(pSession->getLoginMode()==P2P){
				pHeartPacket=new HeartPacket(pSession->getWanHost(),pSession->getWanPort());
				sendPacket(pHeartPacket);
			}else if(pSession->getLoginMode()==TURN){
				pHeartPacket=new HeartPacket(pSession->getWanHost(),pSession->getWanPort());
				sendTurnControlPacket(pHeartPacket);
			}
		}
	}
	m_session_lock.unlock();
}
void PeerNode::processLoginServerAckPacket(LoginServerAckPacket* pPacket){
	if(m_pServerSession!=NULL){
		m_pServerSession->destroy();
		delete m_pServerSession;
	}
	m_serverJudgeNatPort1=pPacket->getServerJudgeNatPort1();
	m_serverJudgeNatPort2=pPacket->getServerJudgeNatPort2();
	m_serverTurnPort=pPacket->getServerTurnPort();
	m_wanHost=pPacket->getWanHost();
	m_wanPort=pPacket->getWanPort();
	PeerInfo peerInfo(m_serverHost,m_serverMainPort,m_serverHost,m_serverMainPort);
	m_pServerSession=new NodeSession(this,peerInfo,SERVER_CLIENT,true);
	if(m_localHost==m_wanHost){
		m_localNatType=No_NAT;
		report();//将自己的Nat类型告诉服务器
		((PeerNodeHandler*)m_pHandler)->onLoginServerSuccess(peerInfo);
	}else{
		JudgeNatPacket* pJudgeNatPacket=new JudgeNatPacket(m_serverHost,m_serverJudgeNatPort2);
		pJudgeNatPacket->setWanHost(m_wanHost);
		pJudgeNatPacket->setWanPort(m_wanPort);
		sendPacket(pJudgeNatPacket);//向服务器发包,进而判断Nat类型
	}
	if(m_supportTurn&&m_pTurnHeart==NULL){
		m_pTurnHeart=new TurnHeart(this);
		if(m_isSendHeart){
			m_pTurnHeart->start();
		}
	}
	delete pPacket;
}
void PeerNode::processLoginPeerPacket(LoginPeerPacket* pPacket){
	m_lock.lock();
	PeerInfo peerInfo;
	peerInfo.m_wanHost=pPacket->getWanHost();
	peerInfo.m_wanPort=pPacket->getWanPort();
	peerInfo.m_localHost=pPacket->getLocalHost();
	peerInfo.m_localPort=pPacket->getLocalPort();
	LoginPeerAckPacket* pLoginPeerAckPacket=NULL;
	if(pPacket->getLoginMode()==LAN){
		pLoginPeerAckPacket=new LoginPeerAckPacket(peerInfo.m_localHost,peerInfo.m_localPort);
	}else if(pPacket->getLoginMode()==TURN){
		pLoginPeerAckPacket=new LoginPeerAckPacket(peerInfo.m_wanHost,peerInfo.m_wanPort);
	}else if(pPacket->getLoginMode()==P2P){
		pLoginPeerAckPacket=new LoginPeerAckPacket(peerInfo.m_wanHost,peerInfo.m_wanPort);
	}
	NodeSession* pSession=new NodeSession(this,peerInfo,pPacket->getLoginMode(),false);
	addSession(pSession);
	pLoginPeerAckPacket->setWanHost(m_wanHost);
	pLoginPeerAckPacket->setWanPort(m_wanPort);
	pLoginPeerAckPacket->setLocalHost(m_localHost);
	pLoginPeerAckPacket->setLocalPort(m_localPort);
	pLoginPeerAckPacket->setLoginMode(pPacket->getLoginMode());
	if(pPacket->getLoginMode()==TURN){
		sendTurnControlPacket(pLoginPeerAckPacket);
	}else{
		sendPacket(pLoginPeerAckPacket);
	}
	m_lock.unlock();
	((PeerNodeHandler*)m_pHandler)->onPeerLogin(peerInfo);
	delete pPacket;
}
void PeerNode::processLoginPeerAckPacket(LoginPeerAckPacket* pPacket){
	m_lock.lock();
	PeerInfo peerInfo;
	peerInfo.m_wanHost=pPacket->getWanHost();
	peerInfo.m_wanPort=pPacket->getWanPort();
	peerInfo.m_localHost=pPacket->getLocalHost();
	peerInfo.m_localPort=pPacket->getLocalPort();
	NodeSession* pSession=new NodeSession(this,peerInfo,pPacket->getLoginMode(),true);
	addSession(pSession);
	m_lock.unlock();
	((PeerNodeHandler*)m_pHandler)->onLoginPeerSuccess(peerInfo);
	delete pPacket;
}
void PeerNode::processHeartPacket(HeartPacket* pPacket){
	NodeSession* pSession=NULL;
	if(CommonUtils::isLocalIp(pPacket->getHost())){
		pSession=getSessionByLocal(pPacket->getHost(),pPacket->getPort());
	}else{
		pSession=getSession(pPacket->getHost(),pPacket->getPort());
	}
	if(pSession!=NULL){
		pSession->setLastReadTime(TimeUtils::currentTimeMillis());
		HeartAckPacket* pHeartAckPacket=new HeartAckPacket(pPacket->getHost(),pPacket->getPort());
		if(pSession->getLoginMode()==TURN){
			sendTurnControlPacket(pHeartAckPacket);
		}else{
			sendPacket(pHeartAckPacket);
		}
	}
	delete pPacket;
}
void PeerNode::processHeartAckPacket(HeartAckPacket* pPacket){
	if(isServerSession(pPacket->getHost(),pPacket->getPort())){
		m_pServerSession->setLastReadTime(TimeUtils::currentTimeMillis());
	}else{
		NodeSession* pSession=NULL;
		if(CommonUtils::isLocalIp(pPacket->getHost())){
			pSession=getSessionByLocal(pPacket->getHost(),pPacket->getPort());
		}else{
			pSession=getSession(pPacket->getHost(),pPacket->getPort());
		}
		if(pSession!=NULL){
			pSession->setLastReadTime(TimeUtils::currentTimeMillis());
		}
	}
	delete pPacket;
}
void PeerNode::processAckPacket(AckPacket* pPacket){
	if(isServerSession(pPacket->getHost(),pPacket->getPort())){
		m_pServerSession->processAckPacket(pPacket);
	}else{
		NodeSession* pSession;
		if(CommonUtils::isLocalIp(pPacket->getHost())){
			pSession=getSessionByLocal(pPacket->getHost(),pPacket->getPort());
		}else{
			pSession=getSession(pPacket->getHost(),pPacket->getPort());
		}
		if(pSession!=NULL){
			pSession->processAckPacket(pPacket);
		}
	}
}
void PeerNode::processJudgeNatAckPacket(JudgeNatAckPacket* pPacket){
	if(m_localNatType!=No_NAT){
		PeerInfo serverPeerInfo;
		getServerPeerInfo(serverPeerInfo);
		if(pPacket->getPort()==m_serverMainPort){//主通讯端口数据
			if(m_localNatType!=Full_Cone_NAT_OR_Restricted_Cone_NAT){
				m_mainJudgeNatHaveComing=true;
				if(pPacket->getWanPort()==m_wanPort&&pPacket->getWanHost()==m_wanHost){
					m_localNatType=Port_Restricted_Cone_NAT;
				}else{
					m_localNatType=Symmetric_NAT;
				}
				report();
				((PeerNodeHandler*)m_pHandler)->onLoginServerSuccess(serverPeerInfo);
			}
		}else if(pPacket->getPort()==m_serverJudgeNatPort1){//说明是JdugeNatServer返回的数据包,决大部分网络收不到该包
			m_localNatType=Full_Cone_NAT_OR_Restricted_Cone_NAT;
			report();
			if(!m_mainJudgeNatHaveComing){
				((PeerNodeHandler*)m_pHandler)->onLoginServerSuccess(serverPeerInfo);
			}
		}
	}
	delete pPacket;
}
void PeerNode::processWantHoleAckPacket(WantHoleAckPacket* pPacket){
	if(pPacket->getDirection()==To_SRC){
		tryP2PLogin(pPacket->getPeerHost(),pPacket->getPeerPort());
	}else if(pPacket->getDirection()==To_DEST){
		trySendHolePacket(pPacket->getPeerHost(),pPacket->getPeerPort());
	}else if(pPacket->getDirection()==CAN_NOT_HOLE){
		PeerInfo peerInfo(pPacket->getPeerHost(),pPacket->getPeerPort(),pPacket->getPeerLocalHost(),pPacket->getPeerLocalPort());
		((PeerNodeHandler*)m_pHandler)->onLoginPeerFailure(peerInfo,P2P);
	}
	delete pPacket;
}
void PeerNode::processNotifyHolePacket(NotifyHolePacket* pPacket){
	if(pPacket->getDirection()==To_SRC){
		trySendHolePacket(pPacket->getPeerHost(),pPacket->getPeerPort());
	}else if(pPacket->getDirection()==To_DEST){
		tryP2PLogin(pPacket->getPeerHost(),pPacket->getPeerPort());
	}
	delete pPacket;
}
void PeerNode::processLogoutPacket(LogoutPacket* pPacket){
	NodeSession* pSession=NULL;
	if(CommonUtils::isLocalIp(pPacket->getHost())){
		pSession=getSession(pPacket->getHost(),pPacket->getPort());
	}else{
		pSession=getSessionByLocal(pPacket->getHost(),pPacket->getPort());
	}
	if(pSession!=NULL){
		PeerInfo peerInfo;
		pSession->getPeerInfo(peerInfo);
		removeSession(pSession->getWanHost(),pSession->getWanPort());
		((PeerNodeHandler*)m_pHandler)->onPeerLogout(peerInfo);
	}
	delete pPacket;
}
void PeerNode::processDataPacket(DataPacket* pPacket){
	if(isServerSession(pPacket->getHost(),pPacket->getPort())){
		AckPacket* pAckPacket=new AckPacket(pPacket->getHost(),pPacket->getPort());
		pAckPacket->setPacketSeqNo(pPacket->getPacketSeqNo());
		sendPacket(pAckPacket);
		m_pServerSession->setLastReadTime(TimeUtils::currentTimeMillis());
		m_pServerSession->processDataPacket(pPacket);
	}else{
		NodeSession* pSession=NULL;
		if(CommonUtils::isLocalIp(pPacket->getHost())){
			pSession=getSessionByLocal(pPacket->getHost(),pPacket->getPort());
		}else{
			pSession=getSession(pPacket->getHost(),pPacket->getPort());
		}
		if(pSession!=NULL){
			AckPacket* pAckPacket=new AckPacket(pPacket->getHost(),pPacket->getPort());
			pAckPacket->setPacketSeqNo(pPacket->getPacketSeqNo());
			if(pSession->getLoginMode()==TURN){
				sendTurnControlPacket(pAckPacket);
			}else{
				sendPacket(pAckPacket);
			}
			pSession->setLastReadTime(TimeUtils::currentTimeMillis());
			pSession->processDataPacket(pPacket);
		}
	}
}
void PeerNode::report(){
	ReportPacket* pReportPacket=new ReportPacket(m_serverHost,m_serverMainPort);
	pReportPacket->setNatType(m_localNatType);
	sendPacket(pReportPacket);
}
void PeerNode::trySendHolePacket(UInt32 host,UInt32 port){
	SendHolePacket *pShp=new SendHolePacket(host,port,this);
	CoreExecutorService::getInstance()->execute(pShp);
}
void PeerNode::tryP2PLogin(UInt32 host,UInt32 port){
	PeerInfo peerInfo;
	getLocalPeerInfo(peerInfo);
	P2PLogin* pPl=new P2PLogin(host,port,this,peerInfo);
	CoreExecutorService::getInstance()->execute(pPl);
}
/*TurnHeart*/
TurnHeart::TurnHeart(PeerNode* pNode):DoRepeatWork(),m_pNode(pNode),m_logger("node.TurnHeart"),m_repeatWork(this,&m_logger){
}
TurnHeart::~TurnHeart(){
}
void TurnHeart::start(){
	m_repeatWork.start();
}
void TurnHeart::stop(){
	m_repeatWork.stop();
}
void TurnHeart::waitForEnd(){
	m_repeatWork.waitForEnd();
}
void TurnHeart::doRepeatWork(){
	m_pNode->sendTurnHeartPacket();
	m_repeatWork.wait(15000);
}
/*NodeHeart*/
NodeHeart::NodeHeart(PeerNode* pNode):DoRepeatWork(),m_pNode(pNode),m_logger("node.NodeHeart"),m_repeatWork(this,&m_logger){
}
NodeHeart::~NodeHeart(){
}
void NodeHeart::start(){
	m_repeatWork.start();
}
void NodeHeart::stop(){
	m_repeatWork.stop();
}
void NodeHeart::waitForEnd(){
	m_repeatWork.waitForEnd();
}
void NodeHeart::doRepeatWork(){
	m_pNode->doSendHeart();
	m_repeatWork.wait(15000);
}
/*ReleaseSessionWork*/
ReleaseSessionWork::ReleaseSessionWork(Node* pNode):DoRepeatWork(),m_pNode(pNode),m_logger("node.ReleaseSessionWork"),m_repeatWork(this,&m_logger){
}
ReleaseSessionWork::~ReleaseSessionWork(){
}
void ReleaseSessionWork::start(){
	m_repeatWork.start();
}
void ReleaseSessionWork::stop(){
	m_repeatWork.stop();
}
void ReleaseSessionWork::waitForEnd(){
	m_repeatWork.waitForEnd();
}
void ReleaseSessionWork::doRepeatWork(){
	m_pNode->doReleaseSession();
	m_repeatWork.wait(30000);
}
/*SenderTimeoutWork*/
SenderTimeoutWork::SenderTimeoutWork(Node* pNode):DoRepeatWork(),m_pNode(pNode),m_logger("node.SenderTimeoutWork"),m_repeatWork(this,&m_logger){
}
SenderTimeoutWork::~SenderTimeoutWork(){
}
void SenderTimeoutWork::start(){
	m_repeatWork.start();
}
void SenderTimeoutWork::stop(){
	m_repeatWork.stop();
}
void SenderTimeoutWork::waitForEnd(){
	m_repeatWork.waitForEnd();
}
void SenderTimeoutWork::doRepeatWork(){
	m_pNode->doSenderTimeout();
	m_repeatWork.wait(10);//0.01秒跑一次
}
