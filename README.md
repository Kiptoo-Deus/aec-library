# Acoustic Echo Cancellation (AEC) Library

A high-performance, real-time Acoustic Echo Cancellation library written in C++ for VoIP and teleconferencing applications.

##  Features

- **Real-time Processing**: <5ms latency for VoIP scenarios
- **Adaptive Filtering**: NLMS algorithm with fixed-point optimization
- **Double-talk Detection**: Robust, coherence- and energy-based detector to freeze adaptation during near-end speech (configurable thresholds and hangover)
- **Multi-channel Support**: Process interleaved input with up to **8** channels (configurable via `AECConfig::channels`). Per-channel NLMS filters and per-channel DTDs are used.
- **Production Ready**: Comprehensive tests, benchmarks, and CI/CD
- **Cross-platform**: Linux, macOS, Windows support
- **Modern C++**: C++11 with RAII and Pimpl idiom

##  Performance

- **Latency**: <5ms end-to-end processing
- **Echo Reduction**: >25dB ERLE (Echo Return Loss Enhancement)
- **CPU Usage**: Optimized fixed-point arithmetic for embedded systems

git clone 

## 🛠 Quick Start

### Prerequisites

- CMake >= 3.15
- Conan (dependency manager)
- Ninja (optional, for faster builds)
- Android NDK (for Android builds)

#### Install Tools
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake ninja
pip install conan

# macOS
brew install cmake ninja
pip3 install conan

# Windows (vcpkg)
vcpkg install cmake ninja gtest benchmark
pip install conan
```

### Clone and Build (Desktop)
```bash
git clone https://github.com/Kiptoo-Deus/aec-library.git
cd aec-library
conan install . --build=missing
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### Run Tests, Benchmarks, Example
```bash
cd build && ctest --output-on-failure
./benchmarks/aec_benchmark
./examples/basic_usage
```

### Build for Android
# Build for iOS
```bash
# Configure for iOS (arm64)
cmake -B build-ios \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DAEC_SRC="src/aec.cpp;src/fixed_point.cpp;src/nlms_filter.cpp"
cmake --build build-ios --parallel
# The output will be aec.framework (for iOS integration)
```
```bash
# Set your NDK path and ABI (e.g. arm64-v8a, armeabi-v7a, x86_64)
export ANDROID_NDK_HOME=/path/to/ndk
cmake -B build-android-arm64 \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21
cmake --build build-android-arm64 --parallel
# Repeat for other ABIs
```

#### JNI Integration (Android)
- Copy the produced `.so` files from `build-android-*` to your Android project's `app/src/main/jniLibs/<ABI>/`.
- Use the provided `AecWrapper.java` in your Android app.
- Load the library in Java: `System.loadLibrary("aec");`



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

## Double-talk Detection (DTD)

The library includes a configurable double-talk detector enabled by default. Configure behavior via `AECConfig` fields:

- `enable_double_talk_detection` (bool)
- `dtd_near_to_far_threshold` (float)
- `dtd_coherence_threshold` (float)
- `dtd_smoothing_alpha` (float)
- `dtd_hangover_frames` (uint32_t)

The detector uses smoothed near/far energies and a coherence estimate to decide whether to freeze adaptation when near-end speech is present.


## Acknowledgments
# Based on classical adaptive filtering theory

# Inspired by SpeexDSP and WebRTC AEC implementations

# Google Test and Benchmark for testing infrastructure
