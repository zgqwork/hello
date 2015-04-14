#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "pt_api.h"
using namespace std;
void* threadFun(void* args);
PT::PTGroup* pGroup;
class MyHandler:public PT::PTGroupHandler{
public:
	MyHandler(){}
	virtual void onLoginServerSuccess(){
		cout<<"onLoginServerSuccess"<<endl;
		pGroup->joinGroup();
		//testShutdown();
		//testSendDataToAll();
		testAddHaveData(19,30);
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
		testWriteRequestedData(peerInfo,index);
	}
	virtual void onFetchData(const PT::PeerInfo& peerInfo,int index,char* data,int length){
		cout<<"onFetchData:"<<data<<endl;
	}
public:
	void testShutdown(){
		pthread_t thread;
		pthread_create(&thread,NULL,threadFun,NULL);
	}
	void testSendDataToPeer(const PT::PeerInfo& peerInfo){
		char data[]="test111";
		pGroup->sendDataToPeer(data,8,peerInfo);
	}
	void testSendDataToAll(){
		sleep(8);
		char data[]="test111";
		pGroup->sendDataToAll(data,8);
	}
	void testAddHaveData(int firstIndex,int lastIndex){
		sleep(5);
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
		sleep(5);
		pGroup->addWantData(index);
	}
	void testWriteRequestedData(const PT::PeerInfo& peerInfo,int index){
		char data[]="test111";
		pGroup->writeRequestedData(peerInfo,index,data,8);
	}
};
void* threadFun(void* args){
	sleep(5);
    pGroup->shutdown();
    pthread_exit(NULL);
}
int main(int argc,char* argv[]) {
	pGroup=PT::createGroup("test_group",true);
	MyHandler handler;
	pGroup->setHandler(&handler);
	pGroup->start(PT::toIpInt("192.168.1.105"),6061);
	//pGroup->start(PT::toIpInt("192.168.1.105"),12345,PT::toIpInt("192.168.1.105"),6061);
	//handler.testShutdown();
	pGroup->waitForEnd();
	return 0;
}
