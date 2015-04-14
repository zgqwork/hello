// test2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../../../src/pt_api.h"
#include <Windows.h>
#include <iostream>
#pragma comment(lib,"../../../win/vc11/Debug/ptlib.lib")
using namespace std;

PT::PTPeer* pPeer=NULL;
DWORD WINAPI threadFun(LPVOID param);

class MyHandler:public PT::PTPeerHandler{
public:
	MyHandler(){
	}
	void onLoginServerSuccess(const PT::PeerInfo& peerInfo){
		cout<<"onLoginServerSuccess"<<endl;
		//testSendData(NULL,true);
		//testLoginPeer();
		//testLogoutAllAndShutdown();
	}
	void onLoginServerFailure(){
		cout<<"onLoginServerFailure"<<endl;
		//testShutdown();
	}
	void onServerData(char* data,int len){
		cout<<"onServerData"<<endl;
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
	}
	void onDisconnectToPeer(const PT::PeerInfo& peerInfo){
		cout<<"onDisconnectToPeer"<<endl;
	}
public:
	void testSendData(PT::PeerInfo* pPeerInfo,bool isSendServer){
		char buf[10240];
		int len=10240;
		int i=0;
		do{
			if(isSendServer){
				pPeer->sendDataToServer(buf,len);
			}else{
				pPeer->sendDataToPeer(buf,len,*pPeerInfo);
			}
			i++;
		}while(i<20000);
	}
	void testShutdown(){
		DWORD threadID;
        HANDLE thread = CreateThread(NULL,0,threadFun,this,0,&threadID);
	}
	void testLogoutServerAndShutdown(){
		Sleep(10000);
		pPeer->logoutServerAndShutdown();
	}
	void testLoginPeer(){
		PT::PeerInfo peerInfo(PT::toIpInt("192.168.1.103"),50544,PT::toIpInt("192.168.1.103"),50544);
		pPeer->loginPeer(peerInfo);
		//pPeer->loginPeer(peerInfo,PT::TURN);
		//pPeer->loginPeer(peerInfo,PT::P2P,2000);
	}
	void testLogoutPeer(const PT::PeerInfo& peerInfo){
		Sleep(5000);
		pPeer->logoutPeer(peerInfo);
	}
	void testLogoutAllAndShutdown(){
		Sleep(10000);
		pPeer->logoutAllAndShutdown();
	}
	void testGetLocalPeerInfo(){
		PT::PeerInfo peerInfo;
		pPeer->getLocalPeerInfo(peerInfo);
		peerInfo.printInfo();
	}
};
DWORD WINAPI threadFun(LPVOID param){
	Sleep(5000);
	pPeer->shutdown();
	return 0;
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

int _tmain(int argc, _TCHAR* argv[])
{
	test();
	return 0;
}

