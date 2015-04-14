#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "pt_api.h"
using namespace std;

void* threadFun(void* args);
PT::PTGroupServer* pGroupServer;
void testShutdown(){
	pthread_t thread;
	pthread_create(&thread,NULL,threadFun,NULL);
}
void* threadFun(void* args){
	sleep(5);
    pGroupServer->shutdown();
    pthread_exit(NULL);
}
int main(int argc,char* argv[]) {
	pGroupServer=PT::createGroupServer(true);
	pGroupServer->start(PT::toIpInt("192.168.1.105"),6061,6062,6063,6064);
	//testShutdown();
	pGroupServer->waitForEnd();
	cout<<"end............."<<endl;
	return 0;
}
