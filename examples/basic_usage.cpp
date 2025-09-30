#include "aec/aec.hpp"
#include <iostream>
#include <vector>
#include <random>

int main() {
    std::cout << "AEC Library Demo - Real-time Echo Cancellation" << std::endl;
    
    // Configure AEC for VoIP scenario
    aec::AECConfig config;
    config.sample_rate = 16000;
    config.frame_size = 256;
    config.filter_length = 512;
    config.use_fixed_point = true;
    config.mu = 0.1f;
    
    auto aec = aec::create_aec(config);
    
    // Generate synthetic audio data (far-end + echo in near-end)
    std::vector<int16_t> far_end(config.frame_size);
    std::vector<int16_t> near_end(config.frame_size);
    std::vector<int16_t> output(config.frame_size);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int16_t> dis(-1000, 1000);
    
    // Simulate: near_end = far_end * 0.3 + noise
    for (int i = 0; i < config.frame_size; ++i) {
        far_end[i] = dis(gen);
        near_end[i] = static_cast<int16_t>(far_end[i] * 0.3) + dis(gen) / 10;
    }
    
    std::cout << "Processing " << config.frame_size << " sample frames..." << std::endl;
    
    // Process multiple frames
    for (int frame = 0; frame < 50; ++frame) {
        bool success = aec->process(
            far_end.data(),
            near_end.data(),
            output.data(),
            config.frame_size
        );
        
        if (!success) {
            std::cerr << "Error processing frame " << frame << std::endl;
            return 1;
        }
        
        if (frame % 10 == 0) {
            std::cout << "Frame " << frame
                      << " | Latency: " << aec->get_latency_ms() << "ms"
                      << " | ERLE: " << aec->get_erle() << "dB"
                      << std::endl;
        }
    }
    
    std::cout << "Demo completed successfully!" << std::endl;
    std::cout << "Final performance:" << std::endl;
    std::cout << "  - Average latency: " << aec->get_latency_ms() << "ms" << std::endl;
    std::cout << "  - Echo reduction: " << aec->get_erle() << "dB" << std::endl;
    
    return 0;
}
