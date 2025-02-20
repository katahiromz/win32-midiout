#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <cstdio>

// MIDファイル再生
bool play_midi_file(const std::wstring& filePath)
{
    // MCIコマンド実行
    std::wstring command = L"play \"" + filePath + L"\" wait";
    MCIERROR error = mciSendStringW(command.c_str(), nullptr, 0, nullptr);

    if (error) { // エラーチェック
        std::printf("MCI error: %d\n", error);
        return false;
    }

    return true;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc <= 1) {
        std::printf("No file specified\n");
        return 1;
    }

    std::printf("Playing MID...\n");
    play_midi_file(argv[1]);

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
