#pragma once

class FtpLoginDialog
{
public:

    FtpLoginDialog(Mlupd *mlupd);
    virtual ~FtpLoginDialog();
    MLERR Init();
    int DoModal();

protected:

    // loginダイアログのデータ交換用。
    typedef struct LOGIN_ITEM
    {
        ControlType type;       // コントロール種別
        int id;                 // リソースID
        std::string *pstr;      // 文字列型。
    }
    LOGIN_ITEM;
    std::vector<LOGIN_ITEM> loginItems;

    static FtpLoginDialog *m_pThis;
    HWND m_hDlg = NULL;
    int m_ret = IDOK;
    Mlupd *m_mlupd;

    INT_PTR CALLBACK OnInitDialog(HWND hDlg);
    INT_PTR CALLBACK OnOK(HWND hDlg);
    static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};
