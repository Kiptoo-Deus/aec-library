#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <fstream>

struct WavSpec {
    uint16_t audio_format; // 1 = PCM
    uint16_t num_channels;
    uint32_t sample_rate;
    uint16_t bits_per_sample; // expect 16
};

// Read PCM16 interleaved WAV file into vector<int16_t>
inline std::vector<int16_t> read_wav_pcm16(const std::string &path, WavSpec &spec) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("failed to open WAV: " + path);

    char riff[4];
    f.read(riff, 4);
    if (std::string(riff, 4) != "RIFF") throw std::runtime_error("not a RIFF file");
    uint32_t _;
    f.read(reinterpret_cast<char*>(&_), 4); // size
    char wave[4]; f.read(wave, 4);
    if (std::string(wave,4) != "WAVE") throw std::runtime_error("not a WAVE file");

    // find 'fmt ' chunk
    while (true) {
        char id[4]; if (!f.read(id,4)) throw std::runtime_error("fmt chunk not found");
        uint32_t len; f.read(reinterpret_cast<char*>(&len), 4);
        if (std::string(id,4) == "fmt ") {
            uint16_t audio_format; uint16_t num_channels; uint32_t sample_rate; uint32_t byte_rate; uint16_t block_align; uint16_t bits_per_sample;
            f.read(reinterpret_cast<char*>(&audio_format), sizeof(audio_format));
            f.read(reinterpret_cast<char*>(&num_channels), sizeof(num_channels));
            f.read(reinterpret_cast<char*>(&sample_rate), sizeof(sample_rate));
            f.read(reinterpret_cast<char*>(&byte_rate), sizeof(byte_rate));
            f.read(reinterpret_cast<char*>(&block_align), sizeof(block_align));
            f.read(reinterpret_cast<char*>(&bits_per_sample), sizeof(bits_per_sample));
            // skip any extra bytes
            if (len > 16) f.seekg(len - 16, std::ios::cur);
            spec.audio_format = audio_format;
            spec.num_channels = num_channels;
            spec.sample_rate = sample_rate;
            spec.bits_per_sample = bits_per_sample;
            break;
        } else {
            f.seekg(len, std::ios::cur);
        }
    }

    // find 'data' chunk
    uint32_t data_bytes = 0;
    while (true) {
        char id[4]; if (!f.read(id,4)) throw std::runtime_error("data chunk not found");
        uint32_t len; f.read(reinterpret_cast<char*>(&len), 4);
        if (std::string(id,4) == "data") {
            data_bytes = len; break;
        } else {
            f.seekg(len, std::ios::cur);
        }
    }

    if (spec.audio_format != 1) throw std::runtime_error("only PCM WAV supported");
    if (spec.bits_per_sample != 16) throw std::runtime_error("only 16-bit WAV supported");

    size_t samples = data_bytes / 2; // 2 bytes per sample
    std::vector<int16_t> out(samples);
    f.read(reinterpret_cast<char*>(out.data()), data_bytes);
    if (!f) throw std::runtime_error("failed to read WAV data");
    return out;
}

// Write PCM16 interleaved WAV
inline void write_wav_pcm16(const std::string &path, const WavSpec &spec, const std::vector<int16_t> &data) {
    std::ofstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("failed to open WAV for write: " + path);

    auto write_u32 = [&](uint32_t v){ f.write(reinterpret_cast<const char*>(&v), 4); };
    auto write_u16 = [&](uint16_t v){ f.write(reinterpret_cast<const char*>(&v), 2); };

    f.write("RIFF",4);
    uint32_t file_size_minus8 = 36 + static_cast<uint32_t>(data.size())*2/2; // we'll correct below
    write_u32(0); // placeholder
    f.write("WAVE",4);

    // fmt chunk
    f.write("fmt ",4);
    write_u32(16);
    write_u16(spec.audio_format);
    write_u16(spec.num_channels);
    write_u32(spec.sample_rate);
    uint32_t byte_rate = spec.sample_rate * spec.num_channels * spec.bits_per_sample / 8;
    write_u32(byte_rate);
    uint16_t block_align = spec.num_channels * spec.bits_per_sample / 8;
    write_u16(block_align);
    write_u16(spec.bits_per_sample);

    // data chunk
    f.write("data",4);
    uint32_t data_bytes = static_cast<uint32_t>(data.size()) * sizeof(int16_t);
    write_u32(data_bytes);
    f.write(reinterpret_cast<const char*>(data.data()), data_bytes);

    // finalize file size
    uint32_t file_size = 4 + (8 + 16) + (8 + data_bytes);
    f.seekp(4, std::ios::beg);
    write_u32(file_size);
}
