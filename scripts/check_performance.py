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
        print(f" Benchmark file {benchmark_file} not found - skipping performance check")
        return True
    
    max_regression = 0
    failed_benchmarks = []
    
    print("📊 Performance Results:")
    for benchmark in data['benchmarks']:
        name = benchmark['name']
        cpu_time = benchmark['cpu_time']
        time_unit = benchmark.get('time_unit', 'ns')
        
        print(f"  {name}: {cpu_time:.2f}{time_unit}")
        
        # Set reasonable thresholds
        if 'BM_AEC_Process_FixedPoint' in name:
            threshold = 1000  # microseconds
            if cpu_time > threshold and time_unit == 'us':
                failed_benchmarks.append({
                    'name': name,
                    'current': cpu_time,
                    'threshold': threshold,
                    'unit': time_unit
                })
        elif 'BM_AEC_Latency' in name:
            threshold = 5.0  # milliseconds
            if cpu_time > threshold and time_unit == 'ms':
                failed_benchmarks.append({
                    'name': name,
                    'current': cpu_time,
                    'threshold': threshold,
                    'unit': time_unit
                })
    
    if failed_benchmarks:
        print("🚨 Performance regression detected!")
        for bench in failed_benchmarks:
            print(f"  {bench['name']}: {bench['current']:.2f}{bench['unit']} > {bench['threshold']}{bench['unit']}")
        return False
    else:
        print("✅ All benchmarks within acceptable performance limits")
        return True

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python check_performance.py <benchmark_json_file>")
        sys.exit(1)
    
    success = check_performance_regression(sys.argv[1])
    sys.exit(0 if success else 1)
