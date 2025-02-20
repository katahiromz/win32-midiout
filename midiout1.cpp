// midiout.cpp --- MIDI出力のサンプルコード
// License: MIT
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <cstdio>

// MIDIイベントを送信
MMRESULT send_midi_event(HMIDIOUT hmo, int status, int data1, int data2)
{
    union {
        DWORD dwData;
        BYTE bData[4];
    } u;

    u.bData[0] = (BYTE)status;  // MIDI status byte
    u.bData[1] = (BYTE)data1;   // first MIDI data byte
    u.bData[2] = (BYTE)data2;   // second MIDI data byte
    u.bData[3] = 0;
    return midiOutShortMsg(hmo, u.dwData);
}

int main() {
    // MIDI出力を開く
    HMIDIOUT hMidiOut;
    MMRESULT result = midiOutOpen(&hMidiOut, 0, 0, 0, CALLBACK_NULL);
    if (result != MMSYSERR_NOERROR) {
        printf("Error opening MIDI output\n");
        return 1;
    }

    int ch0 = 0, ch1 = 1; // チャンネル
    send_midi_event(hMidiOut, 0xB0 + ch0, 0x79, 0x00); // リセットオールコントローラ
    send_midi_event(hMidiOut, 0xC0 + ch0, 30, 0); // プログラムチェンジ（音色）
    send_midi_event(hMidiOut, 0xB0 + ch0, 0x07, 106); // 音量を106に設定
    send_midi_event(hMidiOut, 0xB0 + ch0, 0x0A, 0); // パンを左に設定（127は右、64は中央、0は左）

    send_midi_event(hMidiOut, 0xB0 + ch1, 0x79, 0x00); // リセットオールコントローラ
    send_midi_event(hMidiOut, 0xC0 + ch1, 0, 0); // プログラムチェンジ（音色）
    send_midi_event(hMidiOut, 0xB0 + ch1, 0x07, 106); // 音量を106に設定
    send_midi_event(hMidiOut, 0xB0 + ch1, 0x0A, 127); // パンを右に設定（127は右、64は中央、0は左）

    // 実際に音を出す
    int octave = 4, tempo = 120, quantity = 6, max_quantity = 8;
    std::vector<int> notes = { 0, 2, 4, 5, 7, 9, 11, 12 }; // ドレミファソラシド
    for (int note : notes) {
        send_midi_event(hMidiOut, 0x90 + ch0, note + 12 * octave, 0x7F); // ノートオン（ベロシティ 127）
        send_midi_event(hMidiOut, 0x90 + ch1, note + 12 * octave, 0x7F); // ノートオン（ベロシティ 127）
        Sleep(1000 * 60 * quantity / tempo / max_quantity);
        send_midi_event(hMidiOut, 0x80 + ch0, note + 12 * octave, 0x00); // ノートオフ（ベロシティ 0）
        send_midi_event(hMidiOut, 0x80 + ch1, note + 12 * octave, 0x00); // ノートオフ（ベロシティ 0）
        Sleep(1000 * 60 * (max_quantity - quantity) / tempo / max_quantity);
    }

    send_midi_event(hMidiOut, 0xB0 + ch0, 0x78, 0x00); // オールサウンドオフ
    send_midi_event(hMidiOut, 0xB0 + ch1, 0x78, 0x00); // オールサウンドオフ
    midiOutClose(hMidiOut); // MIDI出力を閉じる
    return 0;
}
