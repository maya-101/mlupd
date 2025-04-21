#include "framework.h"
#include "mlupd.h"
#include "Resource.h"
#include "MlupdApp.h"

std::vector<std::string> ConvertArgvWToUtf8(LPWSTR* argvW, int argc)
{
    std::vector<std::string> result;
    for (int i = 0; i < argc; ++i) {
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
        std::string str(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, &str[0], sizeNeeded, NULL, NULL);
        str.pop_back(); // null文字削除
        result.push_back(str);
    }
    return result;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    int argc = 0;
    LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argvW == nullptr) {
        MessageBox(nullptr, L"コマンドライン解析に失敗しました。", L"mlupd", MB_OK);
        return 1;
    }

    std::vector<std::string> argv = ConvertArgvWToUtf8(argvW, argc);

    Mlupd mlupd(hInstance);

    int res = mlupd.Main(argc, argv);

    return res;
}
