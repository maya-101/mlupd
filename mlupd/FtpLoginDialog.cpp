#include "framework.h"
#include "MlupdApp.h"
#include "Resource.h"
#include "pathmap.h"
#include "FtpLoginDialog.h"

FtpLoginDialog *FtpLoginDialog::m_pThis = NULL;

INT_PTR CALLBACK FtpLoginDialog::OnInitDialog(HWND hDlg)
{
    if (m_mlupd->parentWndHandle) {
        m_mlupd->CenterWindow(hDlg, m_mlupd->parentWndHandle);
    }

    LOGIN_ITEM items[] =
    {
        { controlEdit, IDC_FTP_URL_EDIT,         &m_mlupd->ftpUrl,  },
        { controlEdit, IDC_FTP_USERNAME_EDIT,    &m_mlupd->ftpUsername,  },
        { controlEdit, IDC_FTP_PASSWORD_EDIT,    &m_mlupd->ftpPassword,  },
    };
    for (int n = 0; n < numof(loginItems); n++) {
        loginItems.push_back(items[n]);
    }

    for (const auto& item : loginItems) {
        HWND hItem = GetDlgItem(hDlg, item.id);
        switch (item.type) {
        case controlEdit:
        {
            SetWindowTextA(hItem, item.pstr->c_str());
        }
        break;
        }
    }

    return true;
}

INT_PTR CALLBACK FtpLoginDialog::OnOK(HWND hDlg)
{
    for (const auto& item : loginItems) {
        HWND hItem = GetDlgItem(hDlg, item.id);
        switch (item.type) {
        case controlEdit:
        {
            std::string s;
            s.resize(1024);
            GetWindowTextA(hItem, s.data(), 1024);
            s.resize(strlen(s.c_str()));
            *item.pstr = s;
        }
        break;
        }
    }

    return true;
}

INT_PTR CALLBACK FtpLoginDialog::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    INT_PTR res = 0;

    switch (message)
    {
    case WM_INITDIALOG:
        // ‰Šú‰»
        res = m_pThis->OnInitDialog(hDlg);
        if (res == FALSE) {
            m_pThis->m_ret = IDABORT;
            EndDialog(hDlg, m_pThis->m_ret);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            res = m_pThis->OnOK(hDlg);
            if (res == TRUE) {
                m_pThis->m_ret = IDOK;
                EndDialog(hDlg, m_pThis->m_ret);
            }
            break;

        case IDCANCEL:
            m_pThis->m_ret = IDCANCEL;
            EndDialog(hDlg, m_pThis->m_ret);
            break;
        }
        break;
    }

    return res;
}

FtpLoginDialog::FtpLoginDialog(Mlupd *mlupd)
    : m_mlupd(mlupd)
{
    assert(m_pThis == NULL);  // DownloadDialog‚ð“¯Žž‚É•¡”Žg‚¤‚Ì‹ÖŽ~B
    m_pThis = this;
}

FtpLoginDialog::~FtpLoginDialog()
{
    m_pThis = NULL;
}

MLERR FtpLoginDialog::Init()
{
    return S_OK;
}

int FtpLoginDialog::DoModal()
{
    DialogBox(m_mlupd->m_hInst, MAKEINTRESOURCE(IDD_FTP_LOGIN), NULL, DialogProc);
    return m_ret;
}
