#pragma once

class ConfigDialog
{
public:

    ConfigDialog(Mlupd *mlupd);
    virtual ~ConfigDialog();
    MLERR Init();
    int DoModal();

protected:

    // config�_�C�A���O�̃f�[�^�����p�}�b�v�B
    typedef struct CONFIG_ITEM_INFO
    {
        ControlType type;       // �R���g���[�����
        int id;                 // ���\�[�XID
        struct {
            std::string *pstr;  // ������^�B
            bool *pbool;        // bool�^�B
        };
        PCSTR key;              // �R���t�B�O�t�@�C��(json)�̃L�[�B����΁B
    }
    CONFIG_ITEM_INFO;
    std::vector<CONFIG_ITEM_INFO> configItems;

    static ConfigDialog *m_pThis;
    int m_ret = IDOK;
    Mlupd *m_mlupd;

    INT_PTR OnInitDialog(HWND hDlg);
    INT_PTR OnOK(HWND hDlg);
    static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    bool SaveConfigFile(HWND hDlg);
    bool LoadConfigFile(HWND hDlg);
    bool InitControls(HWND hDlg);

};
