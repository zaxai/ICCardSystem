// CUserCloseCard.cpp: 实现文件
//

#include "stdafx.h"
#include "ICCardSystem.h"
#include "CUserCloseCard.h"
#include "afxdialogex.h"


// CUserCloseCard 对话框

IMPLEMENT_DYNAMIC(CUserCloseCard, CDialogEx)

CUserCloseCard::CUserCloseCard(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USERCLOSECARD, pParent)
	, m_nDeposit(0)
{

}

CUserCloseCard::~CUserCloseCard()
{
}

void CUserCloseCard::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ID, m_editID);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_IDNO, m_editIDNo);
	DDX_Control(pDX, IDC_EDIT_PHONENO, m_editPhoneNo);
	DDX_Control(pDX, IDC_EDIT_ADDR, m_editAddr);
	DDX_Control(pDX, IDC_EDIT_LICENSE, m_editLicense);
	DDX_Control(pDX, IDC_EDIT_STATE, m_editState);
	DDX_Control(pDX, IDC_EDIT_CARDNO, m_editCardNo);
	DDX_Control(pDX, IDC_EDIT_BALANCE, m_editBalance);
	DDX_Control(pDX, IDC_EDIT_DEPOSIT, m_editDeposit);
	DDX_Control(pDX, IDC_EDIT_AMOUNT, m_editAmount);
}


BEGIN_MESSAGE_MAP(CUserCloseCard, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_READCARD, &CUserCloseCard::OnBnClickedButtonReadcard)
	ON_BN_CLICKED(IDC_BUTTON_CLOSECARD, &CUserCloseCard::OnBnClickedButtonClosecard)
END_MESSAGE_MAP()


// CUserCloseCard 消息处理程序


BOOL CUserCloseCard::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CUserCloseCard::OnBnClickedButtonReadcard()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strCard, strError;
	if (CCardOperator::ReadCard(strCard, strError,true))
	{
		int nLicense, nCardType, nUserID, nBalance;
		CString strCardNo;
		CCardOperator::DecodeUserCard(strCard, strCardNo, nLicense, nCardType, nUserID, nBalance);
		if (nCardType == CCardOperator::TYPE_USER_RECHARGE|| nCardType == CCardOperator::TYPE_USER_TIME)
		{
			CString strSql, strBalance,strDeposit,strAmount;
			std::vector<CUser> vec_user;
			strSql.Format(_T("SELECT * FROM User WHERE ID=%d ORDER BY ID"), nUserID);
			g_udb.Select(strSql, vec_user);
			std::vector<CCard> vec_card;
			strSql.Format(_T("SELECT * FROM Card WHERE No='%s' AND UserID=%d ORDER BY ID"), strCardNo, nUserID);
			g_cdb.Select(strSql, vec_card);
			if (vec_user.size() == 1 && vec_card.size() == 1)
			{
				GetDeposit(vec_user[0], vec_card[0]);
				InsertEdit(vec_user[0], vec_card[0]);
				m_SelectUser = vec_user[0];
				m_SelectCard = vec_card[0];
				if (nCardType == CCardOperator::TYPE_USER_TIME)
					nBalance = 0;
				strBalance.Format(_T("%.02f"), (float)nBalance / (float)g_nINT100);
				m_editBalance.SetWindowText(strBalance);
				strAmount.Format(_T("%.02f"), (float)(nBalance+ m_nDeposit) / (float)g_nINT100);
				m_editAmount.SetWindowText(strAmount);
			}
			else
				AfxMessageBox(_T("无客户信息!"));
		}
		else
			AfxMessageBox(_T("此卡非会员卡!"));
	}
	else
		AfxMessageBox(_T("读卡失败!\r\n") + strError);
}


