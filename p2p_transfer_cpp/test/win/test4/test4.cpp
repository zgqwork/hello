// test4.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "../../../src/pt_api.h"
#include <Windows.h>
#include <iostream>
#pragma comment(lib,"../../../win/vc11/Debug/ptlib.lib")
using namespace std;

DWORD WINAPI threadFun(LPVOID param);
PT::PTGroup* pGroup;
class MyHandler:public PT::PTGroupHandler{
public:
	MyHandler(){}
	virtual void onLoginServerSuccess(){
		cout<<"onLoginServerSuccess"<<endl;
		pGroup->joinGroup();
		//testShutdown();
		//testSendDataToAll();
		//testAddHaveData(19,30);
		//testAddWantData(5);
	}
	virtual void onLoginServerFailure(){
		cout<<"onLoginServerFailure"<<endl;
		//testShutdown();
	}
	virtual void onDisconnectToServer(){
		cout<<"onDisconnectToServer"<<endl;
	}
	virtual void onLoginPeerSuccess(const PT::PeerInfo& peerInfo){
		PT::PeerInfo pi=peerInfo;
		cout<<"onLoginPeerSuccess:";pi.printInfo();
	}
	virtual void onLoginPeerFailure(const PT::PeerInfo& peerInfo){
		cout<<"onLoginPeerFailure"<<endl;
	}
	virtual void onPeerLogin(const PT::PeerInfo& peerInfo){
		PT::PeerInfo pi=peerInfo;
		cout<<"onPeerLogin:";pi.printInfo();
	}
	virtual void onPeerLogout(const PT::PeerInfo& peerInfo){
		PT::PeerInfo pi=peerInfo;
		cout<<"onPeerLogout:";pi.printInfo();
	}
	virtual void onDisconnectToPeer(const PT::PeerInfo& peerInfo){
		PT::PeerInfo pi=peerInfo;
		cout<<"onDisconnectToPeer:";pi.printInfo();
	}
	virtual void onData(const PT::PeerInfo& peerInfo,char* data,int length){
		cout<<"onData:"<<data<<endl;
	}
	virtual void onWantDataFailure(int index){
		cout<<"onWantDataFailure"<<endl;
	}
	virtual void onRequestData(const PT::PeerInfo& peerInfo,int index){
		cout<<"onRequestData"<<endl;
		//testWriteRequestedData(peerInfo,index);
	}
	virtual void onFetchData(const PT::PeerInfo& peerInfo,int index,char* data,int length){
		char* temp=new char[length+1];
		memcpy(temp,data,length);
		temp[length]='\0';
		cout<<"onFetchData:"<<temp<<endl;
	}
public:
	void testShutdown(){
		DWORD threadID;
        HANDLE thread = CreateThread(NULL,0,threadFun,NULL,0,&threadID);
	}
	void testSendDataToPeer(const PT::PeerInfo& peerInfo){
		char data[]="test111";
		pGroup->sendDataToPeer(data,8,peerInfo);
	}
	void testSendDataToAll(){
		Sleep(8000);
		char data[]="test111";
		pGroup->sendDataToAll(data,8);
	}
	void testAddHaveData(int firstIndex,int lastIndex){
		Sleep(5000);
		pGroup->addHaveData(firstIndex,lastIndex);
	}
	void testRemoveHaveData(int firstIndex,int lastIndex){
		pGroup->removeHaveData(firstIndex,lastIndex);
	}
	void testExistData(int index){
		bool exist=pGroup->existData(index);
		cout<<"exist"<<exist<<endl;
	}
	void testAddWantData(int index){
		Sleep(5000);
		pGroup->addWantData(index);
	}
	void testWriteRequestedData(const PT::PeerInfo& peerInfo,int index){
		char data[]="test111";
		pGroup->writeRequestedData(peerInfo,index,data,8);
	}
};
DWORD WINAPI threadFun(LPVOID param){
	Sleep(5000);
    pGroup->shutdown();
	return 0;
}
void test(){
	pGroup=PT::createGroup("p2p.demo.test_group",true);
	MyHandler handler;
	pGroup->setHandler(&handler);
	pGroup->start(PT::toIpInt("192.168.1.103"),6061);
	//pGroup->start(PT::toIpInt("192.168.1.105"),12345,PT::toIpInt("192.168.1.105"),6061);
	//handler.testShutdown();
	pGroup->waitForEnd();
}

int _tmain(int argc, _TCHAR* argv[])
{
	test();
	return 0;
}

