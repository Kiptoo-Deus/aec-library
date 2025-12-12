#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include "wav_io.hpp"
#include "aec/aec.hpp"

static void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " <far_wav> <near_wav> <out_wav> [options]\n";
    std::cerr << "Options:\n  --channels N (default autodetect from WAV)\n  --frame_size N (default 128)\n  --filter_length N (default 256)\n  --help\n";
}

int main(int argc, char** argv) {
    if (argc < 4) { print_usage(argv[0]); return 1; }
    std::string far_fn = argv[1];
    std::string near_fn = argv[2];
    std::string out_fn = argv[3];

    // parse options
    int argi = 4;
    uint32_t channels = 0;
    uint32_t frame_size = 128;
    uint32_t filter_length = 256;
    bool use_fixed_point = false;

    while (argi < argc) {
        if (std::strcmp(argv[argi], "--channels") == 0 && argi + 1 < argc) { channels = static_cast<uint32_t>(std::atoi(argv[++argi])); }
        else if (std::strcmp(argv[argi], "--frame_size") == 0 && argi + 1 < argc) { frame_size = static_cast<uint32_t>(std::atoi(argv[++argi])); }
        else if (std::strcmp(argv[argi], "--filter_length") == 0 && argi + 1 < argc) { filter_length = static_cast<uint32_t>(std::atoi(argv[++argi])); }
        else if (std::strcmp(argv[argi], "--fixed") == 0) { use_fixed_point = true; }
        else if (std::strcmp(argv[argi], "--help") == 0) { print_usage(argv[0]); return 0; }
        ++argi;
    }

    WavSpec far_spec, near_spec;
    std::vector<int16_t> far_data = read_wav_pcm16(far_fn, far_spec);
    std::vector<int16_t> near_data = read_wav_pcm16(near_fn, near_spec);

    if (far_spec.num_channels != near_spec.num_channels) {
        std::cerr << "channel count mismatch between far and near WAV\n";
        return 1;
    }

    if (channels == 0) channels = far_spec.num_channels;
    if (channels == 0) channels = 1;

    if (far_spec.sample_rate != near_spec.sample_rate) {
        std::cerr << "sample rate mismatch between far and near WAV\n";
        return 1;
    }

    // ensure data lengths match (or pick min)
    size_t frame_samples = frame_size * channels;
    size_t min_samples = std::min(far_data.size(), near_data.size());
    size_t frames = min_samples / frame_samples;
    size_t out_samples = frames * frame_samples;

    aec::AECConfig cfg;
    cfg.channels = channels;
    cfg.frame_size = frame_size;
    cfg.filter_length = filter_length;
    cfg.use_fixed_point = use_fixed_point;
    aec::AEC aec(cfg);

    std::vector<int16_t> out(out_samples);

    for (size_t f = 0; f < frames; ++f) {
        const int16_t* far_ptr = far_data.data() + f * frame_samples;
        const int16_t* near_ptr = near_data.data() + f * frame_samples;
        int16_t* out_ptr = out.data() + f * frame_samples;
        bool ok = aec.process(far_ptr, near_ptr, out_ptr, frame_size, channels);
        if (!ok) {
            std::cerr << "AEC processing failed at frame " << f << "\n";
            return 1;
        }
    }

    WavSpec out_spec = far_spec;
    write_wav_pcm16(out_fn, out_spec, out);
    std::cout << "Wrote: " << out_fn << " (" << out_samples / channels << " frames)\n";
    return 0;
}
