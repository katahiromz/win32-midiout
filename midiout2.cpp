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

void write_variable_length(std::vector<unsigned char> &trackData, uint32_t value) {
    unsigned char buffer[4];
    int len = 0;
    buffer[len++] = value & 0x7F;
    while (value >>= 7) {
        buffer[len++] = (value & 0x7F) | 0x80;
    }
    for (int i = len - 1; i >= 0; --i) {
        trackData.push_back(buffer[i]);
    }
}

int main() {
    FILE *fout = _wfopen(L"output.mid", L"wb");
    if (!fout) {
        printf("Unable to open file\n");
        return 1;
    }

    int tempo = 120;
    uint8_t num_tracks = 3;
    uint16_t ticks_per_quarter_note = 120;
    uint32_t microseconds_per_quarter_note = 60 * 1000 * 1000 / tempo;

    // ヘッダーチャンク
    std::fwrite("MThd", 4, 1, fout);
    write_u32(fout, 6);
    write_u16(fout, 1);  // フォーマット 1
    write_u16(fout, num_tracks);
    write_u16(fout, ticks_per_quarter_note);

    std::vector<unsigned char> trackData;

    // トラック 1（テンポ情報）
    {
        std::fwrite("MTrk", 4, 1, fout);
        size_t trackSizePos = ftell(fout);
        write_u32(fout, 0);

        // テンポ設定
        trackData.push_back(0x00);
        trackData.push_back(0xFF);
        trackData.push_back(0x51);
        trackData.push_back(0x03);
        trackData.push_back(uint8_t(microseconds_per_quarter_note >> 16));
        trackData.push_back(uint8_t(microseconds_per_quarter_note >> 8));
        trackData.push_back(uint8_t(microseconds_per_quarter_note));

        // エンドオブトラック
        trackData.push_back(0x00);
        trackData.push_back(0xFF);
        trackData.push_back(0x2F);
        trackData.push_back(0x00);

        // トラックサイズとトラックデータを書き込む
        size_t trackSize = trackData.size();
        fseek(fout, trackSizePos, SEEK_SET);
        write_u32(fout, trackSize);
        fseek(fout, 0, SEEK_END);
        std::fwrite(trackData.data(), trackSize, 1, fout);
        trackData.clear();
    }

    // トラック 2（ノート情報）
    {
        std::fwrite("MTrk", 4, 1, fout);
        size_t trackSizePos = ftell(fout);
        write_u32(fout, 0);

        uint8_t ch = 0;
        int octave = 4;
        int quantity = 6, max_quantity = 8;
        int delta_time = 0;

        // プログラムチェンジ
        trackData.push_back(0x00);
        trackData.push_back(0xC0 + ch);
        trackData.push_back(30);

        // 音量設定
        trackData.push_back(0x00);
        trackData.push_back(0xB0 + ch);
        trackData.push_back(0x07);
        trackData.push_back(106);

        // パンを左に
        trackData.push_back(0x00);
        trackData.push_back(0xB0 + ch);
        trackData.push_back(0x0A);
        trackData.push_back(0); // パンを左に設定（127は右、64は中央、0は左）

        std::vector<int> notes = {0, 2, 4, 5, 7, 9, 11, 12}; // ドレミファソラシド

        for (auto note : notes) {
            // ノートオン
            write_variable_length(trackData, delta_time);
            trackData.push_back(0x90 + ch);
            trackData.push_back(uint8_t(12 * octave + note));
            trackData.push_back(127);
            delta_time = ticks_per_quarter_note * quantity / max_quantity;

            // ノートオフ
            write_variable_length(trackData, delta_time);
            trackData.push_back(0x80 + ch);
            trackData.push_back(uint8_t(12 * octave + note));
            trackData.push_back(127);
            delta_time = ticks_per_quarter_note * (max_quantity - quantity) / max_quantity;
        }

        // エンドオブトラック
        write_variable_length(trackData, delta_time);
        trackData.push_back(0xFF);
        trackData.push_back(0x2F);
        trackData.push_back(0x00);

        // トラックサイズとトラックデータを書き込む
        size_t trackSize = trackData.size();
        fseek(fout, trackSizePos, SEEK_SET);
        write_u32(fout, trackSize);
        fseek(fout, 0, SEEK_END);
        std::fwrite(trackData.data(), trackSize, 1, fout);
        trackData.clear();
    }

    // トラック 3（ノート情報）
    {
        std::fwrite("MTrk", 4, 1, fout);
        size_t trackSizePos = ftell(fout);
        write_u32(fout, 0);

        uint8_t ch = 1;
        int octave = 4;
        int quantity = 6, max_quantity = 8;
        int delta_time = 0;

        // プログラムチェンジ
        trackData.push_back(0x00);
        trackData.push_back(0xC0 + ch);
        trackData.push_back(0);

        // 音量設定
        trackData.push_back(0x00);
        trackData.push_back(0xB0 + ch);
        trackData.push_back(0x07);
        trackData.push_back(106);

        // パンを右に
        trackData.push_back(0x00);
        trackData.push_back(0xB0 + ch);
        trackData.push_back(0x0A);
        trackData.push_back(127); // パンを左に設定（127は右、64は中央、0は左）

        std::vector<int> notes = {0, 2, 4, 5, 7, 9, 11, 12}; // ドレミファソラシド

        for (auto note : notes) {
            // ノートオン
            write_variable_length(trackData, delta_time);
            trackData.push_back(0x90 + ch);
            trackData.push_back(uint8_t(12 * octave + note));
            trackData.push_back(127);
            delta_time = ticks_per_quarter_note * quantity / max_quantity;

            // ノートオフ
            write_variable_length(trackData, delta_time);
            trackData.push_back(0x80 + ch);
            trackData.push_back(uint8_t(12 * octave + note));
            trackData.push_back(127);
            delta_time = ticks_per_quarter_note * (max_quantity - quantity) / max_quantity;
        }

        // エンドオブトラック
        write_variable_length(trackData, delta_time);
        trackData.push_back(0xFF);
        trackData.push_back(0x2F);
        trackData.push_back(0x00);

        // トラックサイズとトラックデータを書き込む
        size_t trackSize = trackData.size();
        fseek(fout, trackSizePos, SEEK_SET);
        write_u32(fout, trackSize);
        fseek(fout, 0, SEEK_END);
        std::fwrite(trackData.data(), trackSize, 1, fout);
        trackData.clear();
    }

    printf("output.mid generated.\n");
    fclose(fout);
    return 0;
}
