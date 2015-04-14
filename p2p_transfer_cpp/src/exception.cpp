#include "pt_api.h"
using namespace PT;
const int PTException::CREATE_THREAD_ERROR=1;
const int PTException::CREATE_SOCKET_ERROR=2;
const int PTException::BIND_SOCKET_ERROR=3;
PTException::PTException(int code,const string& errorMsg):m_code(code),m_errorMsg(errorMsg){
}
PTException::PTException(const PTException& obj){
	m_code=obj.m_code;
	m_errorMsg=obj.m_errorMsg;
}
PTException::~PTException(){
}
int PTException::getCode(){
	return m_code;
}
void PTException::getErrorMsg(string& errorMsg){
	errorMsg=m_errorMsg;
}
