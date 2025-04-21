#pragma once

class ConfigDialog
{
public:

    ConfigDialog(Mlupd *mlupd);
    virtual ~ConfigDialog();
    MLERR Init();
    int DoModal();

protected:

    // configダイアログのデータ交換用マップ。
    typedef struct CONFIG_ITEM_INFO
    {
        ControlType type;       // コントロール種別
        int id;                 // リソースID
        struct {
            std::string *pstr;  // 文字列型。
            bool *pbool;        // bool型。
        };
        PCSTR key;              // コンフィグファイル(json)のキー。あれば。
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
