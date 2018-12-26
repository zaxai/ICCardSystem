#pragma once
class CCard
{
public:
	enum STATE
	{
		STATE_UNOPEN = 0,
		STATE_OPEN,
		STATE_CLOSE,
		STATE_ERROR
	};
private:
	int m_nID;
	int m_nUserID;
	CString m_strNo;//����
	int m_nType;//����
	CString m_strDeadline;//��ֹ����
	int m_nDeposit;//��λ:��
	int m_nTotalAmount;//��λ:��
	int m_nState;
	int m_nEmployeeIDOpenCard;
	CString m_strTimeOpenCard;
	int m_nEmployeeIDCloseCard;
	CString m_strTimeCloseCard;
public:
	CCard();
	CCard(int nID, int nUserID, CString strNo, int nType, CString strDeadline, int nDeposit, int nTotalAmount, int nState, int nEmployeeIDOpenCard, CString strTimeOpenCard, int nEmployeeIDCloseCard, CString strTimeCloseCard);
	virtual ~CCard();
	int GetID() const;
	int GetUserID() const;
	CString GetNo() const;
	int GetType() const;
	CString GetDeadline() const;
	int GetDeposit() const;
	int GetTotalAmount() const;
	int GetState() const;
	int GetEmployeeIDOpenCard() const;
	CString GetTimeOpenCard() const;
	int GetEmployeeIDCloseCard() const;
	CString GetTimeCloseCard() const;
	void SetID(int nID);
	void SetUserID(int nUserID);
	void SetNo(CString strNo);
	void SetType(int nType);
	void SetDeadline(CString strDeadline);
	void SetDeposit(int nDeposit);
	void SetTotalAmount(int nTotalAmount);
	void SetState(int nState);
	void SetEmployeeIDOpenCard(int nEmployeeIDOpenCard);
	void SetTimeOpenCard(CString strTimeOpenCard);
	void SetEmployeeIDCloseCard(int nEmployeeIDCloseCard);
	void SetTimeCloseCard(CString strTimeCloseCard);
};
