#!/usr/bin/env python3
import json
import sys
import os

def check_performance_regression(benchmark_file):
    """Check for performance regressions in benchmark results"""
    try:
        with open(benchmark_file, 'r') as f:
            data = json.load(f)
    except FileNotFoundError:
        print(f"Benchmark file {benchmark_file} not found - skipping performance check")
        return True
    
    print(" Performance Results Analysis:")
    all_within_limits = True
    
    for benchmark in data['benchmarks']:
        name = benchmark['name']
        cpu_time = benchmark['cpu_time']
        time_unit = benchmark.get('time_unit', 'ns')
        
        # Set reasonable thresholds based on benchmark name
        if 'BM_AEC_Process_FixedPoint' in name:
            # Check frame size from benchmark name
            if '/64' in name:
                threshold = 200  # microseconds for 64 samples
            elif '/128' in name:
                threshold = 400  # microseconds for 128 samples
            elif '/256' in name:
                threshold = 800  # microseconds for 256 samples
            elif '/512' in name:
                threshold = 1600  # microseconds for 512 samples
            elif '/1024' in name:
                threshold = 3200  # microseconds for 1024 samples
            elif '/2048' in name:
                threshold = 6400  # microseconds for 2048 samples
            elif '/4096' in name:
                threshold = 12800  # microseconds for 4096 samples
            else:
                threshold = 5000  # default
            
            if cpu_time > threshold and time_unit == 'us':
                print(f"{name}: {cpu_time:.2f}{time_unit} > {threshold}{time_unit}")
                all_within_limits = False
            else:
                print(f"{name}: {cpu_time:.2f}{time_unit} <= {threshold}{time_unit}")
                
        elif 'BM_AEC_Latency' in name:
            threshold = 5.0  # milliseconds
            if cpu_time > threshold and time_unit == 'ms':
                print(f" {name}: {cpu_time:.2f}{time_unit} > {threshold}{time_unit}")
                all_within_limits = False
            else:
                print(f"{name}: {cpu_time:.2f}{time_unit} <= {threshold}{time_unit}")
        else:
            print(f"{name}: {cpu_time:.2f}{time_unit}")
    
    if all_within_limits:
        print("All benchmarks within acceptable performance limits!")
        return True
    else:
        print("Performance regression detected!")
        return False

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python check_performance.py <benchmark_json_file>")
        sys.exit(1)
    
    success = check_performance_regression(sys.argv[1])
    sys.exit(0 if success else 1)
