#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "pt_api.h"
#include "common.h"
#include "endpoint.h"
using namespace std;
using namespace PT;
class MyEndPoint:public EndPoint{
public:
	MyEndPoint():EndPoint(PT::toIpInt("192.168.47.129"),6061){
		EndPoint::start();
	}
	~MyEndPoint(){
		cout<<"~MyEndPoint"<<endl;
	}
	virtual void receiveData(char* data,int length,UInt32 host,UInt32 port){
        cout<<data<<endl;
	}
};
class MyThread:public SimpleThread{
public:
	MyThread(MyEndPoint* pMep):SimpleThread(),m_pMep(pMep){
		SimpleThread::start();
	}
	virtual void run(){
		sleep(5);
		m_pMep->shutdown();
	}
private:
	MyEndPoint* m_pMep;
};
int main(int argc,char* argv[]) {
	MyEndPoint mep;
	sleep(1);
	MyThread my(&mep);
	mep.waitForEnd();
	return 0;
}
