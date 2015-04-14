// test3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../../../src/pt_api.h"
#include <Windows.h>
#include <iostream>
#pragma comment(lib,"../../../win/vc11/Debug/ptlib.lib")
using namespace std;

DWORD WINAPI threadFun(LPVOID param);
PT::PTGroupServer* pGroupServer;
void testShutdown(){
	DWORD threadID;
    HANDLE thread = CreateThread(NULL,0,threadFun,NULL,0,&threadID);
}
DWORD WINAPI threadFun(LPVOID param){
	Sleep(5000);
    pGroupServer->shutdown();
	return 0;
}

void test(){
	pGroupServer=PT::createGroupServer(true);
	pGroupServer->start(PT::toIpInt("192.168.1.103"),6061,6062,6063,6064);
	//testShutdown();
	pGroupServer->waitForEnd();
	cout<<"end............."<<endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	test();
	return 0;
}

