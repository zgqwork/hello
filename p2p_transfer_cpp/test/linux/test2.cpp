#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "pt_api.h"
using namespace std;

PT::PTPeer* pPeer=NULL;
void* threadFun(void*);

class MyHandler:public PT::PTPeerHandler{
public:
	MyHandler():m_out(),m_in("cdt-master-8.1.2.zip"),m_initOut(false){
	}
	void onLoginServerSuccess(const PT::PeerInfo& peerInfo){
		cout<<"onLoginServerSuccess loginMode:"<<pPeer->getLoginMode(peerInfo)<<endl;
		//testLogoutAllAndShutdown();
		//testSendData(NULL,true);
		testLoginPeer();
		//testLogoutAllAndShutdown();
	}
	void onLoginServerFailure(){
		cout<<"onLoginServerFailure"<<endl;
		//testShutdown();
	}
	void onServerData(char* data,int len){
		cout<<"onServerData"<<endl;
		//testSaveData(data,len);
	}
	void onDisconnectToServer(){
		cout<<"onDisconnectToServer"<<endl;
	}
	void onLoginPeerSuccess(const PT::PeerInfo& peerInfo){
		PT::PeerInfo pi=peerInfo;
		cout<<"onLoginPeerSuccess"<<endl;
		//testSendData(&pi,false);
		//testLogoutPeer(peerInfo);
	}
	void onLoginPeerFailure(const PT::PeerInfo& peerInfo,int loginMode){
		cout<<"onLoginPeerFailure"<<endl;
	}
	void onPeerLogin(const PT::PeerInfo& peerInfo){
		cout<<"onPeerLogin"<<endl;
		//testSendData(&peerInfo,false);
	}
	void onPeerLogout(const PT::PeerInfo& peerInfo){
		cout<<"onPeerLogout"<<endl;
	}
	void onPeerData(char* data,int len,const PT::PeerInfo& peerInfo){
		cout<<"onPeerData"<<endl;
		//testSaveData(data,len);
	}
	void onDisconnectToPeer(const PT::PeerInfo& peerInfo){
		cout<<"onDisconnectToPeer"<<endl;
	}
public:
	void testSendData(PT::PeerInfo* pPeerInfo,bool isSendServer){
		char buf[10240];
		int len=0;
		do{
			m_in.read(buf,sizeof(buf));
			len=m_in.gcount();
			if(len>0){
				if(isSendServer){
					pPeer->sendDataToServer(buf,len);
				}else{
					pPeer->sendDataToPeer(buf,len,*pPeerInfo);
				}
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
	void testShutdown(){
		pthread_t thread;
		pthread_create(&thread,NULL,threadFun,NULL);
	}
	void testLogoutServerAndShutdown(){
		sleep(10);
		pPeer->logoutServerAndShutdown();
	}
	void testLoginPeer(){
		PT::PeerInfo peerInfo(PT::toIpInt("192.168.1.103"),57376,PT::toIpInt("192.168.1.103"),57376);
		pPeer->loginPeer(peerInfo);
		//pPeer->loginPeer(peerInfo,PT::TURN);
		//pPeer->loginPeer(peerInfo,PT::P2P,2000);
	}
	void testLogoutPeer(const PT::PeerInfo& peerInfo){
		sleep(5);
		pPeer->logoutPeer(peerInfo);
	}
	void testLogoutAllAndShutdown(){
		sleep(10);
		pPeer->logoutAllAndShutdown();
	}
	void testGetLocalPeerInfo(){
		PT::PeerInfo peerInfo;
		pPeer->getLocalPeerInfo(peerInfo);
		peerInfo.printInfo();
	}
private:
	ofstream m_out;
	ifstream m_in;
	bool m_initOut;
};
void* threadFun(void* args){
	sleep(5);
	pPeer->shutdown();
	pthread_exit(NULL);
}

void test(){
	try{
		pPeer=PT::createPeer(true);
		MyHandler handler;
		pPeer->setHandler(&handler);
		pPeer->start();
		pPeer->loginServer(PT::toIpInt("192.168.1.103"),6061);
		//pPeer->loginServer(PT::toIpInt("192.168.1.103"),6061,2000);
		//handler.testGetLocalPeerInfo();
		//handler.testShutdown();
		pPeer->waitForEnd();
		PT::destroyPeer(pPeer);
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
