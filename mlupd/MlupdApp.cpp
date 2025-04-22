#include "framework.h"
#include "MlupdApp.h"
#include "pathmap.h"
#include "ConfigDialog.h"
#include "FtpLoginDialog.h"
#include "DownloadDialog.h"
#include "GenericDialog.h"
#include "resource.h"

#define REGKEY_SETTINGS "Software\\mlupd\\settings"
#define REGKEY_SKIP_VERSIONS REGKEY_SETTINGS "\\skip_versions"

Mlupd::Mlupd(HINSTANCE hInst)
    : m_hInst(hInst)
{

}

Mlupd::~Mlupd()
{

}

std::string Mlupd::GetExecFilePath()
{
    char modulePath[MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
    GetModuleFileNameA(NULL, modulePath, sizeof(modulePath));
    _splitpath_s(modulePath, drive, dir, fname, ext);

    char path[MAX_PATH] = { NULL };
    strcat_s(path, drive);
    strcat_s(path, dir);

    return std::string(path);
}

bool Mlupd::UploadFileFTPS(const std::string& localFilePath,
    const std::string& ftpsUrl,
    const std::string& username,
    const std::string& password)
{
    CURL *curl;
    CURLcode res;
    FILE *hd_src;
    struct stat file_info;

    // ファイル情報取得
    if (stat(localFilePath.c_str(), &file_info)) {
        std::cerr << "Cannot access file: " << localFilePath << std::endl;
        return false;
    }

    // ファイルを開く
    if (fopen_s(&hd_src, localFilePath.c_str(), "rb")) {
        std::cerr << "Cannot open file: " << localFilePath << std::endl;
        return false;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // FTPS URL（例: ftps://example.com/path/file.json）
        curl_easy_setopt(curl, CURLOPT_URL, ftpsUrl.c_str());

        // 認証情報
        curl_easy_setopt(curl, CURLOPT_USERPWD, (username + ":" + password).c_str());

        // FTPSオプション
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

#if 0
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // 本番では有効化推奨
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // 本番では有効化推奨
#else
        std::string caPath = GetExecFilePath() + "curl-ca-bundle.crt";
        curl_easy_setopt(curl, CURLOPT_CAINFO, caPath.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

        // アップロードモード
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);

        // 転送実行
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: "
                << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            fclose(hd_src);
            return false;
        }

        // 後始末
        curl_easy_cleanup(curl);
    }

    fclose(hd_src);
    curl_global_cleanup();

    return true;
}

bool Mlupd::VersionIsNewer(const std::string& remote, const std::string& local)
{
    auto to_vector = [](const std::string& v) {
        std::stringstream ss(v);
        std::string item;
        std::vector<int> parts;
        while (std::getline(ss, item, '.')) parts.push_back(std::stoi(item));
        return parts;
        };
    auto r = to_vector(remote);
    auto l = to_vector(local);
    for (size_t i = 0; i < (std::max)(r.size(), l.size()); ++i) {
        int ri = (i < r.size()) ? r[i] : 0;
        int li = (i < l.size()) ? l[i] : 0;
        if (ri > li) return true;
        if (ri < li) return false;
    }
    return false;
}

std::string Mlupd::GetTempFolderPath()
{
    char tempPath[MAX_PATH];
    DWORD len = GetTempPathA(MAX_PATH, tempPath);
    if (len > 0 && len < MAX_PATH) {
        return std::string(tempPath);
    }
    else {
        return ""; // エラー時は空文字列
    }
}

std::wstring Mlupd::GetAppDataRoamingPath()
{
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);
    std::wstring result;
    if (SUCCEEDED(hr)) {
        result = path;
        CoTaskMemFree(path);  // 解放忘れずに！
    }
    return result;
}

