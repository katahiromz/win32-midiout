#include <cstdio>
#include <vector>
#include <cstdint>

void write_u32(FILE *fout, uint32_t value) {
    std::fputc(uint8_t(value >> 24), fout);
    std::fputc(uint8_t(value >> 16), fout);
    std::fputc(uint8_t(value >> 8), fout);
    std::fputc(uint8_t(value), fout);
}

void write_u16(FILE *fout, uint16_t value) {
    std::fputc(uint8_t(value >> 8), fout);
    std::fputc(uint8_t(value), fout);
}

int main() {
    FILE *fout = _wfopen(L"output.mid", L"wb");
    if (!fout) {
        printf("Unable to open file\n");
        return 1;
    }

    int tempo = 120; // テンポ
    uint8_t num_tracks = 2; // トラック数
    uint8_t ticks_per_quarter_note = 120; // 4分音符 = 120 ticks
    uint32_t microseconds_per_quarter_note = 60 * 1000 * 1000 / tempo;

    // ヘッダーチャンク
    std::fwrite("MThd", 4, 1, fout);
    write_u32(fout, 6);
    write_u16(fout, 0);
    write_u16(fout, num_tracks);
    write_u16(fout, ticks_per_quarter_note);

    uint8_t ch = 0; // チャンネル (0～15)
    std::vector<unsigned char> trackData;

    // トラック1
    {
        int octave = 4;
        int quantity = 6, max_quantity = 8;
        int delta_time = 0;

        // トラックチャンク
        std::fwrite("MTrk", 4, 1, fout);

        // テンポ設定
        trackData.push_back(0x00);
        trackData.push_back(0xFF);
        trackData.push_back(0x51);
        trackData.push_back(0x03);
        trackData.push_back(uint8_t(microseconds_per_quarter_note >> 16));
        trackData.push_back(uint8_t(microseconds_per_quarter_note >> 8));
        trackData.push_back(uint8_t(microseconds_per_quarter_note));

        // プログラムチェンジイベント (音色変更)
        trackData.push_back(0x00); // デルタタイム
        trackData.push_back(0xC0 + ch); // プログラムチェンジ
        trackData.push_back(30); // 楽器番号

        // 音量を106に設定
        trackData.push_back(0x00); // デルタタイム
        trackData.push_back(0xB0 + ch); // 音量設定
        trackData.push_back(0x07); // 音量設定
        trackData.push_back(106); // 音量の値

        // パンを左に設定（127は右、64は中央、0は左）
        trackData.push_back(0x00); // デルタタイム
        trackData.push_back(0xB0 + ch); // パン設定
        trackData.push_back(0x0A); // パン設定
        trackData.push_back(0); // 左

        std::vector<int> notes = {0, 2, 4, 5, 7, 9, 11, 12}; // ドレミファソラシド
        for (auto note : notes) {
            trackData.push_back(delta_time); // デルタタイム
            trackData.push_back(0x90 + ch); // ノートオン
            trackData.push_back(uint8_t(12 * octave + note)); // ノートナンバー
            trackData.push_back(127); // ベロシティ
            delta_time = ticks_per_quarter_note * quantity / max_quantity;

            trackData.push_back(delta_time); // デルタタイム (4分音符)
            trackData.push_back(0x80 + ch); // ノートオフ
            trackData.push_back(uint8_t(12 * octave + note)); // ノートナンバー
            trackData.push_back(127); // ベロシティ
            delta_time = ticks_per_quarter_note * (max_quantity - quantity) / max_quantity;
        }

        trackData.push_back(delta_time); // デルタタイム (4分音符)
        trackData.push_back(0x80 + ch); // ノートオフ
        trackData.push_back(uint8_t(0)); // ノートナンバー
        trackData.push_back(127); // ベロシティ

        // エンドオブトラック
        trackData.push_back(0x00);
        trackData.push_back(0xFF);
        trackData.push_back(0x2F);
        trackData.push_back(0x00);

        write_u32(fout, trackData.size());
        std::fwrite(trackData.data(), trackData.size(), 1, fout);
    }

    // トラック2
    {
        int octave = 4;
        trackData.clear();
        ++ch;
        int quantity = 6, max_quantity = 8;
        int delta_time = 0;

        // トラックチャンク
        std::fwrite("MTrk", 4, 1, fout);

        // テンポ設定
        trackData.push_back(0x00);
        trackData.push_back(0xFF);
        trackData.push_back(0x51);
        trackData.push_back(0x03);
        trackData.push_back(uint8_t(microseconds_per_quarter_note >> 16));
        trackData.push_back(uint8_t(microseconds_per_quarter_note >> 8));
        trackData.push_back(uint8_t(microseconds_per_quarter_note));

        // プログラムチェンジイベント (音色変更)
        trackData.push_back(0x00); // デルタタイム
        trackData.push_back(0xC0 + ch); // プログラムチェンジ
        trackData.push_back(0); // 楽器番号

        // 音量を106に設定
        trackData.push_back(0x00); // デルタタイム
        trackData.push_back(0xB0 + ch); // 音量設定
        trackData.push_back(0x07); // 音量設定
        trackData.push_back(106); // 音量

        // パンを右に設定（127は右、64は中央、0は左）
        trackData.push_back(0x00); // デルタタイム
        trackData.push_back(0xB0 + ch); // パン設定
        trackData.push_back(0x0A); // パン設定
        trackData.push_back(127); // 右

        std::vector<int> notes = {0, 2, 4, 5, 7, 9, 11, 12}; // ドレミファソラシド
        for (auto note : notes) {
            trackData.push_back(delta_time); // デルタタイム
            trackData.push_back(0x90 + ch); // ノートオン
            trackData.push_back(uint8_t(12 * octave + note)); // ノートナンバー
            trackData.push_back(127); // ベロシティ
            delta_time = ticks_per_quarter_note * quantity / max_quantity;

            trackData.push_back(delta_time); // デルタタイム (4分音符)
            trackData.push_back(0x80 + ch); // ノートオフ
            trackData.push_back(uint8_t(12 * octave + note)); // ノートナンバー
            trackData.push_back(127); // ベロシティ
            delta_time = ticks_per_quarter_note * (max_quantity - quantity) / max_quantity;
        }

        trackData.push_back(delta_time); // デルタタイム (4分音符)
        trackData.push_back(0x80 + ch); // ノートオフ
        trackData.push_back(uint8_t(0)); // ノートナンバー
        trackData.push_back(127); // ベロシティ

        // エンドオブトラック
        trackData.push_back(0x00);
        trackData.push_back(0xFF);
        trackData.push_back(0x2F);
        trackData.push_back(0x00);

        write_u32(fout, trackData.size());
        std::fwrite(trackData.data(), trackData.size(), 1, fout);
    }

    printf("output.mid generated.\n");
    return 0;
}
