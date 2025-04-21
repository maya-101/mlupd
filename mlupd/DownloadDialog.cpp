#include "framework.h"
#include "MlupdApp.h"
#include "Resource.h"
#include "pathmap.h"
#include "DownloadDialog.h"

DownloadDialog *DownloadDialog::m_pThis = NULL;

size_t DownloadDialog::Download_WriteCallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
    std::ofstream* out = reinterpret_cast<std::ofstream*>(stream);
    out->write(reinterpret_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

int DownloadDialog::Download_ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    DownloadDialog *pThis = (DownloadDialog *)clientp;

    // �i�����X�V�B
    pThis->m_param.total = dltotal;
    pThis->m_param.current = dlnow;

    // ���f�v�����o�Ă���悤�Ȃ璆�f�B
    if (pThis->m_param.abortReq) {
        return 1;
    }

    return 0;
}

MLERR DownloadDialog::Download()
{
    CURLcode curlErr = CURLE_OK;

    if (!m_mlupd->CreateFullDirectory(m_param.dstPath.c_str())) {
        return MLUPD_ERR + ERR_COULD_NOT_OUTPUT_FILE;
    }

    m_param.outFile.open(m_param.dstPath.c_str(), std::ios::binary);
    if (!m_param.outFile) {
        return MLUPD_ERR + ERR_COULD_NOT_OUTPUT_FILE;
    }

    m_param.curl = curl_easy_init();
    if (!m_param.curl) {
        return MLUPD_ERR_CURLE + CURLE_FAILED_INIT;
    }

    do {
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_URL, m_param.srcUrl.c_str());
        BREAK_CURLERR(curlErr);
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_WRITEFUNCTION, Download_WriteCallback);
        BREAK_CURLERR(curlErr);
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_WRITEDATA, &m_param.outFile);
        BREAK_CURLERR(curlErr);
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_NOPROGRESS, 0L);
        BREAK_CURLERR(curlErr);
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_XFERINFOFUNCTION, Download_ProgressCallback);
        BREAK_CURLERR(curlErr);
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_XFERINFODATA, this);
        BREAK_CURLERR(curlErr);
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_FOLLOWLOCATION, 1L);
        BREAK_CURLERR(curlErr);

#if 0
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_SSL_VERIFYPEER, 0L);
        BREAK_CURLERR(curlErr);
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_SSL_VERIFYHOST, 0L);
        BREAK_CURLERR(curlErr);
#else
        std::string caPath = m_mlupd->GetExecFilePath() + "curl-ca-bundle.crt";
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_CAINFO, caPath.c_str());
        BREAK_CURLERR(curlErr);
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_SSL_VERIFYPEER, 1L);
        BREAK_CURLERR(curlErr);
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_SSL_VERIFYHOST, 2L);
        BREAK_CURLERR(curlErr);
        curlErr = curl_easy_setopt(m_param.curl, CURLOPT_VERBOSE, 1L);
        BREAK_CURLERR(curlErr);
#endif

        curlErr = curl_easy_perform(m_param.curl);
        BREAK_CURLERR(curlErr);

    } while (0);

    if (m_param.curl) {
        curl_easy_cleanup(m_param.curl);
        m_param.curl = NULL;
    }
    if (m_param.outFile) {
        m_param.outFile.close();
    }

    if (curlErr != CURLE_OK) {
        m_param.err = MLUPD_ERR_CURLE + curlErr;
    }
    if (m_param.err != S_OK) {
        return m_param.err;
    }

    return S_OK;
}

void DownloadDialog::DownloadThread(DownloadDialog *pThis)
{
    MLERR err = S_OK;

    // �_�E�����[�h���s�B
    err = pThis->Download();

    // �܂��_�C�A���O���\������Ă���I������B
    if (IsWindow(pThis->m_hDlg)) {
        PostMessage(pThis->m_hDlg, WM_CLOSE, 0, 0);
    }
}

INT_PTR DownloadDialog::OnInitDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    MLERR err = S_OK;

    m_hDlg = hDlg;

    // UI�X�V�p�̃^�C�}�[�J�n�B
    SetTimer(m_hDlg, TID_UPDATE, 300, NULL);

    // �X���b�h�J�n�B
    m_param.thread = std::thread(DownloadThread, this);

    return 0;
}

INT_PTR DownloadDialog::OnTimer(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (wParam) {
    case TID_UPDATE:
        if (m_param.total > 0) {

            // �v���O���X�o�[�X�V�B
            HWND hProg = GetDlgItem(hDlg, IDC_PROGRESS);
            SendMessage(hProg, PBM_SETRANGE32, 0, (m_param.total + 1023) / 1024);
            SendMessage(hProg, PBM_SETPOS, (m_param.current + 1023) / 1024, 0);

            // �i���󋵍X�V�B
            HWND hStatic = GetDlgItem(hDlg, IDC_PROGRESS_STATIC);
            std::string s;
            s.resize(1024);
            sprintf_s(s.data(), 1024, "%I64d / %I64d (Bytes)", m_param.current, m_param.total);
            s.resize(strlen(s.c_str()));
            SetWindowTextA(hStatic, s.c_str());
        }
        break;
    }

    return 0;
}

INT_PTR DownloadDialog::OnCancel(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    // �_�E�����[�h�𒆒f����B
    m_param.abortReq = true;
    return 0;
}

INT_PTR DownloadDialog::OnDestroy(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    // �X���b�h���I���܂ő҂B
    m_param.thread.join();
    m_hDlg = NULL;
    return 0;
}

INT_PTR CALLBACK DownloadDialog::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    INT_PTR res = 0;

    switch (message)
    {
    case WM_INITDIALOG:
        res = m_pThis->OnInitDialog(hDlg, message, wParam, lParam);
        break;

    case WM_DESTROY:
        res = m_pThis->OnDestroy(hDlg, message, wParam, lParam);
        break;

    case WM_TIMER:
        res = m_pThis->OnTimer(hDlg, message, wParam, lParam);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            res = EndDialog(hDlg, IDOK);
            break;

        case IDCANCEL:
            res = m_pThis->OnCancel(hDlg, message, wParam, lParam);
            res = EndDialog(hDlg, IDCANCEL);
            break;
        }
        break;
    }

    return res;
}

DownloadDialog::DownloadDialog(Mlupd *mlupd)
    : m_mlupd(mlupd)
{
    assert(m_pThis == NULL);  // DownloadDialog�𓯎��ɕ����g���̋֎~�B
    m_pThis = this;
}

DownloadDialog::~DownloadDialog()
{
    m_pThis = NULL;
}

MLERR DownloadDialog::Init(std::string srcUrl, std::string dstPath)
{
    m_param = PARAM();
    m_param.srcUrl = srcUrl;
    m_param.dstPath = dstPath;
    return S_OK;
}

MLERR DownloadDialog::DoModal()
{
    DialogBox(m_mlupd->m_hInst, MAKEINTRESOURCE(IDD_DOWNLOAD), NULL, DialogProc);
    return m_param.err;
}
