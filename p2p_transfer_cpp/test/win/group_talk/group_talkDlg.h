
// group_talkDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "../../../src/pt_api.h"
#pragma comment(lib,"../../../win/vc11/Debug/ptlib.lib")


// Cgroup_talkDlg 对话框
class Cgroup_talkDlg : public CDialogEx,PT::PTGroupHandler
{
// 构造
public:
	Cgroup_talkDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GROUP_TALK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_txtMsg;
	CEdit m_txtInfo;
	CEdit m_txtGroupName;
	CButton m_btnSend;
	CButton m_btnClose;
	CButton m_btnJoinGroup;
	afx_msg void OnClickedJoinGroup();
	afx_msg void OnClickedClose();
	afx_msg void OnClickedSend();
	PT::PTGroup* m_pGroup;
	virtual void onLoginServerSuccess();
	virtual void onLoginServerFailure();
	virtual void onDisconnectToServer();
	virtual void onLoginPeerSuccess(const PT::PeerInfo& peerInfo);
	virtual void onLoginPeerFailure(const PT::PeerInfo& peerInfo);
	virtual void onPeerLogin(const PT::PeerInfo& peerInfo);
	virtual void onPeerLogout(const PT::PeerInfo& peerInfo);
	virtual void onDisconnectToPeer(const PT::PeerInfo& peerInfo);
	virtual void onData(const PT::PeerInfo& peerInfo,char* data,int length);
	virtual void onWantDataFailure(int index);
	virtual void onRequestData(const PT::PeerInfo& peerInfo,int index);
	virtual void onFetchData(const PT::PeerInfo& peerInfo,int index,char* data,int length);
	void appendMsg(CString msg);
	void appendMsg2(string msg);
};
