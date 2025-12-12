## WAV AEC Example

This project includes a simple command-line example `wav_aec` that runs the AEC on two PCM16 WAV files:

Usage:

```
examples/wav_aec <far_wav> <near_wav> <out_wav> [--channels N] [--frame_size N] [--filter_length N] [--fixed]
```

- `far_wav`: far-end signal (usually the playback/reference) in 16-bit PCM WAV
- `near_wav`: near-end signal (microphone + echo) in 16-bit PCM WAV
- `out_wav`: output file written as 16-bit PCM WAV

Notes:
- This is a simple example intended for testing and demonstration (no resampling/format conversion).
- Use the `--fixed` flag to exercise the fixed-point code paths.
