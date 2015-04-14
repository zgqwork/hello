#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "common.h"

/*CommonUtils*/
void CommonUtils::split(const string& src, const string& separator, vector<string>& dest){
    string str = src;
    string substring;
    string::size_type start = 0,index;
    do{
    	index = str.find_first_of(separator,start);
        if (index != string::npos){
            substring = str.substr(start,index-start);
            dest.push_back(substring);
            start = str.find_first_not_of(separator,index);
            if (start == string::npos) return;
        }
    }while(index != string::npos);
    substring = str.substr(start);
    dest.push_back(substring);
}
void CommonUtils::toIpStr(UInt32 ip,string& ipStr){
	UInt8 m1=ip>>24;
	UInt8 m2=ip>>16;
	UInt8 m3=ip>>8;
	UInt8 m4=ip;
	char temp[16];
	memset(temp,0,sizeof(temp));
	sprintf(temp,"%d.%d.%d.%d",m4,m3,m2,m1);
    ipStr=string(temp);
}
UInt32 CommonUtils::toIpInt(const string& ip){
	vector<string> items;
	split(ip,".",items);
	int m1=atoi(items.at(0).c_str());
	int m2=atoi(items.at(1).c_str());
	int m3=atoi(items.at(2).c_str());
	int m4=atoi(items.at(3).c_str());
	UInt32 result =0;
	result = (UInt32)(m4&0xFF)<<24|(m3&0xFF)<<16|(m2&0xFF)<<8|(m1&0xFF);
	return result;
}
void CommonUtils::toIpPortKey(UInt32 host,UInt32 port,string& key){
	UInt8 m1=host>>24;
	UInt8 m2=host>>16;
	UInt8 m3=host>>8;
	UInt8 m4=host;
	char temp[48];
	memset(temp,0,48);
	sprintf(temp,"%d.%d.%d.%d:%d",m4,m3,m2,m1,port);
	key=string(temp);
}
bool CommonUtils::isLocalIp(UInt32 ip){
	UInt8 m1=ip>>24;
	UInt8 m2=ip>>16;
	if(m1==192&&m2==168){
		return true;
	}else if(m1==10){
		return true;
	}else if(m1==172&&m2<=31&&m2>=16){
		return true;
	}
	return false;
}
void CommonUtils::printBytes(const char data[],int start,int len){
	UInt8 temp;
	int i;
	printf("[");
	for(i=start;i<len;i++){
		temp=(UInt8)data[i];
		if(i==start){
			printf("%d",temp);
		}else{
			printf(",%d",temp);
		}
	}
	printf("]\n");
}
/*SimpleLogger实现*/
const int SimpleLogger::ERROR_LEVEL=1;
const int SimpleLogger::INFO_LEVEL=2;
const int SimpleLogger::DEBUG_LEVEL=3;
int SimpleLogger::m_level=__loggerLevel;
set<string> SimpleLogger::m_excludeClassNames;
bool SimpleLogger::m_isInit=false;
SimpleLogger::SimpleLogger(const string& className):m_className(className),m_isPrint(true){
	if(!m_isInit){
		m_isInit=true;
		vector<string> items;
		vector<string>::iterator it;
		CommonUtils::split(__loggerExcludeClassNames,",",items);
		for(it=items.begin();it!=items.end();it++){
			m_excludeClassNames.insert(*it);
		}
	}
	if(existExcludeClassName()){
		m_isPrint=false;
	}
}
SimpleLogger::~SimpleLogger(){
}
void SimpleLogger::error(const char* msg,...){
	if(m_level>=ERROR_LEVEL&&m_isPrint){
		va_list ap;
		va_start(ap,msg);
		vprintf(msg,ap);
		va_end(ap);
	}
}
void SimpleLogger::info(const char* msg,...){
	if(m_level>=INFO_LEVEL&&m_isPrint){
		va_list ap;
		va_start(ap,msg);
		vprintf(msg,ap);
		va_end(ap);
	}
}
void SimpleLogger::debug(const char* msg,...){
	if(m_level>=DEBUG_LEVEL&&m_isPrint){
		va_list ap;
		va_start(ap,msg);
		vprintf(msg,ap);
		va_end(ap);
	}
}
bool SimpleLogger::existExcludeClassName(){
	set<string>::iterator it;
	for(it=m_excludeClassNames.begin();it!=m_excludeClassNames.end();it++){
		if(strncmp((*it).c_str(),m_className.c_str(),(*it).size())==0){
			return true;
		}
	}
    return false;
}
/*TimeUtils*/
#ifdef WIN32
	ULARGE_INTEGER TimeUtils::m_ullUtcTime;
	bool TimeUtils::m_initUtcTime=false;
	LARGE_INTEGER TimeUtils::m_Freq;
	bool TimeUtils::m_initFreq=false;
