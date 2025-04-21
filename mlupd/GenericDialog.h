#pragma once

class GenericDialog
{
public:

    GenericDialog(Mlupd *mlupd);
    virtual ~GenericDialog();
    MLERR Init();
    int DoModal(HINSTANCE hInst, UINT nID);

protected:

    static GenericDialog *m_pThis;
    HWND m_hDlg = NULL;
    int m_ret = IDOK;
    Mlupd *m_mlupd;

    INT_PTR CALLBACK OnInitDialog(HWND hDlg);
    INT_PTR CALLBACK OnOK(HWND hDlg);
    INT_PTR CALLBACK OnCancel(HWND hDlg);
    static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    bool SaveConfigFile(HWND hDlg);
    bool InitConfig(HWND hDlg);
    bool LoadConfigFile(HWND hDlg);
};
