#ifndef __GROUP_H__
#define __GROUP_H__
#include "pt_api.h"
#include "group_pkt.h"
namespace PT{
/*GroupInfo*/
class GroupInfo{
public:
	GroupInfo();
	~GroupInfo();
	void addPeerInfo(const PeerInfo& peerInfo);
	void removePeerInfo(const PeerInfo& peerInfo);
	void addIndex(const PeerInfo& peerInfo,int firstIndex,int lastIndex);
	void removeIndex(const PeerInfo& peerInfo,int firstIndex,int lastIndex);
	void removePeerInfoAllIndex(const PeerInfo& peerInfo);
	bool whichHaveData(int index,PeerInfo& peerInfo);
	void getPeerInfos(list<PeerInfo>& peerInfos);
	void printPeerInfos();
	void printIndexInfos();
private:
	map<string,PeerInfo> m_peerInfos;
	map<int,list<PeerInfo> > m_indexInfos;
	SimpleLock m_lock;
	SimpleLogger m_logger;
};
/*GroupReceiver*/
class GroupNodeSession;
class GroupReceiver{
public:
	virtual void receive(GroupNodeSession* pSession,GroupPacket* pPacket)=0;
};
/*GroupSender*/
class GroupSender{
public:
	virtual void sendData(char* data,int length,const PeerInfo& peerInfo)=0;
};
/*GroupNodeSession*/
class BufferPacket;
class GroupNodeSession{
public:
	GroupNodeSession(GroupSender* pSender,GroupReceiver* pReceiver,const PeerInfo& peerInfo);
	~GroupNodeSession();
	void processData(char* data,int len);
	void sendPacket(GroupPacket* pGroupPacket);
public:
	string m_groupName;
	GroupInfo* m_pGroupInfo;
	GroupSender* m_pSender;
	GroupReceiver* m_pReceiver;
	PeerInfo m_peerInfo;
private:
	BufferPacket* m_pBufferPacket;
	SimpleLogger m_logger;
};
/*BufferPacket*/
class BufferPacket{
public:
	BufferPacket(GroupNodeSession* pSession);
	~BufferPacket();
	void processData(char* data,int offset,int len);
private:
	void parseComplete();
public:
	GroupNodeSession* m_pSession;
private:
	char* m_headData;
	bool m_completeHead;
	int m_currentHeadLen;
	char* m_bodyData;
	int m_bodyLen;
	int m_currentBodyLen;
};
/*OnReceivePacket*/
class OnReceivePacket:public Runnable{
public:
	OnReceivePacket(GroupNodeSession* pSession,GroupPacket* pPacket):m_pSession(pSession),m_pPacket(pPacket){
	}
	virtual void run(){
		m_pSession->m_pReceiver->receive(m_pSession,m_pPacket);
	}
private:
	GroupNodeSession* m_pSession;
	GroupPacket* m_pPacket;
};
/*SimpleGroupServer*/
class SimpleGroupServer:public PTGroupServer,PTServerHandler,GroupSender,GroupReceiver{
public:
	SimpleGroupServer(bool supportTurn);
	~SimpleGroupServer();
	virtual void start(UInt32 host,UInt32 mainPort,UInt32 judgeNatPort1,UInt32 judgeNatPort2,UInt32 turnPort);
	virtual void shutdown();
	virtual void waitForEnd();
	virtual void onPeerLogin(const PeerInfo& peerInfo);
	virtual void onPeerLogout(const PeerInfo& peerInfo);
	virtual void onPeerData(char* data,int len,const PeerInfo& peerInfo);
	virtual void onDisconnectToPeer(const PeerInfo& peerInfo);
	virtual void sendData(char* data,int length,const PeerInfo& peerInfo);
	virtual void receive(GroupNodeSession* pSession,GroupPacket* pPacket);
private:
	void processJoinGroupPacket(GroupNodeSession* pSession,JoinGroupPacket* pPacket);
	void processChunkPacket(GroupNodeSession* pSession,ChunkPacket* pPacket);
	void addSession(GroupNodeSession* pSession);
	void removeSession(GroupNodeSession* pSession);
	GroupNodeSession* getSession(const PeerInfo& peerInfo);
	void removeGroupInfo(const string& groupName);
private:
	bool m_supportTurn;
	PTServer* m_pServer;
	map<string,GroupNodeSession*> m_pSessions;
	SimpleLock m_session_lock;
	map<string ,GroupInfo*> m_pGroupInfos;
	SimpleLock m_group_lock;
	SimpleLogger m_logger;
};
/*SimpleGroup*/
class SimpleGroup:public PTGroup,PTPeerHandler,GroupReceiver,GroupSender{
public:
	SimpleGroup(const string& groupName,bool supportTurn);
	~SimpleGroup();
	virtual void setHandler(PTGroupHandler* pHandler);
	virtual void start(UInt32 serverHost,UInt32 serverPort);
	virtual void start(UInt32 localHost,UInt32 localPort,UInt32 serverHost,UInt32 serverPort);
	virtual void shutdown();
	virtual void waitForEnd();
	virtual void joinGroup();
	virtual void sendDataToPeer(char* data,int length,const PeerInfo& peerInfo);
	virtual void sendDataToAll(char* data,int length);
	virtual void addHaveData(int firstIndex,int lastIndex);
	virtual void removeHaveData(int firstIndex,int lastIndex);
	virtual bool existData(int index);
	virtual void addWantData(int index);
	virtual void writeRequestedData(const PeerInfo& peerInfo,int index,char* data,int length);
	virtual void getLocalPeerInfo(PeerInfo& peerInfo);
	virtual int getLoginMode(const PeerInfo& peerInfo);
	virtual void receive(GroupNodeSession* pSession,GroupPacket* pPacket);
	virtual void sendData(char* data,int length,const PeerInfo& peerInfo);

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
private:
	void processJoinGroupAckPacket(GroupNodeSession* pSession,JoinGroupAckPacket* pPacket);
	void processChunkPacket(GroupNodeSession* pSession,ChunkPacket* pPacket);
	void processAddHaveDataPacket(GroupNodeSession* pSession,AddHaveDataPacket* pPacket);
	void processRemoveHaveDataPacket(GroupNodeSession* pSession,RemoveHaveDataPacket* pPacket);
	void processNotifySynIndexsPacket(GroupNodeSession* pSession,NotifySynIndexsPacket* pPacket);
	void processSynIndexsPacket(GroupNodeSession* pSession,SynIndexsPacket* pPacket);
	void processWantDataPacket(GroupNodeSession* pSession,WantDataPacket* pPacket);
	void processWantDataAckPacket(GroupNodeSession* pSession,WantDataAckPacket* pPacket);
	void processNotifyWriteDataPacket(GroupNodeSession* pSession,NotifyWriteDataPacket* pPacket);
	void processShareDataPacket(GroupNodeSession* pSession,ShareDataPacket* pPacket);
	void addSession(GroupNodeSession* pSession);
	void removeSession(GroupNodeSession* pSession);
	void removeSession(const PeerInfo& peerInfo);
	GroupNodeSession* getSession(const PeerInfo& peerInfo);
private:
	string m_groupName;
	bool m_supportTurn;
	PTGroupHandler* m_pHandler;
	PTPeer* m_pPeer;
	PeerInfo m_serverPeerInfo;
	PeerInfo m_localPeerInfo;
	GroupNodeSession* m_pServerSession;
	map<string,GroupNodeSession*> m_pSessions;
	SimpleLock m_session_lock;
	set<int> m_indexs;
	SimpleLock m_index_lock;
	GroupInfo* m_pGroupInfo;
	SimpleLock m_group_lock;
	SimpleLogger m_logger;
};
}
#endif
