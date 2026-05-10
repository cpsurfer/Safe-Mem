#!/bin/bash

set -e

echo "🚀 Starting Safe-Mem Optimized Execution Suite..."

if [ ! -f "Makefile" ]; then
    echo "❌ Error: Makefile not found! Sahi directory mein check karein."
    exit 1
fi

echo "🛠️  Building Static & Shared Libraries (MODE: RELEASE)..."

# Build everything
make clean
make MODE=release -j$(nproc) 

echo "🧪 Running Hardware Safety Checks (SIMD)..."
./simdtest

echo "✅ Running Functional Tests (Logic Validation)..."
./test_driver

if [ -f "./benchmark" ]; then
    echo "📊 Running Performance Benchmarks (JSON Export)..."
    rm -f result.json
    
    # Core pinning for accuracy
    echo "📌 Pinning to Isolated Core 1 for Maximum Accuracy..."
    taskset -c 1 ./benchmark --benchmark_out=result.json --benchmark_out_format=json
    
    if [ -f "scripts/perf_charts.py" ]; then
        echo "📈 Generating Performance Charts..."
        if [ -d ".venv" ]; then
            source .venv/bin/activate
            python3 scripts/perf_charts.py
            deactivate
        else
            python3 scripts/perf_charts.py
        fi
        echo "✅ Charts updated in assets/ folder."
    fi
else
    echo "⚠️  Warning: benchmark binary not found, skipping performance tests..."
fi

echo "🏁 All tasks completed! Your shared library (libfmem.so) is ready for Python/C."
