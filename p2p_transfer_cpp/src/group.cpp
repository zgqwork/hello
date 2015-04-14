#include <typeinfo>
#ifdef WIN32
#include <time.h>
#endif
#include "group.h"
/*GroupInfo*/
GroupInfo::GroupInfo():m_peerInfos(),m_indexInfos(),m_lock(),m_logger("group.GroupInfo"){
	srand((unsigned)time(NULL));
}
GroupInfo::~GroupInfo(){
}
void GroupInfo::addPeerInfo(const PeerInfo& peerInfo){
	m_lock.lock();
	string key;
	CommonUtils::toIpPortKey(peerInfo.m_wanHost,peerInfo.m_wanPort,key);
	map<string,PeerInfo>::iterator it;
	it=m_peerInfos.find(key);
	if(it!=m_peerInfos.end()){
		m_peerInfos.erase(it);
	}
	m_peerInfos.insert(pair<string,PeerInfo>(key,peerInfo));
	m_lock.unlock();
}
void GroupInfo::removePeerInfo(const PeerInfo& peerInfo){
	m_lock.lock();
	string key;
	CommonUtils::toIpPortKey(peerInfo.m_wanHost,peerInfo.m_wanPort,key);
	map<string,PeerInfo>::iterator it;
	it=m_peerInfos.find(key);
	if(it!=m_peerInfos.end()){
		m_peerInfos.erase(it);
	}
	m_lock.unlock();
}
void GroupInfo::addIndex(const PeerInfo& peerInfo,int firstIndex,int lastIndex){
	m_lock.lock();
	int i;
	map<int,list<PeerInfo> >::iterator it;
	list<PeerInfo>::iterator iter;
	for(i=firstIndex;i<=lastIndex;i++){
		it=m_indexInfos.find(i);
		if(it==m_indexInfos.end()){
			list<PeerInfo> peerInfos;
			peerInfos.push_back(peerInfo);
			m_indexInfos.insert(pair<int,list<PeerInfo> >(i,peerInfos));
		}else{
			bool exist=false;
			for(iter=it->second.begin();iter!=it->second.end();iter++){
				if(*iter==peerInfo){
					exist=true;
					break;
				}
			}
			if(!exist){
				it->second.push_back(peerInfo);
			}
		}
	}
	m_lock.unlock();
}
void GroupInfo::removeIndex(const PeerInfo& peerInfo,int firstIndex,int lastIndex){
	m_lock.lock();
	int i;
	map<int,list<PeerInfo> >::iterator it;
	list<PeerInfo>::iterator iter;
	for(i=firstIndex;i<=lastIndex;i++){
		it=m_indexInfos.find(i);
		if(it!=m_indexInfos.end()){
			for(iter=it->second.begin();iter!=it->second.end();iter++){
				if(*iter==peerInfo){
					it->second.erase(iter);
					break;
				}
			}
		}
	}
	m_lock.unlock();
}
void GroupInfo::removePeerInfoAllIndex(const PeerInfo& peerInfo){
	m_lock.lock();
	map<int,list<PeerInfo> >::iterator it;
    list<PeerInfo>::iterator iter;
	for(it=m_indexInfos.begin();it!=m_indexInfos.end();it++){
		for(iter=it->second.begin();iter!=it->second.end();iter++){
			if(*iter==peerInfo){
				it->second.erase(iter);
				break;
			}
		}
	}
	m_lock.unlock();
}
bool GroupInfo::whichHaveData(int index,PeerInfo& peerInfo){
	bool exist=false;
	m_lock.lock();
	map<int,list<PeerInfo> >::iterator it;
	list<PeerInfo>::iterator iter;
	it=m_indexInfos.find(index);
	if(it!=m_indexInfos.end()){
		int i=rand()%(it->second.size());
		int count=0;
		for(iter=it->second.begin();iter!=it->second.end();iter++){
			if(count==i){
				peerInfo=*iter;
				exist=true;
				break;
			}
			count++;
		}
	}
	m_lock.unlock();
	return exist;
}
void GroupInfo::getPeerInfos(list<PeerInfo>& peerInfos){
	m_lock.lock();
	map<string,PeerInfo>::iterator it;
	for(it=m_peerInfos.begin();it!=m_peerInfos.end();it++){
		peerInfos.push_back(it->second);
	}
	m_lock.unlock();
}
void GroupInfo::printPeerInfos(){
	m_lock.lock();
	map<string,PeerInfo>::iterator it;
	for(it=m_peerInfos.begin();it!=m_peerInfos.end();it++){
		it->second.printInfo();
	}
	m_lock.unlock();
}
void GroupInfo::printIndexInfos(){
	m_lock.lock();
	map<int,list<PeerInfo> >::iterator it;
	list<PeerInfo>::iterator iter;
	for(it=m_indexInfos.begin();it!=m_indexInfos.end();it++){
		m_logger.info("index:%d,",it->first);
		for(iter=it->second.begin();iter!=it->second.end();iter++){
			(*iter).printInfo();
		}
		m_logger.info("----------------------------------------\n");
	}
	m_lock.unlock();
}
/*GroupNodeSession*/
GroupNodeSession::GroupNodeSession(GroupSender* pSender,GroupReceiver* pReceiver,const PeerInfo& peerInfo):
	m_groupName(),m_pGroupInfo(NULL),m_pSender(pSender),m_pReceiver(pReceiver),m_pBufferPacket(NULL),m_logger("group.GroupNodeSession"){
	m_peerInfo=peerInfo;
	m_pBufferPacket=new BufferPacket(this);
}
GroupNodeSession::~GroupNodeSession(){
	delete m_pBufferPacket;
}
void GroupNodeSession::processData(char* data,int len){
	m_pBufferPacket->processData(data,0,len);
}
void GroupNodeSession::sendPacket(GroupPacket* pGroupPacket){
	m_logger.info("send group packet:");pGroupPacket->printInfo();
	char* data=new char[WrapPacket::headSize+pGroupPacket->getBytesLen()];
	WrapPacket::toBytes(pGroupPacket,data);
	//CommonUtils::printBytes(data,0,WrapPacket::headSize+pGroupPacket->getBytesLen());
	m_pSender->sendData(data,WrapPacket::headSize+pGroupPacket->getBytesLen(),m_peerInfo);
	delete pGroupPacket;
	delete data;
}
/*BufferPacket*/
BufferPacket::BufferPacket(GroupNodeSession* pSession):
	m_pSession(pSession),m_headData(NULL),m_completeHead(false),m_currentHeadLen(0),m_bodyData(NULL),m_bodyLen(0),m_currentBodyLen(0){
    m_headData=new char[WrapPacket::headSize];
}
BufferPacket::~BufferPacket(){
	delete m_headData;
}
void BufferPacket::processData(char* data,int offset,int len){
	if(!m_completeHead){
		if((len-offset)>=(WrapPacket::headSize-m_currentHeadLen)){
			memcpy(m_headData+m_currentHeadLen,data+offset,WrapPacket::headSize-m_currentHeadLen);
			offset+=WrapPacket::headSize-m_currentHeadLen;
			m_currentHeadLen=WrapPacket::headSize;
			m_completeHead=true;
			m_bodyLen=ByteUtils::readUInt32(m_headData,1);
			m_bodyData=new char[m_bodyLen];
			m_currentBodyLen=0;
		}else{
			memcpy(m_headData+m_currentHeadLen,data+offset,len-offset);
			m_currentHeadLen=len-offset+m_currentHeadLen;
			return;
		}
	}
	if(m_completeHead){
		if((len-offset)>=(m_bodyLen-m_currentBodyLen)){
			memcpy(m_bodyData+m_currentBodyLen,data+offset,m_bodyLen-m_currentBodyLen);
			offset=offset+m_bodyLen-m_currentBodyLen;
			m_currentBodyLen=m_bodyLen;
			parseComplete();//解析完成事件
			m_completeHead=false;
			m_currentHeadLen=0;
			if(len>offset){
				processData(data,offset,len);
			}
		}else{
			memcpy(m_bodyData+m_currentBodyLen,data+offset,len-offset);
			m_currentBodyLen=len-offset+m_currentBodyLen;
			return;
		}
	}
}
void BufferPacket::parseComplete(){
	GroupPacket* pGroupPacket=WrapPacket::toPacket(m_headData[0],m_bodyData,m_bodyLen);
	delete m_bodyData;
	if(m_pSession!=NULL){
		CoreExecutorService::getInstance()->execute(new OnReceivePacket(m_pSession,pGroupPacket));//线程池执行，防止该调用耗费太长时间而数据接受逻辑阻塞在这里
	}
}
/*SimpleGroupServer*/
SimpleGroupServer::SimpleGroupServer(bool supportTurn):m_supportTurn(supportTurn),m_pServer(NULL),m_pSessions(),
	m_session_lock(),m_pGroupInfos(),m_group_lock(),m_logger("group.SimpleGroupServer"){
}
SimpleGroupServer::~SimpleGroupServer(){
	if(m_pServer!=NULL){
		delete m_pServer;
	}
}
void SimpleGroupServer::start(UInt32 host,UInt32 mainPort,UInt32 judgeNatPort1,UInt32 judgeNatPort2,UInt32 turnPort){
	m_pServer=PT::createServer(m_supportTurn);
	m_pServer->setHandler(this);
	m_pServer->start(host,mainPort,judgeNatPort1,judgeNatPort2,turnPort);
	m_logger.info("PTGroupServer start,version:%s\n",__version);
}
void SimpleGroupServer::shutdown(){
    m_pServer->shutdown();
    m_session_lock.lock();
    map<string ,GroupNodeSession*>::iterator it;
    for(it=m_pSessions.begin();it!=m_pSessions.end();it++){
    	delete it->second;
    }
    m_pSessions.clear();
    m_session_lock.unlock();
    m_group_lock.lock();
    map<string ,GroupInfo*>::iterator iter;
    for(iter=m_pGroupInfos.begin();iter!=m_pGroupInfos.end();iter++){
    	delete iter->second;
    }
    m_pGroupInfos.clear();
    m_group_lock.unlock();
    m_logger.info("PTGroupServer shutdown,version:%s\n",__version);
}
void SimpleGroupServer::waitForEnd(){
	m_pServer->waitForEnd();
}
void SimpleGroupServer::onPeerLogin(const PeerInfo& peerInfo){
	PeerInfo pi=peerInfo;
	m_logger.info("onPeerLogin:");
	pi.printInfo();
	GroupNodeSession* pSession=new GroupNodeSession(this,this,peerInfo);
	addSession(pSession);
}
void SimpleGroupServer::onPeerLogout(const PeerInfo& peerInfo){
	PeerInfo pi=peerInfo;
	m_logger.info("onPeerLogout:");
	pi.printInfo();
	GroupNodeSession* pSession=getSession(peerInfo);
	if(pSession!=NULL&&pSession->m_pGroupInfo!=NULL){
		pSession->m_pGroupInfo->removePeerInfo(peerInfo);
		removeSession(pSession);
	}
}
void SimpleGroupServer::onPeerData(char* data,int len,const PeerInfo& peerInfo){
	GroupNodeSession* pSession=getSession(peerInfo);
	if(pSession!=NULL){
		pSession->processData(data,len);
	}
}
void SimpleGroupServer::onDisconnectToPeer(const PeerInfo& peerInfo){
	PeerInfo pi=peerInfo;
	m_logger.info("onDisconnectToPeer:");
	pi.printInfo();
	GroupNodeSession* pSession=getSession(peerInfo);
	if(pSession!=NULL&&pSession->m_pGroupInfo!=NULL){
		pSession->m_pGroupInfo->removePeerInfo(peerInfo);
		removeSession(pSession);
	}
}
void SimpleGroupServer::sendData(char* data,int length,const PeerInfo& peerInfo){
    m_pServer->sendDataToPeer(data,length,peerInfo);
}
void SimpleGroupServer::receive(GroupNodeSession* pSession,GroupPacket* pPacket){
	m_logger.info("receive group packet:");pPacket->printInfo();
    if(typeid(*pPacket)==typeid(JoinGroupPacket)){
        processJoinGroupPacket(pSession,(JoinGroupPacket*)pPacket);
    }else if(typeid(*pPacket)==typeid(ChunkPacket)){
        processChunkPacket(pSession,(ChunkPacket*)pPacket);
    }else{
    	delete pPacket;
    }
}
void SimpleGroupServer::processJoinGroupPacket(GroupNodeSession* pSession,JoinGroupPacket* pPacket){
    string groupName;
    pPacket->getGroupName(groupName);
    pSession->m_groupName=groupName;
    m_group_lock.lock();
    map<string ,GroupInfo*>::iterator it;
    it=m_pGroupInfos.find(groupName);
    GroupInfo* pGroupInfo;
    if(it==m_pGroupInfos.end()){
    	pGroupInfo=new GroupInfo();
    	m_pGroupInfos.insert(pair<string ,GroupInfo*>(groupName,pGroupInfo));
    }else{
    	pGroupInfo=it->second;
    }
    m_group_lock.unlock();
    pSession->m_pGroupInfo=pGroupInfo;
    list<PeerInfo> peerInfos;
    pGroupInfo->getPeerInfos(peerInfos);
    JoinGroupAckPacket* pJoinGroupAckPacket=new JoinGroupAckPacket(peerInfos);
    pSession->sendPacket(pJoinGroupAckPacket);
    pGroupInfo->addPeerInfo(pSession->m_peerInfo);
    delete pPacket;
}
void SimpleGroupServer::processChunkPacket(GroupNodeSession* pSession,ChunkPacket* pPacket){
    delete pPacket;
}
void SimpleGroupServer::addSession(GroupNodeSession* pSession){
	m_session_lock.lock();
	string key;
	CommonUtils::toIpPortKey(pSession->m_peerInfo.m_wanHost,pSession->m_peerInfo.m_wanPort,key);
	map<string,GroupNodeSession*>::iterator it;
	it=m_pSessions.find(key);
	if(it!=m_pSessions.end()){
		delete it->second;
		m_pSessions.erase(it);
	}
	m_pSessions.insert(pair<string,GroupNodeSession*>(key,pSession));
	m_session_lock.unlock();
}
void SimpleGroupServer::removeSession(GroupNodeSession* pSession){
	m_session_lock.lock();
	string key;
	CommonUtils::toIpPortKey(pSession->m_peerInfo.m_wanHost,pSession->m_peerInfo.m_wanPort,key);
	map<string,GroupNodeSession*>::iterator it;
	it=m_pSessions.find(key);
	if(it!=m_pSessions.end()){
		delete it->second;
		m_pSessions.erase(it);
	}
	m_session_lock.unlock();
}
GroupNodeSession* SimpleGroupServer::getSession(const PeerInfo& peerInfo){
	GroupNodeSession* pSession=NULL;
	m_session_lock.lock();
	string key;
	CommonUtils::toIpPortKey(peerInfo.m_wanHost,peerInfo.m_wanPort,key);
	map<string,GroupNodeSession*>::iterator it;
	it=m_pSessions.find(key);
	if(it!=m_pSessions.end()){
		pSession=it->second;
	}
	m_session_lock.unlock();
	return pSession;
}
void SimpleGroupServer::removeGroupInfo(const string& groupName){
	m_group_lock.lock();
	map<string ,GroupInfo*>::iterator it;
	it=m_pGroupInfos.find(groupName);
	if(it!=m_pGroupInfos.end()){
		delete it->second;
		m_pGroupInfos.erase(it);
	}
	m_group_lock.unlock();
}
/*PTGroupHandler*/
PTGroupHandler::PTGroupHandler(){}
PTGroupHandler::~PTGroupHandler(){}
void PTGroupHandler::onLoginServerSuccess(){}
void PTGroupHandler::onLoginServerFailure(){}
void PTGroupHandler::onDisconnectToServer(){}
void PTGroupHandler::onLoginPeerSuccess(const PeerInfo& peerInfo){}
void PTGroupHandler::onLoginPeerFailure(const PeerInfo& peerInfo){}
void PTGroupHandler::onPeerLogin(const PeerInfo& peerInfo){}
void PTGroupHandler::onPeerLogout(const PeerInfo& peerInfo){}
void PTGroupHandler::onDisconnectToPeer(const PeerInfo& peerInfo){}
void PTGroupHandler::onData(const PeerInfo& peerInfo,char* data,int length){}
void PTGroupHandler::onWantDataFailure(int index){}
void PTGroupHandler::onRequestData(const PeerInfo& peerInfo,int index){}
void PTGroupHandler::onFetchData(const PeerInfo& peerInfo,int index,char* data,int length){}
/*SimpleGroup*/
SimpleGroup::SimpleGroup(const string& groupName,bool supportTurn):
	m_groupName(groupName),m_supportTurn(supportTurn),m_pHandler(NULL),m_pPeer(NULL),m_serverPeerInfo(),m_localPeerInfo(),
	m_pServerSession(NULL),m_pSessions(),m_session_lock(),m_indexs(),m_index_lock(),m_pGroupInfo(),m_group_lock(),m_logger("group.SimpleGroup"){
}
SimpleGroup::~SimpleGroup(){
	if(m_pPeer!=NULL){
		delete m_pPeer;
	}
	if(m_pServerSession!=NULL){
		delete m_pServerSession;
	}
}
void SimpleGroup::setHandler(PTGroupHandler* pHandler){
	m_pHandler=pHandler;
}
void SimpleGroup::start(UInt32 serverHost,UInt32 serverPort){
	m_pGroupInfo=new GroupInfo();
	m_pPeer=PT::createPeer(m_supportTurn);
	m_pPeer->setHandler(this);
	m_pPeer->start();
	m_pPeer->loginServer(serverHost,serverPort);
	m_logger.info("PTGroup start,version:%s\n",__version);
}
void SimpleGroup::start(UInt32 localHost,UInt32 localPort,UInt32 serverHost,UInt32 serverPort){
	m_pPeer=PT::createPeer(m_supportTurn);
	m_pPeer->setHandler(this);
	m_pPeer->start(localHost,localPort);
	m_pPeer->loginServer(serverHost,serverPort);
	m_logger.info("PTGroup start,version:%s\n",__version);
}
void SimpleGroup::shutdown(){
    m_pPeer->logoutAllAndShutdown();
    m_session_lock.lock();
    map<string,GroupNodeSession*>::iterator it;
    for(it=m_pSessions.begin();it!=m_pSessions.end();it++){
    	delete it->second;
    }
    m_pSessions.clear();
    m_session_lock.unlock();
    m_group_lock.lock();
    if(m_pGroupInfo!=NULL){
    	delete m_pGroupInfo;
    }
    m_group_lock.unlock();
    m_logger.info("PTGroup shutdown,version:%s\n",__version);
}
void SimpleGroup::waitForEnd(){
	m_pPeer->waitForEnd();
}
void SimpleGroup::joinGroup(){
    JoinGroupPacket* pJoinGroupPacket=new JoinGroupPacket(m_groupName);
    m_pServerSession->sendPacket(pJoinGroupPacket);
}
void SimpleGroup::sendDataToPeer(char* data,int length,const PeerInfo& peerInfo){
    string key;
    CommonUtils::toIpPortKey(peerInfo.m_wanHost,peerInfo.m_wanPort,key);
    GroupNodeSession* pSession=getSession(peerInfo);
    if(pSession!=NULL){
    	ChunkPacket* pChunkPacket=new ChunkPacket(data,length);
    	pSession->sendPacket(pChunkPacket);
    }
}
void SimpleGroup::sendDataToAll(char* data,int length){
	m_session_lock.lock();
	map<string,GroupNodeSession*>::iterator it;
	for(it=m_pSessions.begin();it!=m_pSessions.end();it++){
		ChunkPacket* pChunkPacket=new ChunkPacket(data,length);
		it->second->sendPacket(pChunkPacket);
	}
	m_session_lock.unlock();
}
void SimpleGroup::addHaveData(int firstIndex,int lastIndex){
	m_group_lock.lock();
	set<int>::iterator it;
	int i;
	for(i=firstIndex;i<=lastIndex;i++){
		m_indexs.insert(i);
	}
	m_group_lock.unlock();
	m_session_lock.lock();
	map<string,GroupNodeSession*>::iterator iter;
	for(iter=m_pSessions.begin();iter!=m_pSessions.end();iter++){
		AddHaveDataPacket* pAddHaveDataPacket=new AddHaveDataPacket(firstIndex,lastIndex);
		iter->second->sendPacket(pAddHaveDataPacket);
	}
	m_session_lock.unlock();
}
void SimpleGroup::removeHaveData(int firstIndex,int lastIndex){
	m_group_lock.lock();
	int i;
	for(i=firstIndex;i<=lastIndex;i++){
	    m_indexs.erase(i);
	}
	m_group_lock.unlock();
	m_session_lock.lock();
	map<string,GroupNodeSession*>::iterator iter;
	for(iter=m_pSessions.begin();iter!=m_pSessions.end();iter++){
		RemoveHaveDataPacket* pRemoveHaveDataPacket=new RemoveHaveDataPacket(firstIndex,lastIndex);
		iter->second->sendPacket(pRemoveHaveDataPacket);
	}
	m_session_lock.unlock();
}
bool SimpleGroup::existData(int index){
	PeerInfo peerInfo;
	if(m_pGroupInfo->whichHaveData(index,peerInfo)){
		return true;
	}
	return false;
}
void SimpleGroup::addWantData(int index){
	PeerInfo peerInfo;
	if(m_pGroupInfo->whichHaveData(index,peerInfo)){
		GroupNodeSession* pSession=getSession(peerInfo);
		if(pSession!=NULL){
			WantDataPacket* pWantDataPacket=new WantDataPacket(index);
			pSession->sendPacket(pWantDataPacket);
			return;
		}
	}
	m_pHandler->onWantDataFailure(index);
}
void SimpleGroup::writeRequestedData(const PeerInfo& peerInfo,int index,char* data,int length){
	GroupNodeSession* pSession=getSession(peerInfo);
	if(pSession!=NULL){
		ShareDataPacket* pShareDataPacket=new ShareDataPacket(index,data,length);
		pSession->sendPacket(pShareDataPacket);
	}
}
void SimpleGroup::getLocalPeerInfo(PeerInfo& peerInfo){
    peerInfo=m_localPeerInfo;
}
int SimpleGroup::getLoginMode(const PeerInfo& peerInfo){
	return m_pPeer->getLoginMode(peerInfo);
}
void SimpleGroup::receive(GroupNodeSession* pSession,GroupPacket* pPacket){
	m_logger.info("receive group packet:");pPacket->printInfo();
    if(typeid(*pPacket)==typeid(JoinGroupAckPacket)){
    	processJoinGroupAckPacket(pSession,(JoinGroupAckPacket*)pPacket);
    }else if(typeid(*pPacket)==typeid(ChunkPacket)){
        processChunkPacket(pSession,(ChunkPacket*)pPacket);
    }else if(typeid(*pPacket)==typeid(AddHaveDataPacket)){
        processAddHaveDataPacket(pSession,(AddHaveDataPacket*)pPacket);
    }else if(typeid(*pPacket)==typeid(RemoveHaveDataPacket)){
        processRemoveHaveDataPacket(pSession,(RemoveHaveDataPacket*)pPacket);
    }else if(typeid(*pPacket)==typeid(NotifySynIndexsPacket)){
        processNotifySynIndexsPacket(pSession,(NotifySynIndexsPacket*)pPacket);
    }else if(typeid(*pPacket)==typeid(SynIndexsPacket)){
        processSynIndexsPacket(pSession,(SynIndexsPacket*)pPacket);
    }else if(typeid(*pPacket)==typeid(WantDataPacket)){
        processWantDataPacket(pSession,(WantDataPacket*)pPacket);
    }else if(typeid(*pPacket)==typeid(WantDataAckPacket)){
        processWantDataAckPacket(pSession,(WantDataAckPacket*)pPacket);
    }else if(typeid(*pPacket)==typeid(NotifyWriteDataPacket)){
        processNotifyWriteDataPacket(pSession,(NotifyWriteDataPacket*)pPacket);
    }else if(typeid(*pPacket)==typeid(ShareDataPacket)){
        processShareDataPacket(pSession,(ShareDataPacket*)pPacket);
    }else{
    	delete pPacket;
    }
}
void SimpleGroup::sendData(char* data,int length,const PeerInfo& peerInfo){
    if(peerInfo.m_wanHost==m_serverPeerInfo.m_wanHost&&peerInfo.m_wanPort==m_serverPeerInfo.m_wanPort){
    	m_pPeer->sendDataToServer(data,length);
    }else{
    	m_pPeer->sendDataToPeer(data,length,peerInfo);
    }
}

