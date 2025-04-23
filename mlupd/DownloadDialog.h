#pragma once

class DownloadDialog
{
public:

    typedef struct PARAM
    {
        std::string srcUrl;     // ダウンロードしたいファイルのURL。
        std::string dstPath;    // 出力ファイル名。
        std::string username;
        std::string password;      

        // 以下はDownloadDialogが使う。
        bool abortReq = false;  // ダウンロード中断要求。
        std::ofstream outFile;  // 出力中のファイル。
        std::thread thread;     // ダウンロードを実行したスレッド。
        UINT64 total = 0;       // ダウンロードの総サイズ。
        UINT64 current = 0;     // 現在までのダウンロード完了サイズ。
        CURL* curl = NULL;      // curl。
        MLERR err = S_OK;       // 戻り値。
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

    // curlのコールバック。
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