void CUserCloseCard::OnBnClickedButtonClosecard()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strID, strNo;
	m_editID.GetWindowText(strID);
	m_editCardNo.GetWindowText(strNo);
	int nID = _ttoi(strID);
	CString strCard, strError;
	if (CCardOperator::ReadCard(strCard, strError))
	{
		int nLicense, nCardType, nUserID, nBalance;
		CString strCardNo;
		CCardOperator::DecodeUserCard(strCard, strCardNo, nLicense, nCardType, nUserID, nBalance);
		if ((nCardType == CCardOperator::TYPE_USER_RECHARGE || nCardType == CCardOperator::TYPE_USER_TIME) && nUserID == nID && strCardNo == strNo)
		{
			int nIDRecord;
			std::vector<CRecord> vec_record;
			if (nCardType == CCardOperator::TYPE_USER_RECHARGE)
			{
				g_rrdb.Select(_T("SELECT * FROM RefundRecord ORDER BY ID DESC LIMIT 1"), vec_record);
				if (vec_record.size() == 1)
					nIDRecord = vec_record[0].GetID() + 1;
				else
					nIDRecord = 1;
				GetDeposit(m_SelectUser, m_SelectCard);
				CRecord recordRefund(nIDRecord, m_SelectUser.GetID(), m_SelectCard.GetNo(), nBalance, nBalance, 0, m_nDeposit, _T("退卡_退款"), g_employee.GetID(), _T(""));
				g_rrdb.Insert(recordRefund);//退款
			}
			g_rdrdb.Select(_T("SELECT * FROM ReturnDepositRecord ORDER BY ID DESC LIMIT 1"), vec_record);
			if (vec_record.size() == 1)
				nIDRecord = vec_record[0].GetID() + 1;
			else
				nIDRecord = 1;
			GetDeposit(m_SelectUser, m_SelectCard);
			CRecord recordDeposit(nIDRecord, m_SelectUser.GetID(), m_SelectCard.GetNo(), m_nDeposit, m_nDeposit, 0,0, _T("退卡_退押金"), g_employee.GetID(), _T(""));
			g_rdrdb.Insert(recordDeposit);//退押金
			m_SelectCard.SetState(CCard::STATE_CLOSE);
			m_SelectCard.SetEmployeeIDCloseCard(g_employee.GetID());
			if (g_cdb.UpdateCloseCard(m_SelectCard))
			{
				CString strSql;
				std::vector<CCard> vec_card;
				strSql.Format(_T("SELECT * FROM Card WHERE UserID=%d AND State=%d ORDER BY ID"), m_SelectCard.GetUserID(),CCard::STATE_OPEN);
				g_cdb.Select(strSql, vec_card);
				if (!vec_card.size())
				{
					m_SelectUser.SetState(CUser::STATE_CLOSE);
					g_udb.UpdateCloseCard(m_SelectUser);
				}
				CCardOperator::ClearUserCard(true);
				AfxMessageBox(_T("退卡成功!"));
			}
			else
				AfxMessageBox(_T("退卡失败!"));
		}
		else
			AfxMessageBox(_T("请重新读卡!"));
	}
	else
		AfxMessageBox(_T("退卡失败!\r\n") + strError);
}


void CUserCloseCard::GetDeposit(const CUser & user, const CCard & card)
{
	CString strSql;
	std::vector<CRecord> vec_record;
	strSql.Format(_T("SELECT * FROM ( SELECT * FROM InitDepositRecord UNION SELECT * FROM ReturnDepositRecord )  WHERE UserID=%d AND CardNo='%s' ORDER BY Time DESC LIMIT 1"), user.GetID(), card.GetNo());
	g_rdrdb.Select(strSql, vec_record);
	if (vec_record.size() == 1)
		m_nDeposit = vec_record[0].GetAmountNew();
	else
		m_nDeposit = card.GetDeposit();
}


void CUserCloseCard::InsertEdit(const CUser & user, const CCard & card)
{
	CString str;
	str.Format(_T("%d"), user.GetID());
	m_editID.SetWindowText(str);
	m_editName.SetWindowText(user.GetName());
	m_editIDNo.SetWindowText(user.GetIDNo());
	m_editPhoneNo.SetWindowText(user.GetPhoneNo());
	m_editAddr.SetWindowText(user.GetAddr());
	str.Format(_T("%d"), user.GetLicense());
	m_editLicense.SetWindowText(str);
	CString sz_strState[] = { _T("0-未开卡"),_T("1-已开卡"), _T("2-已退卡") };
	str = sz_strState[user.GetState()];
	m_editState.SetWindowText(str);
	m_editCardNo.SetWindowText(card.GetNo());
	str.Format(_T("%.02f"), (float)m_nDeposit / (float)g_nINT100);
	m_editDeposit.SetWindowText(str);
}