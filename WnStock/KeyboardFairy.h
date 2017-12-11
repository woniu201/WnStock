#pragma once


// CKeyboardFairy 对话框

class CKeyboardFairy : public CDialogEx
{
	DECLARE_DYNAMIC(CKeyboardFairy)

public:
	CKeyboardFairy(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CKeyboardFairy();

// 对话框数据
	enum { IDD = IDD_DLG_KEYBOARDFAIRY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_stockInfo;
	CListBox m_stockList;
	afx_msg void OnChangeInputStockInfo();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	CFont font;
	afx_msg void OnDblclkListStock();
};
