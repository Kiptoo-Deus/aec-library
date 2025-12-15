#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "aec/webrtc_adapter.h"
int main() {
    aec::AECConfig config;
    config.sample_rate = 16000;
    config.frame_size = 160;
    config.filter_length = 512;
    config.use_fixed_point = true;
    aec::webrtc::WebRTCAecAdapter adapter;
    if (!adapter.Init(config, config.sample_rate)) {
        std::cerr << "Failed to initialize WebRTCAecAdapter" << std::endl;
        return 1;
    }
    std::vector<int16_t> far(config.frame_size * config.channels, 1000);
    std::vector<int16_t> near(config.frame_size * config.channels, 2000);
    std::cout << "Starting simulated real-time demo (no audio I/O).\n";
    std::cout << "Processing 100 frames, AEC enabled.\n";
    for (int i = 0; i < 100; ++i) {
        adapter.ProcessRender(far.data());
        adapter.ProcessCapture(near.data());
        if ((i % 25) == 0) {
            std::cout << "Frame " << i << " - ERLE: " << adapter.GetErle() << " dB, Latency: " << adapter.GetLatencyMs() << " ms\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "Demo finished." << std::endl;
    return 0;
}