void SimpleGroup::onLoginServerSuccess(const PeerInfo& peerInfo){
    m_pServerSession=new GroupNodeSession(this,this,peerInfo);
    m_pPeer->getLocalPeerInfo(m_localPeerInfo);
    m_serverPeerInfo=peerInfo;
    m_pHandler->onLoginServerSuccess();
}
void SimpleGroup::onLoginServerFailure(){
    m_pHandler->onLoginServerFailure();
}
void SimpleGroup::onServerData(char* data,int len){
    m_pServerSession->processData(data,len);
}
void SimpleGroup::onDisconnectToServer(){
    m_pHandler->onDisconnectToServer();
}
void SimpleGroup::onLoginPeerSuccess(const PeerInfo& peerInfo){
    GroupNodeSession* pSession=new GroupNodeSession(this,this,peerInfo);
    addSession(pSession);
    m_pGroupInfo->addPeerInfo(peerInfo);
    NotifySynIndexsPacket* pNotifySynIndexsPacket=new NotifySynIndexsPacket();
    pSession->sendPacket(pNotifySynIndexsPacket);
    m_pHandler->onLoginPeerSuccess(peerInfo);
}
void SimpleGroup::onLoginPeerFailure(const PeerInfo& peerInfo,int loginMode){
    m_pGroupInfo->removePeerInfo(peerInfo);
    m_pHandler->onLoginPeerFailure(peerInfo);
}
void SimpleGroup::onPeerLogin(const PeerInfo& peerInfo){
	GroupNodeSession* pSession=new GroupNodeSession(this,this,peerInfo);
	addSession(pSession);
	m_pGroupInfo->addPeerInfo(peerInfo);
	m_pHandler->onPeerLogin(peerInfo);
}
void SimpleGroup::onPeerLogout(const PeerInfo& peerInfo){
    removeSession(peerInfo);
    m_pGroupInfo->removePeerInfo(peerInfo);
    m_pGroupInfo->removePeerInfoAllIndex(peerInfo);
    m_pHandler->onPeerLogout(peerInfo);
}
void SimpleGroup::onPeerData(char* data,int len,const PeerInfo& peerInfo){
	GroupNodeSession* pSession=getSession(peerInfo);
	if(pSession!=NULL){
		pSession->processData(data,len);
	}
}
void SimpleGroup::onDisconnectToPeer(const PeerInfo& peerInfo){
	removeSession(peerInfo);
	m_pGroupInfo->removePeerInfo(peerInfo);
	m_pGroupInfo->removePeerInfoAllIndex(peerInfo);
	m_pHandler->onDisconnectToPeer(peerInfo);
}
void SimpleGroup::processJoinGroupAckPacket(GroupNodeSession* pSession,JoinGroupAckPacket* pPacket){
	list<PeerInfo> peerInfos;
	pPacket->getPeerInfos(peerInfos);
	list<PeerInfo>::iterator it;
	for(it=peerInfos.begin();it!=peerInfos.end();it++){
		if((*it).m_wanHost!=m_localPeerInfo.m_wanHost||(*it).m_wanPort!=m_localPeerInfo.m_wanPort){
			m_pPeer->loginPeer(*it);
		}
	}
	delete pPacket;
}
void SimpleGroup::processChunkPacket(GroupNodeSession* pSession,ChunkPacket* pPacket){
	char* data=new char[pPacket->getChunkDataLen()];
	pPacket->readChunkData(data);
    m_pHandler->onData(pSession->m_peerInfo,data,pPacket->getChunkDataLen());
    delete pPacket;
    delete data;
}
void SimpleGroup::processAddHaveDataPacket(GroupNodeSession* pSession,AddHaveDataPacket* pPacket){
	m_pGroupInfo->addIndex(pSession->m_peerInfo,pPacket->getFirstIndex(),pPacket->getLastIndex());
	delete pPacket;
}
void SimpleGroup::processRemoveHaveDataPacket(GroupNodeSession* pSession,RemoveHaveDataPacket* pPacket){
    m_pGroupInfo->removeIndex(pSession->m_peerInfo,pPacket->getFirstIndex(),pPacket->getLastIndex());
    delete pPacket;
}
void SimpleGroup::processNotifySynIndexsPacket(GroupNodeSession* pSession,NotifySynIndexsPacket* pPacket){
	m_index_lock.lock();
	SynIndexsPacket* pSynIndexsPacket=new SynIndexsPacket(m_indexs);
	m_index_lock.unlock();
	pSession->sendPacket(pSynIndexsPacket);
	delete pPacket;
}
void SimpleGroup::processSynIndexsPacket(GroupNodeSession* pSession,SynIndexsPacket* pPacket){
	set<int> indexs;
    pPacket->getIndexs(indexs);
    m_index_lock.lock();
    set<int>::iterator it;
    for(it=indexs.begin();it!=indexs.end();it++){
	    m_pGroupInfo->addIndex(pSession->m_peerInfo,*it,*it);
    }
    m_index_lock.unlock();
    delete pPacket;
}
void SimpleGroup::processWantDataPacket(GroupNodeSession* pSession,WantDataPacket* pPacket){
	set<int>::iterator it;
	int result;
	m_index_lock.lock();
	it=m_indexs.find(pPacket->getIndex());
	if(it!=m_indexs.end()){
		result=Have;
	}else{
		result=Not_Have;
	}
	m_index_lock.unlock();
	WantDataAckPacket* pWantDataAckPacket=new WantDataAckPacket(pPacket->getIndex(),result);
	pSession->sendPacket(pWantDataAckPacket);
	delete pPacket;
}
void SimpleGroup::processWantDataAckPacket(GroupNodeSession* pSession,WantDataAckPacket* pPacket){
	if(pPacket->getResult()==Have){
		NotifyWriteDataPacket* pNotifyWriteDataPacket=new NotifyWriteDataPacket(pPacket->getIndex());
		pSession->sendPacket(pNotifyWriteDataPacket);
	}else{
		m_pHandler->onWantDataFailure(pPacket->getIndex());
	}
	delete pPacket;
}
void SimpleGroup::processNotifyWriteDataPacket(GroupNodeSession* pSession,NotifyWriteDataPacket* pPacket){
    m_pHandler->onRequestData(pSession->m_peerInfo,pPacket->getIndex());
    delete pPacket;
}
void SimpleGroup::processShareDataPacket(GroupNodeSession* pSession,ShareDataPacket* pPacket){
	char* data=new char[pPacket->getShareDataLen()];
    pPacket->readShareData(data);
    m_pHandler->onFetchData(pSession->m_peerInfo,pPacket->getIndex(),data,pPacket->getShareDataLen());
    delete pPacket;
    delete data;
}
void SimpleGroup::addSession(GroupNodeSession* pSession){
	m_session_lock.lock();
	string key;
	CommonUtils::toIpPortKey(pSession->m_peerInfo.m_wanHost,pSession->m_peerInfo.m_wanPort,key);
	map<string,GroupNodeSession*>::iterator it;
	it=m_pSessions.find(key);
	if(it!=m_pSessions.end()){
		delete it->second;
		m_pSessions.erase(it);
	}
	m_pSessions.insert(pair<string,GroupNodeSession*>(key,pSession));
	m_session_lock.unlock();
}
void SimpleGroup::removeSession(GroupNodeSession* pSession){
	m_session_lock.lock();
	string key;
	CommonUtils::toIpPortKey(pSession->m_peerInfo.m_wanHost,pSession->m_peerInfo.m_wanPort,key);
	map<string,GroupNodeSession*>::iterator it;
	it=m_pSessions.find(key);
	if(it!=m_pSessions.end()){
		delete it->second;
		m_pSessions.erase(it);
	}
	m_session_lock.unlock();
}
void SimpleGroup::removeSession(const PeerInfo& peerInfo){
	m_session_lock.lock();
	string key;
	CommonUtils::toIpPortKey(peerInfo.m_wanHost,peerInfo.m_wanPort,key);
	map<string,GroupNodeSession*>::iterator it;
	it=m_pSessions.find(key);
	if(it!=m_pSessions.end()){
		delete it->second;
		m_pSessions.erase(it);
	}
	m_session_lock.unlock();
}
GroupNodeSession* SimpleGroup::getSession(const PeerInfo& peerInfo){
	GroupNodeSession* pSession=NULL;
	m_session_lock.lock();
	string key;
	CommonUtils::toIpPortKey(peerInfo.m_wanHost,peerInfo.m_wanPort,key);
	map<string,GroupNodeSession*>::iterator it;
	it=m_pSessions.find(key);
	if(it!=m_pSessions.end()){
		pSession=it->second;
	}
	m_session_lock.unlock();
	return pSession;
}
/*PT*/
PTGroupServer* PT::createGroupServer(bool supportTurn){
	return new SimpleGroupServer(supportTurn);
}
void PT::destroyGroupServer(PTGroupServer* pGroupServer){
	delete pGroupServer;
}
PTGroup* PT::createGroup(const string& groupName,bool supportTurn){
	return new SimpleGroup(groupName,supportTurn);
}
void PT::destroyGroup(PTGroup* pGroup){
	delete pGroup;
}
