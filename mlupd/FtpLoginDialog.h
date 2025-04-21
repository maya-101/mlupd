#pragma once

class FtpLoginDialog
{
public:

    FtpLoginDialog(Mlupd *mlupd);
    virtual ~FtpLoginDialog();
    MLERR Init();
    int DoModal();

protected:

    // login�_�C�A���O�̃f�[�^�����p�B
    typedef struct LOGIN_ITEM
    {
        ControlType type;       // �R���g���[�����
        int id;                 // ���\�[�XID
        std::string *pstr;      // ������^�B
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