#endif
UInt64 TimeUtils::currentTimeMillis(){
#ifndef WIN32
	timeval now;
	gettimeofday(&now,0);
	UInt64 time=(UInt64)now.tv_sec*1000+now.tv_usec/1000;
	return time;
#else
	SYSTEMTIME stSysTime;
	FILETIME stFileTime;
	ULARGE_INTEGER ullSystime;
	if(!m_initUtcTime){
		m_initUtcTime=true;
		stSysTime.wYear = 1970;
        stSysTime.wMonth = 1;
        stSysTime.wDay = 1;
        stSysTime.wHour = 0;
        stSysTime.wMinute = 0;
        stSysTime.wSecond = 0;
        stSysTime.wMilliseconds = 0;
		SystemTimeToFileTime(&stSysTime,&stFileTime);
        memcpy((void*)&m_ullUtcTime,(void*)&stFileTime,sizeof(stFileTime));
	}
	GetSystemTime(&stSysTime);
    SystemTimeToFileTime(&stSysTime,&stFileTime);
    memcpy(&ullSystime,&stFileTime,sizeof(stFileTime));
	return (ULONGLONG)(ullSystime.QuadPart - m_ullUtcTime.QuadPart)/10000;
#endif
}
#ifndef WIN32
UInt64 TimeUtils::microsecondTime(timeval lastTime){
	timeval now;
	gettimeofday(&now,0);
	return (UInt64)(now.tv_sec-lastTime.tv_sec)*1000000+(now.tv_usec-lastTime.tv_usec);
}
#else
UInt64 TimeUtils::microsecondTime(LARGE_INTEGER lastTime){
	if(!m_initFreq){
		m_initFreq=true;
		QueryPerformanceFrequency(&m_Freq);
	}
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return (time.QuadPart-lastTime.QuadPart)*1000000ULL/m_Freq.QuadPart;
}
#endif
/*ByteUtils实现*/
ByteUtils::ByteUtils(){
}
void ByteUtils::writeUInt32(char* dest,int start,UInt32 value){
	char m4=(char)(value>>24);
	char m3=(char)(value>>16);
	char m2=(char)(value>>8);
	char m1=(char)(value);
	char temp[]={m4,m3,m2,m1};
	memcpy(dest+start,temp,4);
}
UInt32 ByteUtils::readUInt32(const char* src,int start){
	UInt32 result =0;
	result = (UInt32)(src[start]&0xFF)<<24
			|(src[start+1]&0xFF)<<16
			|(src[start+2]&0xFF)<<8
			|(src[start+3]&0xFF);
	return result;
}
void ByteUtils::writeUInt64(char* dest,int start,UInt64 value){
	char m8=(char)(value>>56);
	char m7=(char)(value>>48);
	char m6=(char)(value>>40);
	char m5=(char)(value>>32);
	char m4=(char)(value>>24);
	char m3=(char)(value>>16);
	char m2=(char)(value>>8);
	char m1=(char)(value);
	char temp[]={m8,m7,m6,m5,m4,m3,m2,m1};
	memcpy(dest+start,temp,8);
}
UInt64 ByteUtils::readUInt64(const char* src,int start){
	UInt64 result =0;
	result = (UInt64)(src[start+0]&0xFF)<<56
			|(UInt64)(src[start+1]&0xFF)<<48
			|(UInt64)(src[start+2]&0xFF)<<40
			|(UInt64)(src[start+3]&0xFF)<<32
			|(UInt64)(src[start+4]&0xFF)<<24
			|(src[start+5]&0xFF)<<16
			|(src[start+6]&0xFF)<<8
			|(src[start+7]&0xFF);
	return result;
}
/*SimpleObject实现*/
SimpleObject::SimpleObject():m_mutex(),m_cond(){
#ifndef WIN32
	pthread_mutex_init(&m_mutex,NULL);
	pthread_cond_init(&m_cond,NULL);
#else
	m_mutex = CreateMutex(NULL, false, NULL);
    m_cond = CreateEvent(NULL, false, false, NULL);
#endif
}
SimpleObject::~SimpleObject(){
#ifndef WIN32
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_cond);
#else
	CloseHandle(m_mutex);
    CloseHandle(m_cond);
