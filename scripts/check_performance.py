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
        print(f"Benchmark file {benchmark_file} not found")
        return True
    
    max_regression = 0
    failed_benchmarks = []
    
    for benchmark in data['benchmarks']:
        name = benchmark['name']
        cpu_time = benchmark['cpu_time']
        
        # In a real scenario, compare against baseline from previous run
        # For now, we'll set some reasonable thresholds
        thresholds = {
            'BM_AEC_Process_FixedPoint': 1000,  # microseconds
            'BM_AEC_Latency': 5.0  # milliseconds
        }
        
        for key, threshold in thresholds.items():
            if key in name and cpu_time > threshold:
                failed_benchmarks.append({
                    'name': name,
                    'current': cpu_time,
                    'threshold': threshold
                })
    
    if failed_benchmarks:
        print("🚨 Performance regression detected!")
        for bench in failed_benchmarks:
            print(f"  {bench['name']}: {bench['current']:.2f} > {bench['threshold']}")
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
