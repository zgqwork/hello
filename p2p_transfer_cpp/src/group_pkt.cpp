#include <typeinfo>
#include "group_pkt.h"
/*WrapPacket*/
const int WrapPacket::headSize=5;
GroupPacket* WrapPacket::toPacket(int type,char* data,int length){
	GroupPacket* pGroupPacket=NULL;
	if(type==JOIN_GROUP){
		pGroupPacket=new JoinGroupPacket();
	}else if(type==JOIN_GROUP_ACK){
		pGroupPacket=new JoinGroupAckPacket();
	}else if(type==CHUNK){
		pGroupPacket=new ChunkPacket();
	}else if(type==ADD_HAVE_DATA){
		pGroupPacket=new AddHaveDataPacket();
	}else if(type==REMOVE_HAVE_DATA){
		pGroupPacket=new RemoveHaveDataPacket();
	}else if(type==NOTIFY_SYN_INDEXS){
		pGroupPacket=new NotifySynIndexsPacket();
	}else if(type==SYN_INDEXS){
		pGroupPacket=new SynIndexsPacket();
	}else if(type==WANT_DATA){
		pGroupPacket=new WantDataPacket();
	}else if(type==WANT_DATA_ACK){
		pGroupPacket=new WantDataAckPacket();
	}else if(type==NOTIFY_WRITE_DATA){
		pGroupPacket=new NotifyWriteDataPacket();
	}else if(type==SHARE_DATA){
		pGroupPacket=new ShareDataPacket();
	}
	pGroupPacket->writeBytes(data,length);
	return pGroupPacket;
}
void WrapPacket::toBytes(GroupPacket* pGroupPacket,char* dest){
	int type=-1;
	int length;
	if(typeid(*pGroupPacket)==typeid(JoinGroupPacket)){
		type=JOIN_GROUP;
	}else if(typeid(*pGroupPacket)==typeid(JoinGroupAckPacket)){
		type=JOIN_GROUP_ACK;
	}else if(typeid(*pGroupPacket)==typeid(ChunkPacket)){
		type=CHUNK;
	}else if(typeid(*pGroupPacket)==typeid(AddHaveDataPacket)){
		type=ADD_HAVE_DATA;
	}else if(typeid(*pGroupPacket)==typeid(RemoveHaveDataPacket)){
		type=REMOVE_HAVE_DATA;
	}else if(typeid(*pGroupPacket)==typeid(NotifySynIndexsPacket)){
		type=NOTIFY_SYN_INDEXS;
	}else if(typeid(*pGroupPacket)==typeid(SynIndexsPacket)){
		type=SYN_INDEXS;
	}else if(typeid(*pGroupPacket)==typeid(WantDataPacket)){
		type=WANT_DATA;
	}else if(typeid(*pGroupPacket)==typeid(WantDataAckPacket)){
		type=WANT_DATA_ACK;
	}else if(typeid(*pGroupPacket)==typeid(NotifyWriteDataPacket)){
		type=NOTIFY_WRITE_DATA;
	}else if(typeid(*pGroupPacket)==typeid(ShareDataPacket)){
		type=SHARE_DATA;
	}
	dest[0]=type;
	length=pGroupPacket->getBytesLen();
	ByteUtils::writeUInt32(dest,1,length);
	pGroupPacket->readBytes(dest+5);
}
/*JoinGroupPacket*/
JoinGroupPacket::JoinGroupPacket():m_groupName(),m_logger("group.pkt.JoinGroupPacket"){
}
JoinGroupPacket::JoinGroupPacket(const string& groupName):m_groupName(groupName),m_logger("group.pkt.JoinGroupPacket"){
}
JoinGroupPacket::~JoinGroupPacket(){
}
int JoinGroupPacket::getBytesLen(){
	return m_groupName.size();
}
void JoinGroupPacket::readBytes(char* dest){
	const char* temp=m_groupName.c_str();
	memcpy(dest,temp,m_groupName.size());
}
void JoinGroupPacket::writeBytes(char* src,int len){
	char* temp=new char[len+1];
    memcpy(temp,src,len);
    temp[len]='\0';
    m_groupName=string(temp);
	delete temp;
}
void JoinGroupPacket::printInfo(){
	m_logger.info("JoinGroupPacket{groupName:%s}\n",m_groupName.c_str());
}
/*JoinGroupAckPacket*/
JoinGroupAckPacket::JoinGroupAckPacket():m_peerInfos(),m_logger("group.pkt.JoinGroupAckPacket"){
}
JoinGroupAckPacket::JoinGroupAckPacket(const list<PeerInfo>& peerInfos):m_peerInfos(peerInfos),m_logger("group.pkt.JoinGroupAckPacket"){
}
JoinGroupAckPacket::~JoinGroupAckPacket(){
}
int JoinGroupAckPacket::getBytesLen(){
	return 4+m_peerInfos.size()*16;
}
void JoinGroupAckPacket::readBytes(char* dest){
	ByteUtils::writeUInt32(dest,0,m_peerInfos.size());
	list<PeerInfo>::iterator it;
	int i=0;
	for(it=m_peerInfos.begin();it!=m_peerInfos.end();it++){
		ByteUtils::writeUInt32(dest,i*16+4,(*it).m_wanHost);
		ByteUtils::writeUInt32(dest,i*16+8,(*it).m_wanPort);
		ByteUtils::writeUInt32(dest,i*16+12,(*it).m_localHost);
		ByteUtils::writeUInt32(dest,i*16+16,(*it).m_localPort);
		i++;
	}
}
void JoinGroupAckPacket::writeBytes(char* src,int len){
	int count=ByteUtils::readUInt32(src,0);
	int i;
	PeerInfo peerInfo;
	for(i=0;i<count;i++){
		peerInfo.m_wanHost=ByteUtils::readUInt32(src,i*16+4);
		peerInfo.m_wanPort=ByteUtils::readUInt32(src,i*16+8);
		peerInfo.m_localHost=ByteUtils::readUInt32(src,i*16+12);
		peerInfo.m_localPort=ByteUtils::readUInt32(src,i*16+16);
		m_peerInfos.push_back(peerInfo);
	}
}
void JoinGroupAckPacket::printInfo(){
	list<PeerInfo>::iterator it;
	m_logger.info("JoinGroupAckPacket{");
	for(it=m_peerInfos.begin();it!=m_peerInfos.end();it++){
		string wanIp,localIp;
		CommonUtils::toIpStr((*it).m_wanHost,wanIp);
		CommonUtils::toIpStr((*it).m_localHost,localIp);
		m_logger.info("{wanHost:%s,wanPort:%d,localHost:%s,localPort:%d}",wanIp.c_str(),(*it).m_wanPort,localIp.c_str(),(*it).m_localPort);
	}
	m_logger.info("}\n");
}
/*ChunkPacket*/
ChunkPacket::ChunkPacket():m_chunkData(NULL),m_chunkDataLen(0),m_logger("group.pkt.ChunkPacket"){
}
ChunkPacket::ChunkPacket(char* chunkData,int chunkDataLen):m_chunkData(NULL),m_chunkDataLen(chunkDataLen),m_logger("group.pkt.ChunkPacket"){
	m_chunkData=new char[chunkDataLen];
	memcpy(m_chunkData,chunkData,chunkDataLen);
}
ChunkPacket::~ChunkPacket(){
	delete m_chunkData;
}
int ChunkPacket::getBytesLen(){
	return m_chunkDataLen;
}
void ChunkPacket::readBytes(char* dest){
	memcpy(dest,m_chunkData,m_chunkDataLen);
}
void ChunkPacket::writeBytes(char* src,int len){
	m_chunkDataLen=len;
	m_chunkData=new char[len];
	memcpy(m_chunkData,src,len);
}
void ChunkPacket::printInfo(){
	m_logger.info("ChunkPacket{chunkDataLen:%d}\n",m_chunkDataLen);
}
/*AddHaveDataPacket*/
AddHaveDataPacket::AddHaveDataPacket():m_firstIndex(0),m_lastIndex(0),m_logger("group.pkt.AddHaveDataPacket"){
}
AddHaveDataPacket::AddHaveDataPacket(int firstIndex,int lastIndex):m_firstIndex(firstIndex),m_lastIndex(lastIndex),m_logger("group.pkt.AddHaveDataPacket"){
}
AddHaveDataPacket::~AddHaveDataPacket(){
}
int AddHaveDataPacket::getBytesLen(){
	return 8;
}
void AddHaveDataPacket::readBytes(char* dest){
	ByteUtils::writeUInt32(dest,0,m_firstIndex);
	ByteUtils::writeUInt32(dest,4,m_lastIndex);
}
void AddHaveDataPacket::writeBytes(char* src,int len){
	m_firstIndex=ByteUtils::readUInt32(src,0);
	m_lastIndex=ByteUtils::readUInt32(src,4);
}
void AddHaveDataPacket::printInfo(){
	m_logger.info("AddHaveDataPacket{firstIndex:%d,lastIndex:%d}\n",m_firstIndex,m_lastIndex);
}
/*AddHaveDataPacket*/
RemoveHaveDataPacket::RemoveHaveDataPacket():m_firstIndex(0),m_lastIndex(0),m_logger("group.pkt.RemoveHaveDataPacket"){
}
RemoveHaveDataPacket::RemoveHaveDataPacket(int firstIndex,int lastIndex):m_firstIndex(firstIndex),m_lastIndex(lastIndex),m_logger("group.pkt.RemoveHaveDataPacket"){
}
RemoveHaveDataPacket::~RemoveHaveDataPacket(){
}
int RemoveHaveDataPacket::getBytesLen(){
	return 8;
}
void RemoveHaveDataPacket::readBytes(char* dest){
	ByteUtils::writeUInt32(dest,0,m_firstIndex);
	ByteUtils::writeUInt32(dest,4,m_lastIndex);
}
void RemoveHaveDataPacket::writeBytes(char* src,int len){
	m_firstIndex=ByteUtils::readUInt32(src,0);
	m_lastIndex=ByteUtils::readUInt32(src,4);
}
void RemoveHaveDataPacket::printInfo(){
	m_logger.info("RemoveHaveDataPacket{firstIndex:%d,lastIndex:%d}\n",m_firstIndex,m_lastIndex);
}
/*NotifySynIndexsPacket*/
NotifySynIndexsPacket::NotifySynIndexsPacket():m_logger("group,pkt.NotifySynIndexsPacket"){
}
NotifySynIndexsPacket::~NotifySynIndexsPacket(){
}
int NotifySynIndexsPacket::getBytesLen(){
	return 0;
}
void NotifySynIndexsPacket::readBytes(char* dest){
}
void NotifySynIndexsPacket::writeBytes(char* src,int len){
}
void NotifySynIndexsPacket::printInfo(){
	m_logger.info("NotifySynIndexsPacket\n");
}
/*SynIndexsPacket*/
SynIndexsPacket::SynIndexsPacket():m_dispersedIndexs(),m_tupleIndexs(),m_indexs(),m_logger("group.pkt.SynIndexsPacket"){
}
SynIndexsPacket::SynIndexsPacket(set<int> indexs):m_dispersedIndexs(),m_tupleIndexs(),m_indexs(indexs),m_logger("group.pkt.SynIndexsPacket"){
	splitIndexs();
}
SynIndexsPacket::~SynIndexsPacket(){
}
int SynIndexsPacket::getBytesLen(){
	return 8+m_dispersedIndexs.size()*4+m_tupleIndexs.size()*8;
}
void SynIndexsPacket::readBytes(char* dest){
	ByteUtils::writeUInt32(dest,0,m_dispersedIndexs.size());
	ByteUtils::writeUInt32(dest,4,m_tupleIndexs.size());
	int i=0;
	list<int>::iterator it;
	for(it=m_dispersedIndexs.begin();it!=m_dispersedIndexs.end();it++){
		ByteUtils::writeUInt32(dest,8+i*4,*it);
		i++;
	}
	i=0;
	list<UInt64>::iterator ite;
	for(ite=m_tupleIndexs.begin();ite!=m_tupleIndexs.end();ite++){
		ByteUtils::writeUInt64(dest,8+m_dispersedIndexs.size()*4+i*8,*ite);
		i++;
	}
}
void SynIndexsPacket::writeBytes(char* src,int len){
	int dispersedCount=ByteUtils::readUInt32(src,0);
	int tupleCount=ByteUtils::readUInt32(src,4);
	for(int i=0;i<dispersedCount;i++){
		m_indexs.insert(ByteUtils::readUInt32(src,8+i*4));
	}
	int firstIndex,lastIndex;
	for(int j=0;j<tupleCount;j++){
		firstIndex=ByteUtils::readUInt32(src,8+dispersedCount*4+j*8);
		lastIndex=ByteUtils::readUInt32(src,8+dispersedCount*4+j*8+4);
		for(int k=firstIndex;k<=lastIndex;k++){
			m_indexs.insert(k);
		}
	}
}
void SynIndexsPacket::printInfo(){
	set<int>::iterator it;
	m_logger.info("SynIndexsPacket{");
	for(it=m_indexs.begin();it!=m_indexs.end();it++){
		m_logger.info("%d,",*it);
	}
	m_logger.info("}\n");

	/*
	m_logger.info("dispersedIndexs{");
	list<int>::iterator it1;
	for(it1=m_dispersedIndexs.begin();it1!=m_dispersedIndexs.end();it1++){
		m_logger.info("%d,",*it1);
	}
	m_logger.info("}tupleIndexs{");
	list<UInt64>::iterator it2;
	UInt32 v1, v2;
	for(it2=m_tupleIndexs.begin();it2!=m_tupleIndexs.begin();it2++){
		uInt64ToTuple(*it2,v1,v2);
		m_logger.info("%d-%d,",v1,v2);
	}
	m_logger.info("}\n");
	*/
}
void SynIndexsPacket::splitIndexs(){
	bool begin=false;
	int lastIndex=0;
	int index;
	int tuple[2]={0};
	bool isTuple=false;
	set<int>::iterator it;
	int size=m_indexs.size();
	int i=0;
	for(it=m_indexs.begin();it!=m_indexs.end();it++){
		i++;
		index=*it;
		if(begin){
			if(lastIndex+1==index){
				if(isTuple){
					tuple[1]=index;
				}else{
					tuple[0]=lastIndex;
					tuple[1]=index;
					isTuple=true;
				}
			}else{
				if(isTuple){
					m_tupleIndexs.push_back(tupleToUInt64(tuple[0],tuple[1]));
					isTuple=false;
				}else{
					m_dispersedIndexs.push_back(lastIndex);
				}
			}
		}
		if(i==size){
			if(isTuple){
				m_tupleIndexs.push_back(tupleToUInt64(tuple[0],tuple[1]));
			}else{
				m_dispersedIndexs.push_back(index);
			}
			break;
		}
		lastIndex=index;
		if(!begin){
			begin=true;
		}
	}
}
UInt64 SynIndexsPacket::tupleToUInt64(UInt32 v1,UInt32 v2){
	char temp[8];
	ByteUtils::writeUInt32(temp,0,v1);
	ByteUtils::writeUInt32(temp,4,v2);
	return ByteUtils::readUInt64(temp,0);
}
void SynIndexsPacket::uInt64ToTuple(UInt64 v,UInt32& v1,UInt32& v2){
	char temp[8];
	ByteUtils::writeUInt64(temp,0,v);
	v1=ByteUtils::readUInt32(temp,0);
	v2=ByteUtils::readUInt32(temp,4);
}
/*WantDataPacket*/
WantDataPacket::WantDataPacket():m_index(),m_logger("group.pkt.WantDataPacket"){
}
WantDataPacket::WantDataPacket(int index):m_index(index),m_logger("group.pkt.WantDataPacket"){
}
WantDataPacket::~WantDataPacket(){
}
int WantDataPacket::getBytesLen(){
	return 4;
}
void WantDataPacket::readBytes(char* dest){
	ByteUtils::writeUInt32(dest,0,m_index);
}
void WantDataPacket::writeBytes(char* src,int len){
	m_index=ByteUtils::readUInt32(src,0);
}
void WantDataPacket::printInfo(){
	m_logger.info("WantDataPacket{index:%d}\n",m_index);
}
/*WantDataAckPacket*/
WantDataAckPacket::WantDataAckPacket():m_index(0),m_result(0),m_logger("group.pkt.WantDataAckPacket"){
}
WantDataAckPacket::WantDataAckPacket(int index,int result):m_index(index),m_result(result),m_logger("group.pkt.WantDataAckPacket"){
}
WantDataAckPacket::~WantDataAckPacket(){
}
int WantDataAckPacket::getBytesLen(){
	return 8;
}
void WantDataAckPacket::readBytes(char* dest){
	ByteUtils::writeUInt32(dest,0,m_index);
	ByteUtils::writeUInt32(dest,4,m_result);
}
void WantDataAckPacket::writeBytes(char* src,int len){
	m_index=ByteUtils::readUInt32(src,0);
	m_result=ByteUtils::readUInt32(src,4);
}
void WantDataAckPacket::printInfo(){
	m_logger.info("WantDataAckPacket{index:%d,result:%d}\n",m_index,m_result);
}
/*NotifyWriteDataPacket*/
NotifyWriteDataPacket::NotifyWriteDataPacket():m_index(),m_logger("group.pkt.NotifyWriteDataPacket"){
}
NotifyWriteDataPacket::NotifyWriteDataPacket(int index):m_index(index),m_logger("group.pkt.NotifyWriteDataPacket"){
}
NotifyWriteDataPacket::~NotifyWriteDataPacket(){
}
int NotifyWriteDataPacket::getBytesLen(){
	return 4;
}
void NotifyWriteDataPacket::readBytes(char* dest){
	ByteUtils::writeUInt32(dest,0,m_index);
}
void NotifyWriteDataPacket::writeBytes(char* src,int len){
	m_index=ByteUtils::readUInt32(src,0);
}
void NotifyWriteDataPacket::printInfo(){
	m_logger.info("NotifyWriteDataPacket{index:%d}\n",m_index);
}
/*ShareDataPacket*/
ShareDataPacket::ShareDataPacket():m_index(0),m_data(NULL),m_dataLen(0),m_logger("group.pkt.ShareDataPacket"){
}
ShareDataPacket::ShareDataPacket(int index,char* data,int dataLen):m_index(0),m_data(NULL),m_dataLen(dataLen),m_logger("group.pkt.ShareDataPacket"){
	m_data=new char[dataLen];
	memcpy(m_data,data,dataLen);
}
ShareDataPacket::~ShareDataPacket(){
    delete m_data;
}
int ShareDataPacket::getBytesLen(){
    return m_dataLen+4;
}
void ShareDataPacket::readBytes(char* dest){
    ByteUtils::writeUInt32(dest,0,m_index);
    memcpy(dest+4,m_data,m_dataLen);
}
void ShareDataPacket::writeBytes(char* src,int len){
	m_dataLen=len;
	m_data=new char[len];
	memcpy(m_data,src+4,len);
}
void ShareDataPacket::printInfo(){
	m_logger.info("ShareDataPacket{index:%d,dataLen:%d}\n",m_index,m_dataLen);
}
