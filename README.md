# Acoustic Echo Cancellation (AEC) Library

A high-performance, real-time Acoustic Echo Cancellation library written in C++ for VoIP and teleconferencing applications.

## 🚀 Features

- **Real-time Processing**: <5ms latency for VoIP scenarios
- **Adaptive Filtering**: NLMS algorithm with fixed-point optimization
- **Production Ready**: Comprehensive tests, benchmarks, and CI/CD
- **Cross-platform**: Linux, macOS, Windows support
- **Modern C++**: C++11 with RAII and Pimpl idiom

## 📊 Performance

- **Latency**: <5ms end-to-end processing
- **Echo Reduction**: >25dB ERLE (Echo Return Loss Enhancement)
- **CPU Usage**: Optimized fixed-point arithmetic for embedded systems

## 🛠 Quick Start

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake ninja-build

# macOS
brew install cmake ninja

# Windows (vcpkg)
vcpkg install cmake ninja gtest benchmark

git clone 
cd aec-library

# Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# Run tests
cd build && ctest --output-on-failure

# Run benchmarks
./benchmarks/aec_benchmark

# Run example
./examples/basic_usage



# Example Usage
#include "aec/aec.hpp"
#include <vector>

int main() {
    // Configure AEC
    aec::AECConfig config;
    config.sample_rate = 16000;
    config.frame_size = 256;
    config.use_fixed_point = true;
    
    auto aec = aec::create_aec(config);
    
    // Process audio frames
    std::vector<int16_t> far_end(256);
    std::vector<int16_t> near_end(256);
    std::vector<int16_t> output(256);
    
    aec->process(far_end.data(), near_end.data(), output.data(), 256);
    
    return 0;
}


aec-library/
├── include/aec/         # Public headers
├── src/                 # Implementation
├── tests/               # Unit tests
├── benchmarks/          # Performance benchmarks
├── examples/            # Usage examples
└── scripts/             # Utility scripts


# API Reference
## Core Classes

1) aec::AEC: Main echo canceller interface

2) aec::AECConfig: Configuration parameters

3) aec::NLMSFilter: Normalized Least Mean Squares adaptive filter

## Key Methods
process(): Real-time audio processing

reset(): Reset filter state

get_latency_ms(): Get current processing latency

get_erle(): Get echo cancellation performance


## Acknowledgments
# Based on classical adaptive filtering theory

# Inspired by SpeexDSP and WebRTC AEC implementations

# Google Test and Benchmark for testing infrastructure
