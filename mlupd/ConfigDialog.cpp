#include "framework.h"
#include "MlupdApp.h"
#include "Resource.h"
#include "pathmap.h"
#include "ConfigDialog.h"
#include "FtpLoginDialog.h"

ConfigDialog *ConfigDialog::m_pThis = NULL;

ConfigDialog::ConfigDialog(Mlupd *mlupd)
    : m_mlupd(mlupd)
{
    assert(m_pThis == NULL);  // DownloadDialogを同時に複数使うの禁止。
    m_pThis = this;
}

ConfigDialog::~ConfigDialog()
{
    m_pThis = NULL;
}

MLERR ConfigDialog::Init()
{
    return S_OK;
}

int ConfigDialog::DoModal()
{
    DialogBox(m_mlupd->m_hInst, MAKEINTRESOURCE(IDD_CONFIG), NULL, DialogProc);

    return m_ret;
}

INT_PTR ConfigDialog::OnInitDialog(HWND hDlg)
{
    if (m_mlupd->parentWndHandle) {
        m_mlupd->CenterWindow(hDlg, m_mlupd->parentWndHandle);
    }

    CONFIG_ITEM_INFO items[] = {
        { controlEdit,  IDC_TARGET_FILENAME_EDIT,   { &m_mlupd->target_filename, NULL },     "target_filename",      },
        { controlEdit,  IDC_TARGET_URL_EDIT,        { &m_mlupd->target_url, NULL },          "target_url",           },
        { controlEdit,  IDC_TARGET_VERSION_EDIT,    { &m_mlupd->target_version, NULL },      "target_version",       },
        { controlEdit,  IDC_INSTALL_COMMAND_EDIT,   { &m_mlupd->install_command, NULL },     "install_command",      },
        { controlEdit,  IDC_INSTALL_OPTION_EDIT,    { &m_mlupd->install_option, NULL },      "install_option",       },
        { controlCheck, IDC_FORCE_UPDATE_CHECK,     { NULL, &m_mlupd->force_update, },       "force_update",         },
        { controlEdit,  IDC_SKIP_VERSION_FILE_EDIT, { &m_mlupd->skip_version, NULL },        "skip_version",         },
        { controlEdit,  IDC_EXPECTED_SHA256_EDIT,   { &m_mlupd->expected_shr256, NULL },     "expected_shr256",      },
        { controlCheck, IDC_UPLOAD_RESPONSE_CHECK,  { NULL, &m_mlupd->uploadConfig, },       NULL,                   },
    };
    for (int n = 0; n < numof(items); n++) {
        configItems.push_back(items[n]);
    }

    if (!InitControls(hDlg)) {
        return FALSE;
    }

    if (!LoadConfigFile(hDlg)) {
        //return FALSE;
        // ロードできなくても続ける。
    }

    return TRUE;
}

INT_PTR ConfigDialog::OnOK(HWND hDlg)
{
    if (!SaveConfigFile(hDlg)) {
        std::string s = m_mlupd->LoadString(IDS_ERR_CONFIG_FILE_COULD_NOT_BE_SAVED);
        MessageBoxA(hDlg, s.c_str(), NULL, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

#if 0   // FTPでのアップロード。現在は無効。
    if (m_mlupd->uploadConfig) {
        FtpLoginDialog dlg(m_mlupd);
        dlg.Init();
        if (dlg.DoModal(m_mlupd->m_hInst) != IDOK) {
            return FALSE;
        }
        std::string configName = m_mlupd->GetConfigFilePath();
        if (!m_mlupd->UploadFileFTPS(configName.c_str(),
            m_mlupd->ftpUrl.c_str(),
            m_mlupd->ftpUsername.c_str(),
            m_mlupd->ftpPassword.c_str())) {
            std::string s = m_mlupd->LoadString(IDS_ERR_CONFIG_FILE_COULD_NOT_BE_UPLOADED);
            MessageBoxA(hDlg, s.c_str(), NULL, MB_OK | MB_ICONEXCLAMATION);
            return FALSE;
        }
    }
#endif

    return TRUE;
}

INT_PTR CALLBACK ConfigDialog::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    INT_PTR res = 0;

    switch (message)
    {
    case WM_INITDIALOG:
        res = m_pThis->OnInitDialog(hDlg);
        if (res == FALSE) {
            m_pThis->m_ret = IDOK;
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

bool ConfigDialog::SaveConfigFile(HWND hDlg)
{
    json j;

    for (const auto& item : configItems) {
        HWND hItem = GetDlgItem(hDlg, item.id);
        switch (item.type) {
        case controlEdit:
        {
            std::string s;
            s.resize(1024);
            GetWindowTextA(hItem, s.data(), 1024);
            s.resize(strlen(s.c_str()));
            *item.pstr = s;
            if (item.key) {
                j[item.key] = item.pstr->c_str();
            }
        }
        break;
        case controlCheck:
        {
            *item.pbool = Button_GetCheck(hItem);
            if (item.key) {
                j[item.key] = *item.pbool;
            }
        }
        break;
        }
    }

    std::string configName = m_mlupd->GetConfigFilePath();
    if (!m_mlupd->SaveJson(configName, j)) {
        return false;
    }

    return true;
}

bool ConfigDialog::InitControls(HWND hDlg)
{
    for (const auto& item : configItems) {
        HWND hItem = GetDlgItem(hDlg, item.id);
        switch (item.type) {
        case controlEdit:
        {
            SetWindowTextA(hItem, item.pstr->c_str());
        }
        break;
        case controlCheck:
        {
            Button_SetCheck(hItem, *item.pbool);
        }
        break;
        }
    }

    return true;
}

bool ConfigDialog::LoadConfigFile(HWND hDlg)
{
    json j;

    std::string configName = m_mlupd->GetConfigFilePath();
    if (!PathFileExistsA(configName.c_str())) {
        return false;
    }
    if (!m_mlupd->LoadJson(configName, j)) {
        return false;
    }

    for (const auto& item : configItems) {
        HWND hItem = GetDlgItem(hDlg, item.id);
        switch (item.type) {
        case controlEdit:
        {
            if (item.key) {
                *item.pstr = j[item.key].get<std::string>().c_str();
            }
            SetWindowTextA(hItem, item.pstr->c_str());
        }
        break;
        case controlCheck:
        {
            if (item.key) {
                *item.pbool = j[item.key].get<bool>();
            }
            Button_SetCheck(hItem, *item.pbool);
        }
        break;
        }
    }

    return true;
}
