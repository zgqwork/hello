#include <typeinfo>

#include "packet.h"

/*Packet实现*/
Packet::Packet(UInt32 host,UInt32 port):m_host(host),m_port(port),m_isTurn(false),m_isControl(false),
    m_isTurnHeart(false),m_controlPacketType(-1),m_logger("packet.Packet"){
}
Packet::~Packet(){
}
void Packet::writeBytes(char* src,int len){
	m_isTurn=(src[0]&128)!=0;
	m_isControl=(src[0]&64)!=0;
	m_isTurnHeart=(src[0]&32)!=0;
	if(!m_isTurn&&m_isControl){
		m_controlPacketType=src[1]&0xFF;
	}else if(m_isTurn&&m_isControl&&!m_isTurnHeart){
		m_controlPacketType=src[9]&0xFF;
	}
}
void Packet::printInfo(){
	//m_logger.info("Packet{isTurn=%d,isControl=%d,isTurnHeart=%d}\n",m_isTurn,m_isControl,m_isTurnHeart);
}
/*DataPacket实现*/
const int DataPacket::dataPacketHeadSize=5;
DataPacket::DataPacket(UInt32 host,UInt32 port):Packet(host,port),m_packetSeqNo(0),m_wrapData(NULL),m_wrapDataLen(0){
}
DataPacket::~DataPacket(){
	delete m_wrapData;
}
int DataPacket::getBytesLen(){
	return m_wrapDataLen+dataPacketHeadSize;
}
void DataPacket::readBytes(char* dest){
	dest[0]=0;
    ByteUtils::writeUInt32(dest,1,m_packetSeqNo);
    readWrapData(dest,5);
}
void DataPacket::writeBytes(char* src,int len){
    Packet::writeBytes(src,len);
    m_packetSeqNo=ByteUtils::readUInt32(src,1);
    writeWrapData(src,5,len-5);
}
void DataPacket::printInfo(){
	Packet::printInfo();
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("DataPacket{host=%s,port=%d,packetSeqNo=%d,wrapDataLen=%d}\n",ipStr.c_str(),m_port,m_packetSeqNo,m_wrapDataLen);
}
/*LoginServerPacket实现*/
const int LoginServerPacket::loginServerPacketSize=10;
LoginServerPacket::LoginServerPacket(UInt32 host,UInt32 port):Packet(host,port),m_localHost(0),m_localPort(0){
    m_isControl=true;
    m_controlPacketType=LOGIN_SERVER;
}
LoginServerPacket::~LoginServerPacket(){
}
int LoginServerPacket::getBytesLen(){
	return loginServerPacketSize;
}
void LoginServerPacket::readBytes(char* dest){
    dest[0]=64;//01000000
    dest[1]=m_controlPacketType;
    ByteUtils::writeUInt32(dest,2,m_localHost);
    ByteUtils::writeUInt32(dest,6,m_localPort);
}
void LoginServerPacket::writeBytes(char* src,int len){
    Packet::writeBytes(src,len);
    m_localHost=ByteUtils::readUInt32(src,2);
    m_localPort=ByteUtils::readUInt32(src,6);
}
void LoginServerPacket::printInfo(){
	Packet::printInfo();
	string ipStr,localIpStr;
	CommonUtils::toIpStr(m_host,ipStr);
	CommonUtils::toIpStr(m_localHost,localIpStr);
	m_logger.info("LoginServerPacket{host=%s,port=%d,localHost=%s,localPort=%d}\n",ipStr.c_str(),m_port,localIpStr.c_str(),m_localPort);
}
/*LoginServerAckPacket实现*/
const int LoginServerAckPacket::loginServerAckPacketSize=22;
LoginServerAckPacket::LoginServerAckPacket(UInt32 host,UInt32 port):Packet(host,port),m_serverJudgeNatPort1(0),m_serverJudgeNatPort2(0),m_serverTurnPort(0),m_wanHost(0),m_wanPort(0){
	m_isControl=true;
	m_controlPacketType=LOGIN_SERVER_ACK;
}
LoginServerAckPacket::~LoginServerAckPacket(){
}
int LoginServerAckPacket::getBytesLen(){
	return loginServerAckPacketSize;
}
void LoginServerAckPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
	ByteUtils::writeUInt32(dest,2,m_serverJudgeNatPort1);
	ByteUtils::writeUInt32(dest,6,m_serverJudgeNatPort2);
	ByteUtils::writeUInt32(dest,10,m_serverTurnPort);
	ByteUtils::writeUInt32(dest,14,m_wanHost);
	ByteUtils::writeUInt32(dest,18,m_wanPort);
}
void LoginServerAckPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_serverJudgeNatPort1=ByteUtils::readUInt32(src,2);
	m_serverJudgeNatPort2=ByteUtils::readUInt32(src,6);
	m_serverTurnPort=ByteUtils::readUInt32(src,10);
	m_wanHost=ByteUtils::readUInt32(src,14);
	m_wanPort=ByteUtils::readUInt32(src,18);
}
void LoginServerAckPacket::printInfo(){
	Packet::printInfo();
	string ipStr,wanIpStr;
	CommonUtils::toIpStr(m_host,ipStr);
	CommonUtils::toIpStr(m_wanHost,wanIpStr);
	m_logger.info("LoginServerAckPacket{host=%s,port=%d,judgeNatPort1=%d,judgeNatPort2=%d,turnPort=%d,wanHost=%s,wanPort=%d}\n",
	ipStr.c_str(),m_port,m_serverJudgeNatPort1,m_serverJudgeNatPort2,m_serverTurnPort,wanIpStr.c_str(),m_wanPort);
}
/*LoginPeerPacket实现*/
const int LoginPeerPacket::loginPeerPacketSize=19;
LoginPeerPacket::LoginPeerPacket(UInt32 host,UInt32 port):Packet(host,port),m_wanHost(0),m_wanPort(0),m_localHost(0),m_localPort(0),m_loginMode(0){
	m_isControl=true;
	m_controlPacketType=LOGIN_PEER;
}
LoginPeerPacket::~LoginPeerPacket(){
}
int LoginPeerPacket::getBytesLen(){
	return loginPeerPacketSize;
}
void LoginPeerPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
	ByteUtils::writeUInt32(dest,2,m_wanHost);
	ByteUtils::writeUInt32(dest,6,m_wanPort);
	ByteUtils::writeUInt32(dest,10,m_localHost);
	ByteUtils::writeUInt32(dest,14,m_localPort);
	dest[18]=m_loginMode;
}
void LoginPeerPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_wanHost=ByteUtils::readUInt32(src,2);
	m_wanPort=ByteUtils::readUInt32(src,6);
	m_localHost=ByteUtils::readUInt32(src,10);
	m_localPort=ByteUtils::readUInt32(src,14);
	m_loginMode=(UInt8)src[18];
}
void LoginPeerPacket::printInfo(){
	Packet::printInfo();
	string ipStr,wanIpStr,localIpStr;
	CommonUtils::toIpStr(m_host,ipStr);
	CommonUtils::toIpStr(m_wanHost,wanIpStr);
	CommonUtils::toIpStr(m_localHost,localIpStr);
	m_logger.info("LoginPeerPacket{host=%s,port=%d,wanHost=%s,wanPort=%d,localHost=%s,localPort=%d,loginMode=%d}\n",
	ipStr.c_str(),m_port,wanIpStr.c_str(),m_wanPort,localIpStr.c_str(),m_localPort,m_loginMode);
}
/*LoginPeerAckPacket实现*/
const int LoginPeerAckPacket::loginPeerAckPacketSize=19;
LoginPeerAckPacket::LoginPeerAckPacket(UInt32 host,UInt32 port):Packet(host,port),m_wanHost(0),m_wanPort(0),m_localHost(0),m_localPort(0),m_loginMode(0){
	m_isControl=true;
	m_controlPacketType=LOGIN_PEER_ACK;
}
LoginPeerAckPacket::~LoginPeerAckPacket(){
}
int LoginPeerAckPacket::getBytesLen(){
	return loginPeerAckPacketSize;
}
void LoginPeerAckPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
	ByteUtils::writeUInt32(dest,2,m_wanHost);
	ByteUtils::writeUInt32(dest,6,m_wanPort);
	ByteUtils::writeUInt32(dest,10,m_localHost);
	ByteUtils::writeUInt32(dest,14,m_localPort);
	dest[18]=m_loginMode;
}
void LoginPeerAckPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_wanHost=ByteUtils::readUInt32(src,2);
	m_wanPort=ByteUtils::readUInt32(src,6);
	m_localHost=ByteUtils::readUInt32(src,10);
	m_localPort=ByteUtils::readUInt32(src,14);
	m_loginMode=(UInt8)src[18];
}
void LoginPeerAckPacket::printInfo(){
	Packet::printInfo();
	string ipStr,wanIpStr,localIpStr;
	CommonUtils::toIpStr(m_host,ipStr);
	CommonUtils::toIpStr(m_wanHost,wanIpStr);
	CommonUtils::toIpStr(m_localHost,localIpStr);
	m_logger.info("LoginPeerAckPacket{host=%s,port=%d,wanHost=%s,wanPort=%d,localHost=%s,localPort=%d,loginMode=%d}\n",
	ipStr.c_str(),m_port,wanIpStr.c_str(),m_wanPort,localIpStr.c_str(),m_localPort,m_loginMode);
}
/*HeartPacket实现*/
const int HeartPacket::heartPacketSize=2;
HeartPacket::HeartPacket(UInt32 host,UInt32 port):Packet(host,port){
	m_isControl=true;
	m_controlPacketType=HEART;
}
HeartPacket::~HeartPacket(){
}
int HeartPacket::getBytesLen(){
	return heartPacketSize;
}
void HeartPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
}
void HeartPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
}
void HeartPacket::printInfo(){
	Packet::printInfo();
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("HeartPacket{host=%s,port=%d}\n",ipStr.c_str(),m_port);
}
/*HeartAckPacket实现*/
const int HeartAckPacket::heartAckPacketSize=2;
HeartAckPacket::HeartAckPacket(UInt32 host,UInt32 port):Packet(host,port){
	m_isControl=true;
	m_controlPacketType=HEART_ACK;
}
HeartAckPacket::~HeartAckPacket(){
}
int HeartAckPacket::getBytesLen(){
	return heartAckPacketSize;
}
void HeartAckPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
}
void HeartAckPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
}
void HeartAckPacket::printInfo(){
	Packet::printInfo();
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("HeartAckPacket{host=%s,port=%d}\n",ipStr.c_str(),m_port);
}
/*AckPacket实现*/
const int AckPacket::ackPacketSize=6;
AckPacket::AckPacket(UInt32 host,UInt32 port):Packet(host,port),m_packetSeqNo(0){
	m_isControl=true;
	m_controlPacketType=ACK;
}
AckPacket::~AckPacket(){
}
int AckPacket::getBytesLen(){
	return ackPacketSize;
}
void AckPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
	ByteUtils::writeUInt32(dest,2,m_packetSeqNo);
}
void AckPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_packetSeqNo=ByteUtils::readUInt32(src,2);
}
void AckPacket::printInfo(){
	Packet::printInfo();
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("AckPacket{host=%s,port=%d,packetSeqNo=%d}\n",ipStr.c_str(),m_port,m_packetSeqNo);
}
/*JudgeNatPacket实现*/
const int JudgeNatPacket::judgeNatPacketSize=10;
JudgeNatPacket::JudgeNatPacket(UInt32 host,UInt32 port):Packet(host,port),m_wanHost(0),m_wanPort(0){
	m_isControl=true;
	m_controlPacketType=JUDGE_NAT;
}
JudgeNatPacket::~JudgeNatPacket(){
}
int JudgeNatPacket::getBytesLen(){
	return judgeNatPacketSize;
}
void JudgeNatPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
	ByteUtils::writeUInt32(dest,2,m_wanHost);
	ByteUtils::writeUInt32(dest,6,m_wanPort);
}
void JudgeNatPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_wanHost=ByteUtils::readUInt32(src,2);
	m_wanPort=ByteUtils::readUInt32(src,6);
}
void JudgeNatPacket::printInfo(){
	Packet::printInfo();
	string ipStr,wanIpStr;
	CommonUtils::toIpStr(m_host,ipStr);
	CommonUtils::toIpStr(m_wanHost,wanIpStr);
	m_logger.info("JudgeNatPacket{host=%s,port=%d,m_wanHost=%s,m_wanPort=%d}\n",ipStr.c_str(),m_port,wanIpStr.c_str(),m_wanPort);
}
/*JudgeNatPacket实现*/
const int JudgeNatAckPacket::judgeNatAckPacketSize=10;
JudgeNatAckPacket::JudgeNatAckPacket(UInt32 host,UInt32 port):Packet(host,port),m_wanHost(0),m_wanPort(0){
	m_isControl=true;
	m_controlPacketType=JUDGE_NAT_ACK;
}
JudgeNatAckPacket::~JudgeNatAckPacket(){
}
int JudgeNatAckPacket::getBytesLen(){
	return judgeNatAckPacketSize;
}
void JudgeNatAckPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
	ByteUtils::writeUInt32(dest,2,m_wanHost);
	ByteUtils::writeUInt32(dest,6,m_wanPort);
}
void JudgeNatAckPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_wanHost=ByteUtils::readUInt32(src,2);
	m_wanPort=ByteUtils::readUInt32(src,6);
}
void JudgeNatAckPacket::printInfo(){
	Packet::printInfo();
	string ipStr,wanIpStr;
	CommonUtils::toIpStr(m_host,ipStr);
	CommonUtils::toIpStr(m_wanHost,wanIpStr);
	m_logger.info("JudgeNatAckPacket{host=%s,port=%d,m_wanHost=%s,m_wanPort=%d}\n",ipStr.c_str(),m_port,wanIpStr.c_str(),m_wanPort);
}
/*ReportPacket实现*/
const int ReportPacket::reportPacketSize=3;
ReportPacket::ReportPacket(UInt32 host,UInt32 port):Packet(host,port),m_natType(Symmetric_NAT){
	m_isControl=true;
	m_controlPacketType=REPORT;
}
ReportPacket::~ReportPacket(){
}
int ReportPacket::getBytesLen(){
	return reportPacketSize;
}
void ReportPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
	dest[2]=m_natType;
}
void ReportPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_natType=src[2];
}
void ReportPacket::printInfo(){
	Packet::printInfo();
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("ReportPacket{host=%s,port=%d,natType=%d}\n",ipStr.c_str(),m_port,m_natType);
}
/*ReportAckPacket实现*/
const int ReportAckPacket::reportAckPacketSize=2;
ReportAckPacket::ReportAckPacket(UInt32 host,UInt32 port):Packet(host,port){
	m_isControl=true;
	m_controlPacketType=REPORT_ACK;
}
ReportAckPacket::~ReportAckPacket(){
}
int ReportAckPacket::getBytesLen(){
	return reportAckPacketSize;
}
void ReportAckPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
}
void ReportAckPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
}
void ReportAckPacket::printInfo(){
	Packet::printInfo();
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("ReportAckPacket{host=%s,port=%d}\n",ipStr.c_str(),m_port);
}
/*WantHolePacket实现*/
const int WantHolePacket::wantHolePacketSize=18;
WantHolePacket::WantHolePacket(UInt32 host,UInt32 port):Packet(host,port),m_peerHost(0),m_peerPort(0),m_peerLocalHost(0),m_peerLocalPort(0){
	m_isControl=true;
	m_controlPacketType=WANT_HOLE;
}
WantHolePacket::~WantHolePacket(){
}
int WantHolePacket::getBytesLen(){
	return wantHolePacketSize;
}
void WantHolePacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
	ByteUtils::writeUInt32(dest,2,m_peerHost);
	ByteUtils::writeUInt32(dest,6,m_peerPort);
	ByteUtils::writeUInt32(dest,10,m_peerLocalHost);
	ByteUtils::writeUInt32(dest,14,m_peerLocalPort);
}
void WantHolePacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_peerHost=ByteUtils::readUInt32(src,2);
	m_peerPort=ByteUtils::readUInt32(src,6);
	m_peerLocalHost=ByteUtils::readUInt32(src,10);
	m_peerLocalPort=ByteUtils::readUInt32(src,14);
}
void WantHolePacket::printInfo(){
	Packet::printInfo();
	string ipStr,peerIpStr,peerLocalIpStr;
	CommonUtils::toIpStr(m_host,ipStr);
	CommonUtils::toIpStr(m_peerHost,peerIpStr);
	CommonUtils::toIpStr(m_peerLocalHost,peerLocalIpStr);
	m_logger.info("WantHolePacket{host=%s,port=%d,peerHost=%s,peerPort=%d,peerLocalHost=%s,peerLocalPort=%d}\n",
	ipStr.c_str(),m_port,peerIpStr.c_str(),m_peerPort,peerLocalIpStr.c_str(),m_peerLocalPort);
}
/*WantHoleAckPacket实现*/
const int WantHoleAckPacket::wantHoleAckPacketSize=19;
WantHoleAckPacket::WantHoleAckPacket(UInt32 host,UInt32 port):Packet(host,port),m_peerHost(0),m_peerPort(0),m_peerLocalHost(0),m_peerLocalPort(0),m_direction(0){
	m_isControl=true;
	m_controlPacketType=WANT_HOLE_ACK;
}
WantHoleAckPacket::~WantHoleAckPacket(){
}
int WantHoleAckPacket::getBytesLen(){
	return wantHoleAckPacketSize;
}
void WantHoleAckPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
	ByteUtils::writeUInt32(dest,2,m_peerHost);
	ByteUtils::writeUInt32(dest,6,m_peerPort);
	ByteUtils::writeUInt32(dest,10,m_peerLocalHost);
	ByteUtils::writeUInt32(dest,14,m_peerLocalPort);
	dest[18]=m_direction;
}
void WantHoleAckPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_peerHost=ByteUtils::readUInt32(src,2);
	m_peerPort=ByteUtils::readUInt32(src,6);
	m_peerLocalHost=ByteUtils::readUInt32(src,10);
	m_peerLocalPort=ByteUtils::readUInt32(src,14);
	m_direction=src[18];
}
void WantHoleAckPacket::printInfo(){
	Packet::printInfo();
	string ipStr,peerIpStr,peerLocalIpStr;
	CommonUtils::toIpStr(m_host,ipStr);
	CommonUtils::toIpStr(m_peerHost,peerIpStr);
	CommonUtils::toIpStr(m_peerLocalHost,peerLocalIpStr);
	m_logger.info("WantHoleAckPacket{host=%s,port=%d,peerHost=%s,peerPort=%d,peerLocalHost=%s,peerLocalPort=%d,direction=%d}\n",
	ipStr.c_str(),m_port,peerIpStr.c_str(),m_peerPort,peerLocalIpStr.c_str(),m_peerLocalPort,m_direction);
}
/*NotifyHolePacket实现*/
const int NotifyHolePacket::notifyHolePacketSize=19;
NotifyHolePacket::NotifyHolePacket(UInt32 host,UInt32 port):Packet(host,port),m_peerHost(0),m_peerPort(0),m_peerLocalHost(0),m_peerLocalPort(0),m_direction(0){
	m_isControl=true;
	m_controlPacketType=NOTIFY_HOLE;
}
NotifyHolePacket::~NotifyHolePacket(){
}
int NotifyHolePacket::getBytesLen(){
	return notifyHolePacketSize;
}
void NotifyHolePacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
	ByteUtils::writeUInt32(dest,2,m_peerHost);
	ByteUtils::writeUInt32(dest,6,m_peerPort);
	ByteUtils::writeUInt32(dest,10,m_peerLocalHost);
	ByteUtils::writeUInt32(dest,14,m_peerLocalPort);
	dest[18]=m_direction;
}
void NotifyHolePacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_peerHost=ByteUtils::readUInt32(src,2);
	m_peerPort=ByteUtils::readUInt32(src,6);
	m_peerLocalHost=ByteUtils::readUInt32(src,10);
	m_peerLocalPort=ByteUtils::readUInt32(src,14);
	m_direction=src[18];
}
void NotifyHolePacket::printInfo(){
	Packet::printInfo();
	string ipStr,peerIpStr,peerLocalIpStr;
	CommonUtils::toIpStr(m_host,ipStr);
	CommonUtils::toIpStr(m_peerHost,peerIpStr);
	CommonUtils::toIpStr(m_peerLocalHost,peerLocalIpStr);
	m_logger.info("NotifyHolePacket{host=%s,port=%d,peerHost=%s,peerPort=%d,peerLocalHost=%s,peerLocalPort=%d,direction=%d}\n",
	ipStr.c_str(),m_port,peerIpStr.c_str(),m_peerPort,peerLocalIpStr.c_str(),m_peerLocalPort,m_direction);
}
/*HolePacket实现*/
const int HolePacket::holePacketSize=2;
HolePacket::HolePacket(UInt32 host,UInt32 port):Packet(host,port){
	m_isControl=true;
	m_controlPacketType=HOLE;
}
HolePacket::~HolePacket(){
}
int HolePacket::getBytesLen(){
	return holePacketSize;
}
void HolePacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
}
void HolePacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
}
void HolePacket::printInfo(){
	Packet::printInfo();
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("HolePacket{host=%s,port=%d}\n",ipStr.c_str(),m_port);
}
/*LogoutPacket实现*/
const int LogoutPacket::logoutPacketSize=2;
LogoutPacket::LogoutPacket(UInt32 host,UInt32 port):Packet(host,port){
	m_isControl=true;
	m_controlPacketType=LOGOUT;
}
LogoutPacket::~LogoutPacket(){
}
int LogoutPacket::getBytesLen(){
	return logoutPacketSize;
}
void LogoutPacket::readBytes(char* dest){
	dest[0]=64;//01000000
	dest[1]=m_controlPacketType;
}
void LogoutPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
}
void LogoutPacket::printInfo(){
	Packet::printInfo();
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("LogoutPacket{host=%s,port=%d}\n",ipStr.c_str(),m_port);
}
/*TurnHeartPacket实现*/
const int TurnHeartPacket::turnHeartPacketSize=1;
TurnHeartPacket::TurnHeartPacket(UInt32 host,UInt32 port):Packet(host,port){
	m_isTurn=true;
	m_isTurnHeart=true;
}
TurnHeartPacket::~TurnHeartPacket(){
}
int TurnHeartPacket::getBytesLen(){
	return turnHeartPacketSize;
}
void TurnHeartPacket::readBytes(char* dest){
	dest[0]=(char)160;//10100000
}
void TurnHeartPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
}
void TurnHeartPacket::printInfo(){
	Packet::printInfo();
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("TurnHeartPacket{host=%s,port=%d}\n",ipStr.c_str(),m_port);
}
/*TurnDataPacket实现*/
const int TurnDataPacket::turnDataPacketHeadSize=DataPacket::dataPacketHeadSize+8;
TurnDataPacket::TurnDataPacket(UInt32 host,UInt32 port):Packet(host,port),m_pDp(NULL){
	m_isTurn=true;
	m_isControl=false;
	m_isTurnHeart=false;
}
TurnDataPacket::~TurnDataPacket(){
}
int TurnDataPacket::getBytesLen(){
	return turnDataPacketHeadSize+m_pDp->getWrapDataLen();
}
void TurnDataPacket::readBytes(char* dest){
	dest[0]=(char)128;//10000000
	ByteUtils::writeUInt32(dest,1,m_pDp->getHost());
	ByteUtils::writeUInt32(dest,5,m_pDp->getPort());
	char* bytes=new char[m_pDp->getBytesLen()];
	m_pDp->readBytes(bytes);
	memcpy(dest+9,bytes+1,m_pDp->getBytesLen()-1);
	delete bytes;
}
void TurnDataPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	m_pDp=new DataPacket(ByteUtils::readUInt32(src,1),ByteUtils::readUInt32(src,5));
	char* bytes=new char[len-8];
	bytes[0]=0;//00000000
	memcpy(bytes+1,src+9,len-9);
	m_pDp->writeBytes(bytes,len-8);
	delete bytes;
}
void TurnDataPacket::printInfo(){
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("TurnDataPacket{host=%s,port=%d}",ipStr.c_str(),m_port);
	m_pDp->printInfo();
}
/*TurnControlPacket实现*/
TurnControlPacket::TurnControlPacket(UInt32 host,UInt32 port):Packet(host,port),m_pP(NULL){
	m_isTurn=true;
	m_isControl=true;
	m_isTurnHeart=false;
}
TurnControlPacket::~TurnControlPacket(){
}
int TurnControlPacket::getBytesLen(){
    if(typeid(*m_pP)==typeid(LoginPeerPacket)){
    	return LoginPeerPacket::loginPeerPacketSize+8;
    }else if(typeid(*m_pP)==typeid(LoginPeerAckPacket)){
    	return LoginPeerAckPacket::loginPeerAckPacketSize+8;
    }else if(typeid(*m_pP)==typeid(HeartPacket)){
    	return HeartPacket::heartPacketSize+8;
    }else if(typeid(*m_pP)==typeid(HeartAckPacket)){
    	return HeartAckPacket::heartAckPacketSize+8;
    }else if(typeid(*m_pP)==typeid(AckPacket)){
    	return AckPacket::ackPacketSize+8;
    }else if(typeid(*m_pP)==typeid(LogoutPacket)){
    	return LogoutPacket::logoutPacketSize+8;
    }
	return 0;
}
void TurnControlPacket::readBytes(char* dest){
	dest[0]=(char)192;//11000000
	ByteUtils::writeUInt32(dest,1,m_pP->getHost());
	ByteUtils::writeUInt32(dest,5,m_pP->getPort());
	char* bytes=new char[m_pP->getBytesLen()];
	m_pP->readBytes(bytes);
	memcpy(dest+9,bytes+1,m_pP->getBytesLen()-1);
	delete bytes;
}
void TurnControlPacket::writeBytes(char* src,int len){
	Packet::writeBytes(src,len);
	newPacket(ByteUtils::readUInt32(src,1),ByteUtils::readUInt32(src,5));
	char* bytes=new char[len-8];
	bytes[0]=64;//01000000
	memcpy(bytes+1,src+9,len-9);
	m_pP->writeBytes(bytes,len-8);
	delete bytes;
}
void TurnControlPacket::newPacket(UInt32 peerHost,UInt32 peerPort){
   if(m_controlPacketType==LOGIN_PEER){
	   m_pP=new LoginPeerPacket(peerHost,peerPort);
   }else if(m_controlPacketType==LOGIN_PEER_ACK){
	   m_pP=new LoginPeerAckPacket(peerHost,peerPort);
   }else if(m_controlPacketType==HEART){
	   m_pP=new HeartPacket(peerHost,peerPort);
   }else if(m_controlPacketType==HEART_ACK){
	   m_pP=new HeartAckPacket(peerHost,peerPort);
   }else if(m_controlPacketType==ACK){
	   m_pP=new AckPacket(peerHost,peerPort);
   }else if(m_controlPacketType==LOGOUT){
	   m_pP=new LogoutPacket(peerHost,peerPort);
   }
}
void TurnControlPacket::printInfo(){
	string ipStr;
	CommonUtils::toIpStr(m_host,ipStr);
	m_logger.info("TurnControlPacket{host=%s,port=%d}",ipStr.c_str(),m_port);
	m_pP->printInfo();
}
