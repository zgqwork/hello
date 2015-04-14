
// group_talkDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "group_talk.h"
#include "group_talkDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cgroup_talkDlg �Ի���



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


// Cgroup_talkDlg ��Ϣ�������

BOOL Cgroup_talkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_txtGroupName.SetWindowText("p2p.demo.test_group");
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cgroup_talkDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Cgroup_talkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cgroup_talkDlg::OnClickedJoinGroup()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString groupName;
	m_txtGroupName.GetWindowText(groupName);
	m_pGroup=PT::createGroup(groupName.GetString(),true);
	m_pGroup->setHandler(this);
	m_pGroup->start(PT::toIpInt("192.168.1.103"),6061);
	m_btnJoinGroup.EnableWindow(false);
}


void Cgroup_talkDlg::OnClickedClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DestroyWindow();
}


void Cgroup_talkDlg::OnClickedSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString msg;
	m_txtMsg.GetWindowText(msg);
	m_pGroup->sendDataToAll((char*)msg.GetString(),msg.GetLength());
}
void Cgroup_talkDlg::onLoginServerSuccess(){
	appendMsg("��¼�������ɹ�");
	m_pGroup->joinGroup();
}
void Cgroup_talkDlg::onLoginServerFailure(){
	appendMsg("��¼������ʧ��");
}
void Cgroup_talkDlg::onDisconnectToServer(){
	appendMsg("��������Ͽ�����");
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
	appendMsg2("��peer:"+peerId+"��������,����ģʽ:"+sLoginMode);
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
	appendMsg2("��peer:"+peerId+"��������,����ģʽ:"+sLoginMode);
}
void Cgroup_talkDlg::onPeerLogout(const PT::PeerInfo& peerInfo){
	PT::PeerInfo pi=peerInfo;
	string peerId;
	pi.toPeerId(peerId);
	appendMsg2("��peer:"+peerId+"�Ͽ�����");
}
void Cgroup_talkDlg::onDisconnectToPeer(const PT::PeerInfo& peerInfo){
	PT::PeerInfo pi=peerInfo;
	string peerId;
	pi.toPeerId(peerId);
	appendMsg2("��peer:"+peerId+"�Ͽ�����");
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