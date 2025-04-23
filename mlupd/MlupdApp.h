#pragma once

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define MLUPD_OK                                    0     // 正常終了。
#define MLUPD_NEW_VERSION_FOUND_ON_SERVER           1

#define MLUPD_ERR                                   0x1000  // MLUPD_ERR + 下記エラーコードを返す。
#define ERR_CONFIG_FILE_COULD_NOT_BE_FOUND        1       // コンフィグファイルが見つからなかった。
#define ERR_COULD_NOT_OPEN_CONFIG_FILE            2       // コンフィグファイルを開けなかった。  
#define ERR_FAILED_TO_DOWNLOAD_CONFIG_FILE        3       // コンフィグファイルのダウンロードに失敗した。
#define ERR_COULD_NOT_OUTPUT_FILE                 4       // ファイルを出力できなかった。
#define ERR_DOWNLOAD_INTERRUPTED_BY_USER          5       // ダウンロードがユーザーによって中断された。

// CURL固有のエラーコード。
#define MLUPD_ERR_CURLE                             0x2000  // MLUPD_ERR_CURLE + CURLcode

typedef int MLERR;

#define RET_CURLERR(err)    if((err)!=CURLE_OK){return(MLUPD_ERR_CURLE+(err));}
#define BREAK_CURLERR(err)    if((err)!=CURLE_OK){break;}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define numof(ar) (sizeof(ar)/sizeof((ar)[0]))

enum ControlType
{
    controlEdit,
    controlCheck,
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class Mlupd
{
public:

    Mlupd(HINSTANCE hInst);
    virtual ~Mlupd();
    MLERR Main(int argc, std::vector<std::string> argv);

    HINSTANCE m_hInst;

    std::string configfileOption  = std::string("mlupd.config.json");
    std::string password;
    std::string username;
    bool helpFlag               = false;
    bool checkOnlyFlag          = false;
    bool downloadOnlyFlag       = false;
    bool noVersionSkipFlag      = false;
    bool showConfigFlag         = false;
    bool showProgressFlag       = false;
    bool forceUpdate            = false;
    HWND parentWndHandle        = NULL;
    bool skipCurrentVer         = false;
    bool cancelVerSkip          = false;
    

    std::string target_filename     = "MyAppInstaller.exe";
    std::string target_url          = "https://example.com/download/";
    std::string target_version      = "1.0.0";
    std::string install_command     = "MyAppInstaller.exe";
    std::string install_option      = "/quiet";
    bool force_update               = false;
    std::string skip_version;
    std::string expected_shr256;    // 未実装。
    bool uploadConfig               = false;    // upload機能は未実装。

    std::string ftpUrl          = "ftp://xxx.yyy.zzz/root";
    std::string ftpUsername     = "UserName";
    std::string ftpPassword     = "password";

    // confileファイルのパス(ローカル)を取得。
    std::string GetExecFilePath();

    // ファイルをアップロードする。
    bool UploadFileFTPS(const std::string& localFilePath,
        const std::string& ftpsUrl,
        const std::string& username,
        const std::string& password);

    // バージョンを比較する。
    bool VersionIsNewer(const std::string& remote, const std::string& local);
    // Tempフォルダのパスを取得する。
    std::string GetTempFolderPath();
    // AppDataパスを取得する。
    std::wstring GetAppDataRoamingPath();
    // jsonファイルをロードする。
    bool LoadJson(const std::string& path, json& outJson);
    // jsonファイルをセーブする。
    bool SaveJson(const std::string& path, const json& inJson);
    // コマンドラインのオプションの有無を調べる。
    bool HasFlag(const std::vector<std::string>& args, const std::string& flag);
    // コマンドラインのパラメータ付きオプションを取得する。
    std::string GetOptionValue(const std::vector<std::string>& args, const std::string& key_prefix, PCSTR defValue);
    UINT GetOptionValue(const std::vector<std::string>& args, const std::string& key_prefix, UINT defValue);
    UINT64 GetOptionUINT64(const std::vector<std::string>& args, const std::string& key_prefix, UINT64 defValue);
    // hParentの中央にhWndを持ってくる。
    void CenterWindow(HWND hWnd, HWND hParent = NULL);

    // confileファイルのパス(ローカル)を取得。
    std::string GetConfigFilePath();
    // レジストリ。
    std::string RegGetString(HKEY hRoot, PCSTR subKey, PCSTR valueName, PCSTR defValue);
    bool RegSetString(HKEY hRoot, PCSTR subKey, PCSTR valueName, PCSTR value);
    // リソースから文字列をロード。
    std::string LoadString(UINT nID);
    // ディレクトリを作成。
    bool CreateFullDirectory(const std::string& fullPath);
};
