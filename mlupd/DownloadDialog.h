#pragma once

class DownloadDialog
{
public:

    typedef struct PARAM
    {
        std::string srcUrl;     // �_�E�����[�h�������t�@�C����URL�B
        std::string dstPath;    // �o�̓t�@�C�����B
        std::string username;
        std::string password;      

        // �ȉ���DownloadDialog���g���B
        bool abortReq = false;  // �_�E�����[�h���f�v���B
        std::ofstream outFile;  // �o�͒��̃t�@�C���B
        std::thread thread;     // �_�E�����[�h�����s�����X���b�h�B
        UINT64 total = 0;       // �_�E�����[�h�̑��T�C�Y�B
        UINT64 current = 0;     // ���݂܂ł̃_�E�����[�h�����T�C�Y�B
        CURL* curl = NULL;      // curl�B
        MLERR err = S_OK;       // �߂�l�B
    }
    PARAM;

    DownloadDialog(Mlupd *mlupd);
    virtual ~DownloadDialog();
    MLERR Init(std::string srcUrl, std::string dstPath, std::string username, std::string password);
    MLERR DoModal();

protected:

#define TID_UPDATE 0x0001

    static DownloadDialog *m_pThis;
    HWND m_hDlg = NULL;
    PARAM m_param;
    Mlupd *m_mlupd;

    // curl�̃R�[���o�b�N�B
    static size_t Download_WriteCallback(void* ptr, size_t size, size_t nmemb, void* stream);
    static int Download_ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

    MLERR Download();
    static void DownloadThread(DownloadDialog *pThis);

    INT_PTR OnInitDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    INT_PTR OnTimer(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    INT_PTR OnCancel(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    INT_PTR OnDestroy(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    INT_PTR OnClose(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};
