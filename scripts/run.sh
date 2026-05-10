#!/bin/bash

set -e

echo "🚀 Starting Safe-Mem Execution Suite..."

if [ ! -f "Makefile" ]; then
    echo "❌ Error: Makefile not found! Check in correct directory."
    exit 1
fi

echo "🛠️  Cleaning and Building project..."

make clean
make -j$(nproc) 

echo "🧪 Running Hardware Safety Checks..."

./simdtest

echo "✅ Running Functional Tests (Logic Validation)..."

./test_driver

echo "📊 Running Performance Benchmarks..."

if [ -f "./benchmark" ]; then
    echo "📌 Pinning benchmark to Cores 0-7..."
    taskset -c 0-7 ./benchmark
else
    echo "⚠️  Warning: benchmark binary not found, skipping..."
fi

echo "🏁 All tasks completed successfully!"
