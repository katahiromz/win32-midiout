#include <windows.h>
#include <mmsystem.h>
#include <iostream>

bool PlayMidiFile(const std::wstring& filePath)
{
    // MCIコマンド文字列
    std::wstring command = L"play \"" + filePath + L"\" wait";

    // MCIコマンド実行
    MCIERROR error = mciSendStringW(command.c_str(), nullptr, 0, nullptr);

    // エラーチェック
    if (error != 0) {
        std::wcerr << L"MCI error: " << error << std::endl;
        return false;
    }

    return true;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc <= 1)
    {
        std::cout << "No file specified" << std::endl;
        return 1;
    }

    // MIDファイル再生
    std::wcout << L"Playing MID..." << std::endl;
    PlayMidiFile(argv[1]);
    return 0;
}

int main(void)
{
    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    int ret = wmain(argc, argv);
    LocalFree(argv);
    return ret;
}
