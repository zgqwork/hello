#ifndef __PT_API_H__
#define __PT_API_H__
#include <string>

typedef unsigned char  UInt8;
typedef unsigned short UInt16;
typedef unsigned int   UInt32;
#ifndef WIN32
typedef unsigned long long UInt64;
#else
typedef unsigned __int64 UInt64;
#endif

#define __version "0.1.0_beta"
#define __THREAD_POOL_INIT_SIZE 5
#define __THREAD_POOL_MAX_SIZE 50
#define __loggerLevel SimpleLogger::DEBUG_LEVEL//日志打印级别,ERROR、INFO、DEBUG
#define __loggerExcludeClassNames "endpoint,exception,node,packet,peer,server"//哪些类不要打印日志信息,类名称前缀,"common,endpoint,exception,group.pkt,group,node,packet,peer,server"
#define __isSendHeart true //是否发送心跳包,测试时可以设为false
#define __isReleaseSession true //是否释放过期的session，测试时可以设为false
#define __uintMaxValue 10000;//值为:4294967295;设置10000为了测试用,该值影响数据包流水号的范围

using namespace std;

namespace PT{
enum LoginMode{
	SERVER_CLIENT,//服务器与客户端连接模式
	LAN,//局域网内互联
	P2P,//p2p模式
	TURN,//服务器转发数据包
};
enum NatType {
	No_NAT,
	Full_Cone_NAT_OR_Restricted_Cone_NAT,
	Port_Restricted_Cone_NAT,
	Symmetric_NAT,
};
/*PTException*/
class PTException{
public:
	PTException(int code,const string& errorMsg);
	PTException(const PTException& obj);
	virtual ~PTException();
	virtual int getCode();
	virtual void getErrorMsg(string& errorMsg);
private:
	int m_code;
	string m_errorMsg;
public:
	static const int CREATE_THREAD_ERROR;
	static const int CREATE_SOCKET_ERROR;
	static const int BIND_SOCKET_ERROR;
};
/*PeerInfo*/
class PeerInfo{
public:
	PeerInfo();
	PeerInfo(UInt32 wanHost,UInt32 wanPort,UInt32 localHost,UInt32 localPort);
	PeerInfo(const string& peerId);
	PeerInfo(const PeerInfo& obj);
	~PeerInfo();
	const PeerInfo& operator=(const PeerInfo& obj);
	bool operator==(const PeerInfo& obj);
	void toPeerId(string& peerId);
	void printInfo();
public:
	UInt32 m_wanHost;
	UInt32 m_wanPort;
	UInt32 m_localHost;
	UInt32 m_localPort;
};
/*PTServerHandler*/
class PTServerHandler{
public:
	PTServerHandler();
	virtual ~PTServerHandler();
	virtual void onPeerLogin(const PeerInfo& peerInfo);
	virtual void onPeerLogout(const PeerInfo& peerInfo);
	virtual void onPeerData(char* data,int len,const PeerInfo& peerInfo);
	virtual void onDisconnectToPeer(const PeerInfo& peerInfo);
};
/*PTServer*/
class PTServer{
public:
	virtual void setHandler(PTServerHandler* pHandler)=0;
	virtual void start(UInt32 host,UInt32 mainPort,UInt32 judgeNatPort1,UInt32 judgeNatPort2,UInt32 turnPort)=0;
	virtual void shutdown()=0;
	virtual void waitForEnd()=0;
	virtual void sendDataToPeer(char* data,int len,const PeerInfo& peerInfo)=0;
	virtual int getNatType(const PeerInfo& peerInfo)=0;
};
/*PTPeerHandler*/
class PTPeerHandler{
public:
	PTPeerHandler();
	virtual ~PTPeerHandler();
	virtual void onLoginServerSuccess(const PeerInfo& peerInfo);
	virtual void onLoginServerFailure();
	virtual void onServerData(char* data,int len);
	virtual void onDisconnectToServer();
	virtual void onLoginPeerSuccess(const PeerInfo& peerInfo);
	virtual void onLoginPeerFailure(const PeerInfo& peerInfo,int loginMode);
	virtual void onPeerLogin(const PeerInfo& peerInfo);
	virtual void onPeerLogout(const PeerInfo& peerInfo);
	virtual void onPeerData(char* data,int len,const PeerInfo& peerInfo);
	virtual void onDisconnectToPeer(const PeerInfo& peerInfo);
};
/*PTPeer*/
class PTPeer{
public:
	virtual void setHandler(PTPeerHandler* pHandler)=0;
	virtual void start()=0;
	virtual void start(UInt32 localHost,UInt32 localPort)=0;
	virtual void shutdown()=0;
	virtual void waitForEnd()=0;
	virtual void loginServer(UInt32 serverHost,UInt32 serverPort)=0;
	virtual void loginServer(UInt32 serverHost,UInt32 serverPort,int timeout)=0;
	virtual void sendDataToServer(char* data,int len)=0;
	virtual void logoutServerAndShutdown()=0;
	virtual void loginPeer(const PeerInfo& peerInfo)=0;
	virtual void loginPeer(const PeerInfo& peerInfo,int loginMode)=0;
	virtual void loginPeer(const PeerInfo& peerInfo,int loginMode,int timeout)=0;
	virtual void sendDataToPeer(char* data,int len,const PeerInfo& peerInfo)=0;
	virtual void logoutPeer(const PeerInfo& peerInfo)=0;
	virtual void logoutAllAndShutdown()=0;
	virtual int getLocalNatType()=0;
	virtual void getLocalPeerInfo(PeerInfo& peerInfo)=0;
	virtual int getLoginMode(const PeerInfo& peerInfo)=0;
};
/*PTGroupServer*/
class PTGroupServer{
public:
	virtual void start(UInt32 host,UInt32 mainPort,UInt32 judgeNatPort1,UInt32 judgeNatPort2,UInt32 turnPort)=0;
	virtual void shutdown()=0;
	virtual void waitForEnd()=0;
};
/*PTGroupHandler*/
class PTGroupHandler{
public:
	PTGroupHandler();
	~PTGroupHandler();
	virtual void onLoginServerSuccess();
	virtual void onLoginServerFailure();
	virtual void onDisconnectToServer();
	virtual void onLoginPeerSuccess(const PeerInfo& peerInfo);
	virtual void onLoginPeerFailure(const PeerInfo& peerInfo);
	virtual void onPeerLogin(const PeerInfo& peerInfo);
	virtual void onPeerLogout(const PeerInfo& peerInfo);
	virtual void onDisconnectToPeer(const PeerInfo& peerInfo);
	virtual void onData(const PeerInfo& peerInfo,char* data,int length);
	virtual void onWantDataFailure(int index);
	virtual void onRequestData(const PeerInfo& peerInfo,int index);
	virtual void onFetchData(const PeerInfo& peerInfo,int index,char* data,int length);
};
/*PTGroup*/
class PTGroup{
public:
	virtual void setHandler(PTGroupHandler* pHandler)=0;
	virtual void start(UInt32 serverHost,UInt32 serverPort)=0;
	virtual void start(UInt32 localHost,UInt32 localPort,UInt32 serverHost,UInt32 serverPort)=0;
	virtual void shutdown()=0;
	virtual void waitForEnd()=0;
	virtual void joinGroup()=0;
	virtual void sendDataToPeer(char* data,int length,const PeerInfo& peerInfo)=0;
	virtual void sendDataToAll(char* data,int length)=0;
	virtual void addHaveData(int firstIndex,int lastIndex)=0;
	virtual void removeHaveData(int firstIndex,int lastIndex)=0;
	virtual bool existData(int index)=0;
	virtual void addWantData(int index)=0;
	virtual void writeRequestedData(const PeerInfo& peerInfo,int index,char* data,int length)=0;
	virtual void getLocalPeerInfo(PeerInfo& peerInfo)=0;
	virtual int getLoginMode(const PeerInfo& peerInfo)=0;
};
/*全局函数*/
void toIpStr(UInt32 ip,string& ipStr);
UInt32 toIpInt(const string& ip);
PTServer* createServer(bool supportTurn);
void destroyServer(PTServer* pServer);
PTPeer* createPeer(bool supportTurn);
void destroyPeer(PTPeer* pPeer);
PTGroupServer* createGroupServer(bool supportTurn);
void destroyGroupServer(PTGroupServer* pGroupServer);
PTGroup* createGroup(const string& groupName,bool supportTurn);
void destroyGroup(PTGroup* pGroup);
};
#endif