#endif
}
void SimpleObject::wait(){
#ifndef WIN32
	pthread_mutex_lock(&m_mutex);
	pthread_cond_wait(&m_cond,&m_mutex);
	pthread_mutex_unlock(&m_mutex);
#else
	WaitForSingleObject(m_mutex,INFINITE);
	WaitForSingleObject(m_cond,INFINITE);
	ReleaseMutex(m_mutex);
#endif
}
void SimpleObject::wait(UInt32 time){
#ifndef WIN32
	UInt32 secondTime=time/1000;
	UInt32 millisecondTime=time-secondTime*1000;
	timeval now;
	timespec timeout;
	gettimeofday(&now,0);
	if((now.tv_usec+millisecondTime*1000)>=1000000){
		timeout.tv_sec =now.tv_sec+secondTime+1;
		timeout.tv_nsec =(now.tv_usec+millisecondTime*1000-1000000)*1000;
	}else{
		timeout.tv_sec =now.tv_sec+secondTime;
		timeout.tv_nsec =(now.tv_usec+millisecondTime*1000)*1000;
	}
	pthread_mutex_lock(&m_mutex);
	pthread_cond_timedwait(&m_cond,&m_mutex,&timeout);
	pthread_cond_signal(&m_cond);
	pthread_mutex_unlock(&m_mutex);
#else
	WaitForSingleObject(m_mutex,INFINITE);
	WaitForSingleObject(m_cond,time);
	ReleaseMutex(m_mutex);
#endif
}
void SimpleObject::notify(){
#ifndef WIN32
	pthread_mutex_lock(&m_mutex);
	pthread_cond_signal(&m_cond);
	pthread_mutex_unlock(&m_mutex);
#else
	SetEvent(m_cond);
#endif
}
/*SimpleThread实现*/
SimpleThread::SimpleThread():m_thread(),m_pR(NULL),m_logger("common.SimpleThread"){
#ifdef WIN32
	m_ExitCond = CreateEvent(NULL, false, false, NULL);
#endif
}

SimpleThread::SimpleThread(Runnable* pR):m_thread(),m_pR(pR),m_logger("common.SimpleThread"){
#ifdef WIN32
	m_ExitCond = CreateEvent(NULL, false, false, NULL);
#endif
}

SimpleThread::~SimpleThread(){
#ifdef WIN32
	CloseHandle(m_ExitCond);
#endif
}

