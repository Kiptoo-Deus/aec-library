#!/usr/bin/env python3
import numpy as np
import scipy.io.wavfile as wavfile

def generate_test_signals():
    """Generate synthetic test signals for AEC testing"""
    sample_rate = 16000
    duration = 3  # seconds
    t = np.linspace(0, duration, sample_rate * duration)
    
    # Far-end signal (clean speech-like with multiple frequencies)
    far_end = (np.sin(2 * np.pi * 800 * t) * 0.6 +
               np.sin(2 * np.pi * 1200 * t) * 0.3 +
               np.sin(2 * np.pi * 400 * t) * 0.1)
    
    # Near-end signal with echo and noise
    echo_delay = 32  # samples
    echo_attenuation = 0.4
    
    # Create delayed echo
    echo_signal = np.zeros_like(far_end)
    echo_signal[echo_delay:] = far_end[:-echo_delay] * echo_attenuation
    
    near_end = echo_signal + np.random.normal(0, 0.02, len(t))
    
    # Convert to 16-bit PCM
    far_end_int16 = (far_end * 32767).astype(np.int16)
    near_end_int16 = (near_end * 32767).astype(np.int16)
    
    # Save as WAV files
    wavfile.write('test_far_end.wav', sample_rate, far_end_int16)
    wavfile.write('test_near_end.wav', sample_rate, near_end_int16)
    
    print(" Generated test audio files:")
    print("   - test_far_end.wav (clean signal)")
    print("   - test_near_end.wav (signal with echo + noise)")

if __name__ == "__main__":
    generate_test_signals()
