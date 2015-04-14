// test1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "test1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "../../../src/pt_api.h"
#pragma comment(lib,"../../../win/vc11/Debug/ptlib.lib")

PT::PTServer* pServer=NULL;
DWORD WINAPI threadFun(LPVOID param);

class MyHandler:public PT::PTServerHandler{
public:
	MyHandler():m_file(),m_initFile(false){
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
		testSaveData(data,len);
	}
	virtual void onDisconnectToPeer(const PT::PeerInfo& peerInfo){
		PT::PeerInfo pi=peerInfo;
		cout<<"onDisconnectToPeer:";pi.printInfo();
	}
public:
	void testSendData(const PT::PeerInfo& peerInfo){
		Sleep(1000);
		char buf[10240];
		int len=10240;
		int i=0;
		do{
			pServer->sendDataToPeer(buf,len,peerInfo);
			i++;
		}while(i<30000);
	}
	void testSaveData(char* data,int len){
		if(!m_initFile){
			m_initFile=true;
			m_file.Open("test.zip",CFile::modeCreate|CFile::modeWrite);
		}
		m_file.Write(data,len);
		//m_file.Flush();
	}
	void testGetNatType(const PT::PeerInfo& peerInfo){
		Sleep(5000);
		int natType=pServer->getNatType(peerInfo);
		cout<<"natType:"<<natType<<endl;
	}
	void testShutdown(){
		DWORD threadID;
        HANDLE thread = CreateThread(NULL,0,threadFun,this,0,&threadID);
	}
private:
	bool m_initFile;
	CFile m_file;
};
DWORD WINAPI threadFun(LPVOID param){
	Sleep(5000);
	pServer->shutdown();
	return 0;
}
void test(){
	try{
		pServer=PT::createServer(true);
		MyHandler* pHandler=new MyHandler();
		pServer->setHandler(pHandler);
		pServer->start(PT::toIpInt("192.168.1.103"),6061,6062,6063,6064);
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

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 更改错误代码以符合您的需要
			_tprintf(_T("错误: MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: 在此处为应用程序的行为编写代码。
			test();
		}
	}
	else
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: GetModuleHandle 失败\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