bool Mlupd::LoadJson(const std::string& path, json& outJson)
{
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        return false;
    }

    try {
        ifs >> outJson;
    }
    catch (std::exception& e) {
        std::cerr << "JSON読み込みエラー: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool Mlupd::SaveJson(const std::string& path, const json& inJson)
{
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        return false;
    }

    try {
        ofs << inJson.dump(4);  // 4はインデント数
    }
    catch (std::exception& e) {
        std::cerr << "JSON書き出しエラー: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool Mlupd::HasFlag(const std::vector<std::string>& args, const std::string& flag)
{
    return std::find(args.begin(), args.end(), flag) != args.end();
}

std::string Mlupd::GetOptionValue(const std::vector<std::string>& args, const std::string& key_prefix, PCSTR defValue)
{
    for (const auto& arg : args) {
        if (arg.rfind(key_prefix, 0) == 0) { // prefix一致
            return arg.substr(key_prefix.length());
        }
    }
    return std::string(defValue);
}

UINT Mlupd::GetOptionValue(const std::vector<std::string>& args, const std::string& key_prefix, UINT defValue)
{
    for (const auto& arg : args) {
        if (arg.rfind(key_prefix, 0) == 0) { // prefix一致
            std::string s = arg.substr(key_prefix.length());
            int radix = 10;
            if (s.rfind("0x", 0) == 0) {
                radix = 16;
            }
            UINT n = strtol(s.c_str(), NULL, radix);
            return n;
        }
    }
    return defValue;
}

UINT64 Mlupd::GetOptionUINT64(const std::vector<std::string>& args, const std::string& key_prefix, UINT64 defValue)
{
    for (const auto& arg : args) {
        if (arg.rfind(key_prefix, 0) == 0) { // prefix一致
            std::string s = arg.substr(key_prefix.length());
            int radix = 10;
            if (s.rfind("0x", 0) == 0) {
                radix = 16;
            }
            UINT64 n = strtoll(s.c_str(), NULL, radix);    // 64bit
            return n;
        }
    }
    return defValue;
}

std::string Mlupd::GetConfigFilePath()
{
    char modulePath[MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
    GetModuleFileNameA(NULL, modulePath, sizeof(modulePath));
    _splitpath_s(modulePath, drive, dir, fname, ext);

    char path[MAX_PATH] = { NULL };
    strcat_s(path, drive);
    strcat_s(path, dir);
    strcat_s(path, configfileOption.c_str());

    return std::string(path);
}

std::string Mlupd::RegGetString(HKEY hRoot, PCSTR subKey, PCSTR valueName, PCSTR defValue)
{
    HKEY hKey;
    if (RegOpenKeyExA(hRoot, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return defValue;

    char buffer[512];  // 十分なバッファ（長すぎる場合は工夫要）
    DWORD bufferSize = sizeof(buffer);
    DWORD type = REG_SZ;

    LONG result = RegQueryValueExA(hKey, valueName, nullptr, &type, reinterpret_cast<LPBYTE>(buffer), &bufferSize);
    RegCloseKey(hKey);

    if (result == ERROR_SUCCESS && type == REG_SZ) {
        return std::string(buffer);
    }

    return defValue;
}

bool Mlupd::RegSetString(HKEY hRoot, PCSTR subKey, PCSTR valueName, PCSTR value)
{
    HKEY hKey;
    if (RegCreateKeyExA(hRoot, subKey, 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS)
        return false;

    LONG result = RegSetValueExA(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(value), (DWORD)(strlen(value) + 1));
    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
}

std::string Mlupd::LoadString(UINT nID)
{
    std::string s;
    s.resize(1024);
    LoadStringA(m_hInst, nID, s.data(), 1024);
    s.resize(strlen(s.c_str()));
    return s;
}

bool Mlupd::CreateFullDirectory(const std::string& fullPath)
{
    std::istringstream iss(fullPath);
    std::string token;
    std::string path;

    // Windowsパスが "C:\..." から始まる場合、最初の "C:" を残しておく
    if (fullPath.length() >= 2 && fullPath[1] == ':') {
        path = fullPath.substr(0, 2);  // "C:"
        iss.ignore(2); // skip "C:"
    }

    // パスを区切って順に作成
    while (std::getline(iss, token, '\\')) {
        if (token.empty()) continue;
        path += "\\" + token;
        if (GetFileAttributesA(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
            if (!CreateDirectoryA(path.c_str(), nullptr)) {
                DWORD err = GetLastError();
                if (err != ERROR_ALREADY_EXISTS) return false;
            }
        }
    }

    return true;
}

void Mlupd::CenterWindow(HWND hWnd, HWND hParent)
{
    RECT rcWnd, rcParent;
    int x, y, width, height;

    // ウィンドウサイズ取得
    GetWindowRect(hWnd, &rcWnd);
    width = rcWnd.right - rcWnd.left;
    height = rcWnd.bottom - rcWnd.top;

    if (hParent == NULL) {
        // 親ウィンドウが指定されていない場合、デスクトップを基準に
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rcParent, 0);
    }
    else {
        GetWindowRect(hParent, &rcParent);
    }

    x = rcParent.left + (rcParent.right - rcParent.left - width) / 2;
    y = rcParent.top + (rcParent.bottom - rcParent.top - height) / 2;

    SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

MLERR Mlupd::Main(int argc, std::vector<std::string> argv)
{
    MLERR err = MLUPD_OK;
    CURLcode curlErr = CURLE_OK;

    // 各オプション。
    configfileOption = GetOptionValue(argv, "--configfile=", configfileOption.c_str());
    helpFlag = HasFlag(argv, "--help");
    checkOnlyFlag = HasFlag(argv, "--check-only");
    downloadOnlyFlag = HasFlag(argv, "--download-only");
    showConfigFlag = HasFlag(argv, "--config");
    inquiryUpdate = HasFlag(argv, "--inquiry-update");
    forceUpdate = HasFlag(argv, "--force-update");
    noVersionSkip = HasFlag(argv, "--no-version-skip");
    parentWndHandle = (HWND)GetOptionUINT64(argv, "--parent-window-handle=", (UINT64)parentWndHandle);

    if (helpFlag) {
        // コマンドライン書式表示。
        std::cout << "Usage: updater.exe [--configfile=mlupd.config.json] [--check-only] [--inquiry-update] [--download-only]\n";
        return MLUPD_OK;
    }

    if (showConfigFlag) {
        // コンフィグダイアログ。
        ConfigDialog dlg(this);
        dlg.Init();
        dlg.DoModal();
        return MLUPD_OK;
    }

    // curl初期化。
    curlErr = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (curlErr != CURLE_OK) {
        std::cerr << "curl init failed\n";
        return MLUPD_ERR_CURLE + curlErr;
    }

    // アプリ側のコンフィグファイルがあるか。
    std::string configFileName = GetConfigFilePath();
    if (!PathFileExistsA(configFileName.c_str())) {
        return MLUPD_ERR + ERR_CONFIG_FILE_COULD_NOT_BE_FOUND;
    }

    // アプリ側のコンフィグファイルのロード。
    json configLocal;
    if (!LoadJson(GetConfigFilePath(), configLocal)) {
        std::cerr << "設定ファイル読み込み失敗\n";
        return MLUPD_ERR + ERR_COULD_NOT_OPEN_CONFIG_FILE;
    }

    // サーバーからコンフィグファイルをダウンロード。
    std::string serverPath = configLocal["target_url"].get<std::string>();
    std::string tempPath = GetTempFolderPath() + "mlupd\\";
    std::string localPath = tempPath + configfileOption;   // サブディレクトリが作成できないのでアウト。

    if (!CreateFullDirectory(tempPath)) {
        //return MLUPD_ERR + ERR_COULD_NOT_OUTPUT_FILE;
    }

    DownloadDialog dlDlg(this);
    dlDlg.Init(serverPath + configfileOption, localPath);
    err = dlDlg.DoModal();
    if (err != MLUPD_OK) {
        std::cerr << "ダウンロードできなかった。\n";
        return err;
    }

    // サーバーのコンフィグファイルをロード。
    json configSvr;
    if (!LoadJson(localPath, configSvr)) {
        std::cerr << "設定ファイル読み込み失敗\n";
        return MLUPD_ERR + ERR_COULD_NOT_OPEN_CONFIG_FILE;
    }

    // サーバー側のバージョンと、アプリ側のバージョンを比較。
    std::string svrVersion = configSvr["target_version"].get<std::string>();
    std::string localVersion = configLocal["target_version"].get<std::string>();

    // 前回スキップしたバージョン(あれば)を取得。
    if (!noVersionSkip) {
        std::string key = REGKEY_SKIP_VERSIONS;
        std::string value = mlupd::pathmap::HashPath(GetConfigFilePath());
        localVersion = RegGetString(HKEY_CURRENT_USER, key.c_str(), value.c_str(), localVersion.c_str());
    }

    forceUpdate |= configLocal["force_update"].get<bool>();
    if (!forceUpdate && !VersionIsNewer(svrVersion, localVersion)) {
        std::cout << "バージョンは最新です。\n";
        return S_OK; // 変化なし
    }

    if (!forceUpdate && inquiryUpdate) {
        GenericDialog dlg(this);
        int res = dlg.DoModal(m_hInst, IDD_SKIP_UPDATE);
        if (res == IDIGNORE) {  // 次のバージョンまでスキップ。
            // スキップしたバージョンをレジストリに書いておく。
            std::string key = REGKEY_SKIP_VERSIONS;
            std::string value = mlupd::pathmap::HashPath(GetConfigFilePath());
            RegSetString(HKEY_CURRENT_USER, key.c_str(), value.c_str(), svrVersion.c_str());
            return S_OK;
        }
        else if (res == IDCANCEL) {
            return S_OK;
        }
    }

    std::cout << "新しいバージョン " << svrVersion << " を検出しました。\n";
    if (checkOnlyFlag) {
        return MLUPD_NEW_VERSION_FOUND_ON_SERVER; // 新バージョンあり
    }

    // ダウンロード開始。
    std::string targetUrl = configSvr["target_url"].get<std::string>();
    std::string targetFilename = configSvr["target_filename"].get<std::string>();

    if (!CreateFullDirectory(targetUrl)) {
        //return MLUPD_ERR + ERR_COULD_NOT_OUTPUT_FILE;
    }

    dlDlg.Init(targetUrl + targetFilename, tempPath + targetFilename);
    err = dlDlg.DoModal();
    if (err != MLUPD_OK) {
        std::cerr << "ダウンロードできなかった。\n";
        return err;
    }

    std::cout << "ダウンロード完了しました。\n";
    if (downloadOnlyFlag) {
        return S_OK; // ダウンロード完了。
    }

    std::cout << "インストーラーを実行します...\n";
    std::string installCommand = configSvr["install_command"].get<std::string>();
    std::string installOption = configSvr["install_option"].get<std::string>();
    ShellExecuteA(NULL, "open", installCommand.c_str(), installOption.c_str(), tempPath.c_str(), SW_SHOWNORMAL);

    curl_global_cleanup();

    return 0;
}
