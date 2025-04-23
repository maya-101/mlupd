#pragma once

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define MLUPD_OK                                    0     // ����I���B
#define MLUPD_NEW_VERSION_FOUND_ON_SERVER           1

#define MLUPD_ERR                                   0x1000  // MLUPD_ERR + ���L�G���[�R�[�h��Ԃ��B
#define ERR_CONFIG_FILE_COULD_NOT_BE_FOUND        1       // �R���t�B�O�t�@�C����������Ȃ������B
#define ERR_COULD_NOT_OPEN_CONFIG_FILE            2       // �R���t�B�O�t�@�C�����J���Ȃ������B  
#define ERR_FAILED_TO_DOWNLOAD_CONFIG_FILE        3       // �R���t�B�O�t�@�C���̃_�E�����[�h�Ɏ��s�����B
#define ERR_COULD_NOT_OUTPUT_FILE                 4       // �t�@�C�����o�͂ł��Ȃ������B
#define ERR_DOWNLOAD_INTERRUPTED_BY_USER          5       // �_�E�����[�h�����[�U�[�ɂ���Ē��f���ꂽ�B

// CURL�ŗL�̃G���[�R�[�h�B
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
    std::string expected_shr256;    // �������B
    bool uploadConfig               = false;    // upload�@�\�͖������B

    std::string ftpUrl          = "ftp://xxx.yyy.zzz/root";
    std::string ftpUsername     = "UserName";
    std::string ftpPassword     = "password";

    // confile�t�@�C���̃p�X(���[�J��)���擾�B
    std::string GetExecFilePath();

    // �t�@�C�����A�b�v���[�h����B
    bool UploadFileFTPS(const std::string& localFilePath,
        const std::string& ftpsUrl,
        const std::string& username,
        const std::string& password);

    // �o�[�W�������r����B
    bool VersionIsNewer(const std::string& remote, const std::string& local);
    // Temp�t�H���_�̃p�X���擾����B
    std::string GetTempFolderPath();
    // AppData�p�X���擾����B
    std::wstring GetAppDataRoamingPath();
    // json�t�@�C�������[�h����B
    bool LoadJson(const std::string& path, json& outJson);
    // json�t�@�C�����Z�[�u����B
    bool SaveJson(const std::string& path, const json& inJson);
    // �R�}���h���C���̃I�v�V�����̗L���𒲂ׂ�B
    bool HasFlag(const std::vector<std::string>& args, const std::string& flag);
    // �R�}���h���C���̃p�����[�^�t���I�v�V�������擾����B
    std::string GetOptionValue(const std::vector<std::string>& args, const std::string& key_prefix, PCSTR defValue);
    UINT GetOptionValue(const std::vector<std::string>& args, const std::string& key_prefix, UINT defValue);
    UINT64 GetOptionUINT64(const std::vector<std::string>& args, const std::string& key_prefix, UINT64 defValue);
    // hParent�̒�����hWnd�������Ă���B
    void CenterWindow(HWND hWnd, HWND hParent = NULL);

    // confile�t�@�C���̃p�X(���[�J��)���擾�B
    std::string GetConfigFilePath();
    // ���W�X�g���B
    std::string RegGetString(HKEY hRoot, PCSTR subKey, PCSTR valueName, PCSTR defValue);
    bool RegSetString(HKEY hRoot, PCSTR subKey, PCSTR valueName, PCSTR value);
    // ���\�[�X���當��������[�h�B
    std::string LoadString(UINT nID);
    // �f�B���N�g�����쐬�B
    bool CreateFullDirectory(const std::string& fullPath);
};