void SimpleThread::start(){
#ifndef WIN32
	int ret;
	ret=pthread_create(&m_thread,NULL,threadFun,this);
	if(ret!=0){
		string errorMsg="create thread failure:"+string(strerror(errno));
#else
	DWORD threadID;
    m_thread = CreateThread(NULL,0,threadFun,this,0,&threadID);
	if (NULL == m_thread){
		string errorMsg="create thread failure";
#endif
		PTException ex(PTException::CREATE_THREAD_ERROR,errorMsg);
		throw ex;
	}
}

#ifndef WIN32
   void* SimpleThread::threadFun(void* args){
#else
   DWORD WINAPI SimpleThread::threadFun(LPVOID args){
#endif
	SimpleThread* pSt=(SimpleThread*)args;
	Runnable* pR=pSt->m_pR;
	if(pR!=NULL){
		pR->run();
	}else{
		pSt->run();
	}
#ifndef WIN32
	pthread_exit(NULL);
#else
	SetEvent(pSt->m_ExitCond);
    return 0;
#endif
}

void SimpleThread::run(){
}

void SimpleThread::waitForEnd(){
#ifndef WIN32
	pthread_join(m_thread,NULL);
#else
	WaitForSingleObject(m_ExitCond,INFINITE);
#endif
}

void SimpleThread::cancel(){
#ifndef WIN32
    pthread_cancel(m_thread);
#else
	TerminateThread(m_thread,0);
#endif
}
/*RepeatWork实现*/
RepeatWork::RepeatWork(DoRepeatWork* pDrw,SimpleLogger* pLogger):Runnable(),SimpleObject(),
	m_IsStop(true),m_pThread(NULL),m_pDrw(pDrw),m_pLogger(pLogger){
}
RepeatWork::~RepeatWork(){
	if(m_pThread!=NULL){
		delete m_pThread;
	}
}
void RepeatWork::start(){
	if(m_IsStop){
		m_IsStop=false;
		m_pThread=new SimpleThread(this);
		m_pThread->start();
	}
}
void RepeatWork::stop(){
	if(!m_IsStop){
		m_IsStop=true;
		SimpleObject::notify();//唤醒
	}
}
void RepeatWork::waitForEnd(){
	m_pThread->waitForEnd();
}
void RepeatWork::run(){
	printMsg("start");
	m_pDrw->onStart();
	while(!m_IsStop){
		m_pDrw->doRepeatWork();
	}
	printMsg("end");
	m_pDrw->onStop();
}
void RepeatWork::printMsg(string msg){
	string className;
	m_pLogger->getClassName(className);
	msg=className+":"+msg;
	m_pLogger->info("%s\n",msg.c_str());
}
/*SimpleLock实现*/
SimpleLock::SimpleLock():m_mutex(){
#ifndef WIN32
	pthread_mutex_init(&m_mutex,NULL);
#else
	m_mutex = CreateMutex(NULL, false, NULL);
#endif
}

SimpleLock::~SimpleLock(){
#ifndef WIN32
	pthread_mutex_destroy(&m_mutex);
#else
	CloseHandle(m_mutex);
#endif
}

void SimpleLock::lock(){
#ifndef WIN32
	pthread_mutex_lock(&m_mutex);
#else
	WaitForSingleObject(m_mutex,INFINITE);
#endif
}

void SimpleLock::unlock(){
#ifndef WIN32
	pthread_mutex_unlock(&m_mutex);
#else
	ReleaseMutex(m_mutex);
#endif
}
/*ThreadPool实现*/
ThreadPool::ThreadPool(int initSize,int maxSize):m_initSize(initSize),m_maxSize(maxSize),m_size(0),m_idle_size(0),m_tasks(-1),m_invokes(),m_invoke_lock(){
	init();
}
ThreadPool::~ThreadPool(){
}
void ThreadPool::init(){
	int i;
	for(i=0;i<m_initSize;i++){
		addInvoke();
	}
}
void ThreadPool::addInvoke(){
	m_invoke_lock.lock();
	ExecuteTask* pEt=new ExecuteTask(this);
	m_invokes.push_back(pEt);
	pEt->start();
	m_size++;
	m_idle_size++;
	m_invoke_lock.unlock();
}
void ThreadPool::execute(Runnable* pR){
	m_tasks.put(pR);
	if(m_size<m_maxSize){
		if(m_tasks.size()>=m_idle_size||m_idle_size<=1){
			addInvoke();
		}
	}
}
void ThreadPool::clear(){
	Runnable* pR;
	while(!m_tasks.isEmpty()){
		m_tasks.get(pR);
		delete pR;
	}
}
void ThreadPool::shutdown(){
	m_invoke_lock.lock();
	for(list<ExecuteTask*>::iterator it=m_invokes.begin();it!=m_invokes.end();it++){
		((ExecuteTask*)*it)->shutdown();
	}
	m_invoke_lock.unlock();
	Runnable* pR;
	while(!m_tasks.isEmpty()){
		m_tasks.get(pR);
		delete pR;
	}
}
void ThreadPool::shutdownNow(){
	m_invoke_lock.lock();
	for(list<ExecuteTask*>::iterator it=m_invokes.begin();it!=m_invokes.end();it++){
		((ExecuteTask*)*it)->shutdownNow();
	}
	m_invoke_lock.unlock();
	Runnable* pR;
	while(!m_tasks.isEmpty()){
		m_tasks.get(pR);
		delete pR;
	}
}
/*ExecuteTask实现*/
ExecuteTask::ExecuteTask(ThreadPool* pTp):m_is_idle(true),m_is_stop(true){
    m_pTp=pTp;
}
ExecuteTask::~ExecuteTask(){
}
void ExecuteTask::run(){
	m_is_stop=false;
	Runnable* pR;
	while(!m_is_stop){
		m_is_idle=true;
		m_pTp->m_tasks.get(pR);//获取任务,该方法可能会阻塞
		m_pTp->m_invoke_lock.lock();
		m_pTp->m_idle_size--;
		m_pTp->m_invoke_lock.unlock();
		m_is_idle=false;
		pR->run();//执行任务
		delete pR;//删除任务对象
		m_pTp->m_invoke_lock.lock();
		m_pTp->m_idle_size++;
		m_pTp->m_invoke_lock.unlock();
		if(m_pTp->m_size>m_pTp->m_initSize&&m_pTp->m_tasks.size()<m_pTp->m_idle_size){//回收当前线程
			m_pTp->m_invoke_lock.lock();
			m_is_stop=true;
			m_pTp->m_invokes.remove(this);
			m_pTp->m_size--;
			m_pTp->m_idle_size--;
			m_pTp->m_invoke_lock.unlock();
			break;
		}
	}
	delete this;
}
void ExecuteTask::shutdown(){
	if(m_is_idle){
		SimpleThread::cancel();
		if(!m_is_stop){
			delete this;
		}
	}else{
		m_is_stop=true;//等任务跑完后自己停止
	}
}
void ExecuteTask::shutdownNow(){
	SimpleThread::cancel();
	if(!m_is_stop){
		delete this;
	}
}
/*CoreExecutorService实现*/
CoreExecutorService* CoreExecutorService::m_pInstance=NULL;
const int CoreExecutorService::INIT_THREAD_SIZE=__THREAD_POOL_INIT_SIZE;
const int CoreExecutorService::MAX_THREAD_SIZE=__THREAD_POOL_MAX_SIZE;
CoreExecutorService::CoreExecutorService():m_tp(INIT_THREAD_SIZE,MAX_THREAD_SIZE){
}
CoreExecutorService::~CoreExecutorService(){
}
void CoreExecutorService::newInstance(){
	if(m_pInstance==NULL){
		m_pInstance=new CoreExecutorService();
	}else{
		m_pInstance->m_tp.clear();
	}
}
CoreExecutorService* CoreExecutorService::getInstance(){
	return m_pInstance;
}
void CoreExecutorService::execute(Runnable* pR){
	m_tp.execute(pR);
}
void CoreExecutorService::shutdown(){
	m_tp.shutdown();
	delete this;
	m_pInstance=NULL;
}
/*PeerInfo*/
PeerInfo::PeerInfo():m_wanHost(0),m_wanPort(0),m_localHost(0),m_localPort(0){
}
PeerInfo::PeerInfo(UInt32 wanHost,UInt32 wanPort,UInt32 localHost,UInt32 localPort):
	m_wanHost(wanHost),m_wanPort(wanPort),m_localHost(localHost),m_localPort(localPort){
}
PeerInfo::PeerInfo(const string& peerId):m_wanHost(0),m_wanPort(0),m_localHost(0),m_localPort(0){
	sscanf(peerId.c_str(),"%08X%08X%08X%08X",&m_wanHost,&m_wanPort,&m_localHost,&m_localPort);
}
PeerInfo::PeerInfo(const PeerInfo& obj){
	m_wanHost=obj.m_wanHost;
	m_wanPort=obj.m_wanPort;
	m_localHost=obj.m_localHost;
	m_localPort=obj.m_localPort;
}
PeerInfo::~PeerInfo(){
}
const PeerInfo& PeerInfo::operator=(const PeerInfo& obj){
	m_wanHost=obj.m_wanHost;
	m_wanPort=obj.m_wanPort;
	m_localHost=obj.m_localHost;
	m_localPort=obj.m_localPort;
	return *this;
}
bool PeerInfo::operator==(const PeerInfo& obj){
	if(m_wanHost==obj.m_wanHost&&m_wanPort==obj.m_wanPort){
		return true;
	}
	return false;
}
void PeerInfo::toPeerId(string& peerId){
	char temp[33];
	sprintf(temp,"%08X%08X%08X%08X",m_wanHost,m_wanPort,m_localHost,m_localPort);
	temp[32]='\0';
	peerId=string(temp);
}
void PeerInfo::printInfo(){
	string wanIp,localIp;
	CommonUtils::toIpStr(m_wanHost,wanIp);
	CommonUtils::toIpStr(m_localHost,localIp);
	SimpleLogger logger("common.PeerInfo");
	logger.info("PeerInfo{wanHost=%s,wanPort=%d,localHost=%s,localPort=%d}\n",wanIp.c_str(),m_wanPort,localIp.c_str(),m_localPort);
}
