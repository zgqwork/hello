#ifndef __GROUP_PKT_H__
#define __GROUP_PKT_H__
#include "packet.h"
namespace PT{
enum GroupPacketType {
	JOIN_GROUP,
	JOIN_GROUP_ACK,
	CHUNK,
	ADD_HAVE_DATA,
	REMOVE_HAVE_DATA,
	NOTIFY_SYN_INDEXS,
	SYN_INDEXS,
	WANT_DATA,
	WANT_DATA_ACK,
	NOTIFY_WRITE_DATA,
	SHARE_DATA,
};
enum HaveDataResultType {
	Have,
	Not_Have,
};
/*GroupPacket*/
class GroupPacket{
public:
	virtual int getBytesLen()=0;
	virtual void readBytes(char* dest)=0;
	virtual void writeBytes(char* src,int len)=0;
	virtual void printInfo()=0;
};
/*WrapPacket*/
class WrapPacket{
public:
	static GroupPacket* toPacket(int type,char* data,int length);
	static void toBytes(GroupPacket* pGroupPacket,char* dest);
public:
	static const int headSize;
};
/*JoinGroupPacket*/
class JoinGroupPacket:public GroupPacket{
public:
	JoinGroupPacket();
	JoinGroupPacket(const string& groupName);
	virtual ~JoinGroupPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline void getGroupName(string& groupName){
		groupName=m_groupName;
	}
private:
	string m_groupName;
	SimpleLogger m_logger;
};
/*JoinGroupAckPacket*/
class JoinGroupAckPacket:public GroupPacket{
public:
	JoinGroupAckPacket();
	JoinGroupAckPacket(const list<PeerInfo>& peerInfos);
	virtual ~JoinGroupAckPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline void getPeerInfos(list<PeerInfo>& peerInfos){
		peerInfos=m_peerInfos;
	}
private:
	list<PeerInfo> m_peerInfos;
	SimpleLogger m_logger;
};
/*ChunkPacket*/
class ChunkPacket:public GroupPacket{
public:
	ChunkPacket();
	ChunkPacket(char* chunkData,int chunkDataLen);
	virtual ~ChunkPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline void readChunkData(char* dest){
		memcpy(dest,m_chunkData,m_chunkDataLen);
	}
	inline int getChunkDataLen(){
		return m_chunkDataLen;
	}
private:
	char* m_chunkData;
	int m_chunkDataLen;
	SimpleLogger m_logger;
};
/*AddHaveDataPacket*/
class AddHaveDataPacket:public GroupPacket{
public:
	AddHaveDataPacket();
	AddHaveDataPacket(int firstIndex,int lastIndex);
	virtual ~AddHaveDataPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline int getFirstIndex(){
		return m_firstIndex;
	}
	inline int getLastIndex(){
		return m_lastIndex;
	}
private:
	int m_firstIndex;
	int m_lastIndex;
	SimpleLogger m_logger;
};
/*AddHaveDataPacket*/
class RemoveHaveDataPacket:public GroupPacket{
public:
	RemoveHaveDataPacket();
	RemoveHaveDataPacket(int firstIndex,int lastIndex);
	virtual ~RemoveHaveDataPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline int getFirstIndex(){
		return m_firstIndex;
	}
	inline int getLastIndex(){
		return m_lastIndex;
	}
private:
	int m_firstIndex;
	int m_lastIndex;
	SimpleLogger m_logger;
};
/*NotifySynIndexsPacket*/
class NotifySynIndexsPacket:public GroupPacket{
public:
	NotifySynIndexsPacket();
	virtual ~NotifySynIndexsPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
private:
	SimpleLogger m_logger;
};
/*SynIndexsPacket*/
class SynIndexsPacket:public GroupPacket{
public:
	SynIndexsPacket();
	SynIndexsPacket(set<int> indexs);
	virtual ~SynIndexsPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline void getIndexs(set<int>& indexs){
		indexs=m_indexs;
	}
private:
	void splitIndexs();
	UInt64 tupleToUInt64(UInt32 v1,UInt32 v2);
	void uInt64ToTuple(UInt64 v,UInt32& v1,UInt32& v2);
private:
	list<int> m_dispersedIndexs;
	list<UInt64> m_tupleIndexs;
	set<int> m_indexs;
	SimpleLogger m_logger;
};
/*WantDataPacket*/
class WantDataPacket:public GroupPacket{
public:
	WantDataPacket();
	WantDataPacket(int index);
	virtual ~WantDataPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
    inline int getIndex(){
    	return m_index;
    }
private:
	int m_index;
	SimpleLogger m_logger;
};
/*WantDataAckPacket*/
class WantDataAckPacket:public GroupPacket{
public:
	WantDataAckPacket();
	WantDataAckPacket(int index,int result);
	virtual ~WantDataAckPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline int getIndex(){
		return m_index;
	}
	inline int getResult(){
		return m_result;
	}
private:
	int m_index;
	int m_result;;
	SimpleLogger m_logger;
};
/*NotifyWriteDataPacket*/
class NotifyWriteDataPacket:public GroupPacket{
public:
	NotifyWriteDataPacket();
	NotifyWriteDataPacket(int index);
	virtual ~NotifyWriteDataPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline int getIndex(){
		return m_index;
	}
private:
	int m_index;
	SimpleLogger m_logger;
};
/*ShareDataPacket*/
class ShareDataPacket:public GroupPacket{
public:
	ShareDataPacket();
	ShareDataPacket(int index,char* data,int dataLen);
	virtual ~ShareDataPacket();
	virtual int getBytesLen();
	virtual void readBytes(char* dest);
	virtual void writeBytes(char* src,int len);
	virtual void printInfo();
	inline int getIndex(){
		return m_index;
	}
	inline void readShareData(char* dest){
		memcpy(dest,m_data,m_dataLen);
	}
	inline int getShareDataLen(){
		return m_dataLen;
	}
private:
	int m_index;
	char* m_data;
	int m_dataLen;
	SimpleLogger m_logger;
};
}
#endif
