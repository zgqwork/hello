
// group_talkDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "group_talk.h"
#include "group_talkDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cgroup_talkDlg 对话框



Cgroup_talkDlg::Cgroup_talkDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cgroup_talkDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cgroup_talkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG, m_txtMsg);
	DDX_Control(pDX, IDC_INFO, m_txtInfo);
	DDX_Control(pDX, IDC_GROUPNAME, m_txtGroupName);
	DDX_Control(pDX, IDC_SEND, m_btnSend);
	DDX_Control(pDX, IDC_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_JOINGROUP, m_btnJoinGroup);
}

BEGIN_MESSAGE_MAP(Cgroup_talkDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_JOINGROUP, &Cgroup_talkDlg::OnClickedJoinGroup)
	ON_BN_CLICKED(IDC_CLOSE, &Cgroup_talkDlg::OnClickedClose)
	ON_BN_CLICKED(IDC_SEND, &Cgroup_talkDlg::OnClickedSend)
END_MESSAGE_MAP()


// Cgroup_talkDlg 消息处理程序

BOOL Cgroup_talkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_txtGroupName.SetWindowText("p2p.demo.test_group");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cgroup_talkDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cgroup_talkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cgroup_talkDlg::OnClickedJoinGroup()
{
	// TODO: 在此添加控件通知处理程序代码
	CString groupName;
	m_txtGroupName.GetWindowText(groupName);
	m_pGroup=PT::createGroup(groupName.GetString(),true);
	m_pGroup->setHandler(this);
	m_pGroup->start(PT::toIpInt("192.168.1.103"),6061);
	m_btnJoinGroup.EnableWindow(false);
}


void Cgroup_talkDlg::OnClickedClose()
{
	// TODO: 在此添加控件通知处理程序代码
	DestroyWindow();
}


void Cgroup_talkDlg::OnClickedSend()
{
	// TODO: 在此添加控件通知处理程序代码
	CString msg;
	m_txtMsg.GetWindowText(msg);
	m_pGroup->sendDataToAll((char*)msg.GetString(),msg.GetLength());
}
void Cgroup_talkDlg::onLoginServerSuccess(){
	appendMsg("登录服务器成功");
	m_pGroup->joinGroup();
}
void Cgroup_talkDlg::onLoginServerFailure(){
	appendMsg("登录服务器失败");
}
void Cgroup_talkDlg::onDisconnectToServer(){
	appendMsg("与服务器断开连接");
}
void Cgroup_talkDlg::onLoginPeerSuccess(const PT::PeerInfo& peerInfo){
	PT::PeerInfo pi=peerInfo;
	string peerId;
	pi.toPeerId(peerId);
	int loginMode=m_pGroup->getLoginMode(peerInfo);
	string sLoginMode;
	if(loginMode==PT::LAN){
		sLoginMode="LAN";
	}else if(loginMode==PT::P2P){
		sLoginMode="P2P";
	}else if(loginMode==PT::TURN){
		sLoginMode="TURN";
	}
	appendMsg2("与peer:"+peerId+"建立连接,连接模式:"+sLoginMode);
}
void Cgroup_talkDlg::onLoginPeerFailure(const PT::PeerInfo& peerInfo){
	appendMsg("onLoginPeerFailure");
}
void Cgroup_talkDlg::onPeerLogin(const PT::PeerInfo& peerInfo){
	PT::PeerInfo pi=peerInfo;
	string peerId;
	pi.toPeerId(peerId);
	int loginMode=m_pGroup->getLoginMode(peerInfo);
	string sLoginMode;
	if(loginMode==PT::LAN){
		sLoginMode="LAN";
	}else if(loginMode==PT::P2P){
		sLoginMode="P2P";
	}else if(loginMode==PT::TURN){
		sLoginMode="TURN";
	}
	appendMsg2("与peer:"+peerId+"建立连接,连接模式:"+sLoginMode);
}
void Cgroup_talkDlg::onPeerLogout(const PT::PeerInfo& peerInfo){
	PT::PeerInfo pi=peerInfo;
	string peerId;
	pi.toPeerId(peerId);
	appendMsg2("与peer:"+peerId+"断开连接");
}
void Cgroup_talkDlg::onDisconnectToPeer(const PT::PeerInfo& peerInfo){
	PT::PeerInfo pi=peerInfo;
	string peerId;
	pi.toPeerId(peerId);
	appendMsg2("与peer:"+peerId+"断开连接");
}
void Cgroup_talkDlg::onData(const PT::PeerInfo& peerInfo,char* data,int length){
	appendMsg("onData");
	CString temp(data,length);
	appendMsg(temp);
}
void Cgroup_talkDlg::onWantDataFailure(int index){
	appendMsg("onWantDataFailure");
}
void Cgroup_talkDlg::onRequestData(const PT::PeerInfo& peerInfo,int index){
	appendMsg("onRequestData");
}
void Cgroup_talkDlg::onFetchData(const PT::PeerInfo& peerInfo,int index,char* data,int length){
	appendMsg("onFetchData");
}
void Cgroup_talkDlg::appendMsg(CString msg){
	CString lastMsg;
	m_txtInfo.GetWindowText(lastMsg);
	lastMsg.Append(msg);
	lastMsg.Append("\r\n");
	m_txtInfo.SetWindowText(lastMsg);
}
void Cgroup_talkDlg::appendMsg2(string msg){
	CString lastMsg;
	m_txtInfo.GetWindowText(lastMsg);
	lastMsg.Append(msg.c_str());
	lastMsg.Append("\r\n");
	m_txtInfo.SetWindowText(lastMsg);
}