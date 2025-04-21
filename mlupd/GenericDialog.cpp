#include "framework.h"
#include "MlupdApp.h"
#include "GenericDialog.h"
#include "Resource.h"

GenericDialog *GenericDialog::m_pThis = NULL;

INT_PTR GenericDialog::OnInitDialog(HWND hDlg)
{
    return TRUE;
}

INT_PTR GenericDialog::OnOK(HWND hDlg)
{
    return TRUE;
}

INT_PTR GenericDialog::OnCancel(HWND hDlg)
{
    return TRUE;
}

INT_PTR CALLBACK GenericDialog::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    INT_PTR res = 0;

    switch (message)
    {
    case WM_INITDIALOG:
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
            res = m_pThis->OnCancel(hDlg);
            if (res == TRUE) {
                m_pThis->m_ret = IDCANCEL;
                EndDialog(hDlg, m_pThis->m_ret);
            }
            break;

        case IDABORT:
        case IDRETRY:
        case IDIGNORE:
        case IDYES:
        case IDNO:
            m_pThis->m_ret = LOWORD(wParam);
            EndDialog(hDlg, m_pThis->m_ret);
            break;
        }
        break;
    }
    return res;
}

GenericDialog::GenericDialog(Mlupd *mlupd)
    : m_mlupd(mlupd)
{
    assert(m_pThis == NULL);  // DownloadDialog‚ğ“¯‚É•¡”g‚¤‚Ì‹Ö~B
    m_pThis = this;
}

GenericDialog::~GenericDialog()
{
    m_pThis = NULL;
}

MLERR GenericDialog::Init()
{
    return S_OK;
}

int GenericDialog::DoModal(HINSTANCE hInst, UINT nID)
{
    DialogBox(hInst, MAKEINTRESOURCE(nID), NULL, DialogProc);
    return m_ret;
}

