#ifndef __COMMON_H__
#define __COMMON_H__

#include <set>
#include <vector>
#include <list>
#include <map>
#include<iostream>
#include <memory.h>
#ifndef WIN32
#include<pthread.h>
#include <sys/time.h>
#else
#include <Windows.h>
#endif

#ifdef WIN32
typedef HANDLE pthread_t;
typedef HANDLE pthread_mutex_t;
typedef HANDLE pthread_cond_t;
#endif

#include "pt_api.h"
using namespace PT;
namespace PT{
/*CommonUtils*/
class CommonUtils{
public:
    static void split(const string& src, const string& separator, vector<string>& dest);
    static void toIpStr(UInt32 ip,string& ipStr);
    static UInt32 toIpInt(const string& ip);
    static void toIpPortKey(UInt32 host,UInt32 port,string& key);
    static bool isLocalIp(UInt32 ip);
    static void printBytes(const char* data,int start,int len);
};
/*SimpleLogger定义*/
class SimpleLogger{
public:
	SimpleLogger(const string& className);
	~SimpleLogger();
	inline void getClassName(string& className){
		className=m_className;
	}
	void error(const char* msg,...);
	void info(const char* msg,...);
	void debug(const char* msg,...);
private:
	bool existExcludeClassName();
public:
	static const int ERROR_LEVEL;
	static const int INFO_LEVEL;
	static const int DEBUG_LEVEL;
private:
	string m_className;
	bool m_isPrint;
    static int m_level;
    static set<string> m_excludeClassNames;
    static bool m_isInit;
};
/*TimeUtils*/
class TimeUtils{
public:
	static UInt64 currentTimeMillis();//当前时间戳，毫秒
#ifndef WIN32
	static UInt64 microsecondTime(timeval lastTime);//从指定的时间到现在微秒
#else
	static UInt64 microsecondTime(LARGE_INTEGER lastTime);//从指定的时间到现在微秒
private:
	static ULARGE_INTEGER m_ullUtcTime;
	static bool m_initUtcTime;
	static LARGE_INTEGER m_Freq;
	static bool m_initFreq;
#endif
};
/*ByteUtils定义*/
class ByteUtils{
private:
	ByteUtils();
public:
	static void writeUInt32(char* dest,int start,UInt32 value);
	static UInt32 readUInt32(const char* src,int start);
    static void writeUInt64(char* dest,int start,UInt64 value);
    static UInt64 readUInt64(const char* src,int start);
};
/*SimpleObject定义*/
class SimpleObject{
public:
	SimpleObject();
	~SimpleObject();
	void wait();
	void wait(UInt32 time);
	void notify();
private:
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
};
/*Runnable定义*/
class Runnable{
public:
	virtual void run()=0;
};
/*SimpleThread定义*/
class SimpleThread{
public:
	SimpleThread();
	SimpleThread(Runnable* pR);
	virtual ~SimpleThread();
	void start();
	virtual void run();
	void waitForEnd();
	void cancel();//应该避免使用该方法,调用了该方法线程中new的对象可能得不到释放
#ifndef WIN32
    static void* threadFun(void*);
#else
    static DWORD WINAPI threadFun(LPVOID param);
#endif
private:
	pthread_t m_thread;
	Runnable* m_pR;
	SimpleLogger m_logger;
#ifdef WIN32
	pthread_cond_t m_ExitCond;
#endif
};
/*DoRepeatWork*/
class DoRepeatWork{
public:
	DoRepeatWork(){}
	virtual ~DoRepeatWork(){}
	virtual void doRepeatWork(){}
	virtual void onStart(){}
	virtual void onStop(){}
};
/*RepeatWork定义*/
class RepeatWork:public Runnable,public SimpleObject{
public:
	RepeatWork(DoRepeatWork* pDrw,SimpleLogger* pLogger);
	virtual ~RepeatWork();
	void start();
	void stop();
	void waitForEnd();
	virtual void run();
private:
	void printMsg(string msg);
private:
	bool m_IsStop;
	SimpleThread* m_pThread;
	DoRepeatWork* m_pDrw;
	SimpleLogger* m_pLogger;
};
/*SimpleLock定义*/
class SimpleLock{
public:
	SimpleLock();
	~SimpleLock();
	void lock();
	void unlock();
private:
	pthread_mutex_t m_mutex;
};
/*BlockingQueue定义*/
template<class T>
class BlockingQueue:public SimpleObject{
public:
	BlockingQueue(int capacity);
	~BlockingQueue();
	void put(const T data);
	void get(T& data);
	bool isEmpty();
	int size();
private:
	int m_size;
	int m_capacity;//值为-1，表示容量不作限制
    list<T> m_datas;
    SimpleLock m_lock;
};
/*BlockingQueue实现*/
template<class T>
BlockingQueue<T>::BlockingQueue(int capacity):SimpleObject(),m_size(0),m_capacity(capacity),m_datas(),m_lock(){
}
template<class T>
BlockingQueue<T>::~BlockingQueue(){
}
template<class T>
void BlockingQueue<T>::put(const T data){
	if(m_capacity!=-1){
		while(m_size>=m_capacity){
			SimpleObject::wait();
		}
	}
	m_lock.lock();
	m_datas.push_back(data);
	m_size++;
	SimpleObject::notify();
	m_lock.unlock();
}
template<class T>
void BlockingQueue<T>::get(T& data){
	while(m_size==0){
		SimpleObject::wait();
	}
	m_lock.lock();
	data=m_datas.front();
	m_datas.pop_front();
    m_size--;
    SimpleObject::notify();
    m_lock.unlock();
}
template<class T>
bool BlockingQueue<T>::isEmpty(){
	bool isEmpty;
	m_lock.lock();
	isEmpty=m_datas.size()==0?true:false;
	m_lock.unlock();
	return isEmpty;
}
template<class T>
int BlockingQueue<T>::size(){
	int size;
	m_lock.lock();
	size=m_datas.size();
	m_lock.unlock();
	return size;
}
/*ThreadPool定义*/
class ExecuteTask;
class ThreadPool{
public:
	ThreadPool(int initSize,int maxSize);
	~ThreadPool();
	void execute(Runnable* pR);
	void clear();
	void shutdown();
	void shutdownNow();
private:
	void init();
	void addInvoke();
public:
	int m_initSize;
	int m_maxSize;
	int m_size;//当前实际线程个数
	int m_idle_size;//空闲的线程个数
	BlockingQueue<Runnable*> m_tasks;
	list<ExecuteTask*> m_invokes;
	SimpleLock m_invoke_lock;
};
/*ExecuteTask定义*/
class ExecuteTask:public SimpleThread{
public:
	ExecuteTask(ThreadPool* pTp);
	~ExecuteTask();
	virtual void run();
	void shutdown();
	void shutdownNow();
private:
	ThreadPool* m_pTp;
	bool m_is_idle;
	bool m_is_stop;
};
/*CoreExecutorService定义*/
class CoreExecutorService{
public:
	~CoreExecutorService();
	static void newInstance();
	static CoreExecutorService* getInstance();
	void execute(Runnable* pR);
	void shutdown();
private:
	CoreExecutorService();
public:
	static CoreExecutorService* m_pInstance;
	static const int INIT_THREAD_SIZE;
	static const int MAX_THREAD_SIZE;
private:
	ThreadPool m_tp;
};
/*ConcurrentQueue定义*/
template<class T>
class ConcurrentQueue{
public:
	ConcurrentQueue();
	~ConcurrentQueue();
	void add(const T& obj);
	bool get(T& obj);
	void remove(const T& obj);
	bool isEmpty();
	int size();
private:
	list<T> m_datas;
	SimpleLock m_lock;
};
/*ConcurrentQueue实现*/
template<class T>
ConcurrentQueue<T>::ConcurrentQueue():m_datas(),m_lock(){
}
template<class T>
ConcurrentQueue<T>::~ConcurrentQueue(){
}
template<class T>
void ConcurrentQueue<T>::add(const T& obj){
	m_lock.lock();
	m_datas.push_back(obj);
	m_lock.unlock();
}
template<class T>
bool ConcurrentQueue<T>::get(T& obj){
	bool exist=false;
	m_lock.lock();
	typename list<T>::iterator it;
	for(it=m_datas.begin();it!=m_datas.end();it++){
		if(*it==obj){
			obj=*it;
			exist=true;
			break;
		}
	}
	m_lock.unlock();
	return exist;
}
template<class T>
void ConcurrentQueue<T>::remove(const T& obj){
	m_lock.lock();
	m_datas.remove(obj);
	m_lock.unlock();
}
template<class T>
bool ConcurrentQueue<T>::isEmpty(){
	return size()==0?true:false;
}
template<class T>
int ConcurrentQueue<T>::size(){
	int size;
	m_lock.lock();
	size=m_datas.size();
	m_lock.unlock();
	return size;
}
/*ConcurrentMap定义*/
template<class T1,class T2>
class ConcurrentMap{
public:
	ConcurrentMap();
	~ConcurrentMap();
	void put(T1 key,T2 value);
	bool get(T1 key,T2& value);
	void remove(T1 key);
	bool isEmpty();
	int size();
private:
	map<T1,T2> m_datas;
	SimpleLock m_lock;
};
/*ConcurrentMap实现*/
template<class T1,class T2>
ConcurrentMap<T1,T2>::ConcurrentMap():m_datas(),m_lock(){
}
template<class T1,class T2>
ConcurrentMap<T1,T2>::~ConcurrentMap(){
}
template<class T1,class T2>
void ConcurrentMap<T1,T2>::put(T1 key,T2 value){
	m_lock.lock();
	typename map<T1,T2>::iterator it;
	it=m_datas.find(key);
	if(it!=m_datas.end()){
		m_datas.erase(it);
	}
	m_datas.insert(pair<T1,T2>(key,value));
	m_lock.unlock();
}
template<class T1,class T2>
bool ConcurrentMap<T1,T2>::get(T1 key,T2& value){
	bool exist=false;
	m_lock.lock();
	typename map<T1,T2>::iterator it=m_datas.find(key);
	if(it!=m_datas.end()){
		exist=true;
		value=it->second;
	}
	m_lock.unlock();
    return exist;
}
template<class T1,class T2>
void ConcurrentMap<T1,T2>::remove(T1 key){
	m_lock.lock();
	typename map<T1,T2>::iterator it;
	it=m_datas.find(key);
	if(it!=m_datas.end()){
		m_datas.erase(it);
	}
	m_lock.unlock();
}
template<class T1,class T2>
bool ConcurrentMap<T1,T2>::isEmpty(){
	return size()==0?true:false;
}
template<class T1,class T2>
int ConcurrentMap<T1,T2>::size(){
	int size;
	m_lock.lock();
	size=m_datas.size();
	m_lock.unlock();
	return size;
}
}
#endif
