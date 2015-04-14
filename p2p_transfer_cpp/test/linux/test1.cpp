#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "pt_api.h"
using namespace std;

PT::PTServer* pServer=NULL;
void* threadFun(void*);

class MyHandler:public PT::PTServerHandler{
public:
	MyHandler():m_out(),m_in("cdt-master-8.1.2.zip"),m_initOut(false){
	}
	~MyHandler(){
	}
	virtual void onPeerLogin(const PT::PeerInfo& peerInfo){
		PT::PeerInfo pi=peerInfo;
		cout<<"onPeerLogin:";pi.printInfo();
		//testGetNatType(peerInfo);
		//testSendData(peerInfo);
	}
	virtual void onPeerLogout(const PT::PeerInfo& peerInfo){
		PT::PeerInfo pi=peerInfo;
		cout<<"onPeerLogout:";pi.printInfo();
	}
	virtual void onPeerData(char* data,int len,const PT::PeerInfo& peerInfo){
		cout<<"onPeerData:"<<endl;
		//testSaveData(data,len);
	}
	virtual void onDisconnectToPeer(const PT::PeerInfo& peerInfo){
		PT::PeerInfo pi=peerInfo;
		cout<<"onDisconnectToPeer:";pi.printInfo();
	}
public:
	void testSendData(const PT::PeerInfo& peerInfo){
		sleep(1);
		char buf[1024];
		int len=0;
		do{
			m_in.read(buf,sizeof(buf));
			len=m_in.gcount();
			if(len>0){
				pServer->sendDataToPeer(buf,len,peerInfo);
			}
		}while(!m_in.eof());
	}
	void testSaveData(char* data,int len){
		if(!m_initOut){
			m_initOut=true;
			m_out.open("test.zip");
		}
		m_out.write(data,len);
		m_out<<flush;
	}
	void testGetNatType(const PT::PeerInfo& peerInfo){
		sleep(5);
		int natType=pServer->getNatType(peerInfo);
		cout<<"natType:"<<natType<<endl;
	}
	void testShutdown(){
		pthread_t thread;
		pthread_create(&thread,NULL,threadFun,NULL);
	}
private:
	ofstream m_out;
	ifstream m_in;
	bool m_initOut;
};
void* threadFun(void* args){
	sleep(5);
	pServer->shutdown();
	pthread_exit(NULL);
}
void test(){
	try{
		pServer=PT::createServer(true);
		MyHandler* pHandler=new MyHandler();
		pServer->setHandler(pHandler);
		pServer->start(PT::toIpInt("121.40.86.143"),6061,6062,6063,6064);
		//pHandler->testShutdown();
		pServer->waitForEnd();
		PT::destroyServer(pServer);
		delete pHandler;
		cout<<"end................"<<endl;
	}catch (PT::PTException ex) {
		string errorMsg;
		ex.getErrorMsg(errorMsg);
		cout<<errorMsg<<endl;
	}
}
int main(int argc,char* argv[]) {
	test();
	return 0;
}
