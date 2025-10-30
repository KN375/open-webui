#include "../Source/DSP/PitchDetector.hpp"
#include "../Source/DSP/PitchShifter.hpp"
#include "../Source/DSP/AudioProcessor.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

using namespace std::chrono;

// Generate test audio
std::vector<float> generateTestAudio(float frequency, float sampleRate, int numSamples) {
    std::vector<float> buffer(numSamples);
    float phase = 0.0f;
    float phaseIncrement = 2.0f * M_PI * frequency / sampleRate;

    for (int i = 0; i < numSamples; i++) {
        buffer[i] = 0.5f * std::sin(phase);
        phase += phaseIncrement;
        if (phase >= 2.0f * M_PI) {
            phase -= 2.0f * M_PI;
        }
    }

    return buffer;
}

void benchmarkPitchDetection() {
    std::cout << "\n=== Pitch Detection Benchmark ===\n";

    PitchDetector detector;
    const float sampleRate = 44100.0f;
    const int bufferSize = 2048;
    const int iterations = 1000;

    detector.initialize(sampleRate, bufferSize);

    // Generate test signal
    auto testSignal = generateTestAudio(440.0f, sampleRate, bufferSize);

    // Warmup
    for (int i = 0; i < 10; i++) {
        detector.detectPitch(testSignal.data(), bufferSize);
    }

    // Benchmark
    auto start = high_resolution_clock::now();

    for (int i = 0; i < iterations; i++) {
        detector.detectPitch(testSignal.data(), bufferSize);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();

    double avgTime = (double)duration / iterations;
    double realTimeRatio = (bufferSize / sampleRate * 1000000.0) / avgTime;

    std::cout << "Buffer size: " << bufferSize << " samples\n";
    std::cout << "Iterations: " << iterations << "\n";
    std::cout << "Average time: " << avgTime << " μs\n";
    std::cout << "Real-time ratio: " << realTimeRatio << "x\n";
    std::cout << "CPU usage (approx): " << (100.0 / realTimeRatio) << "%\n";
}

void benchmarkPitchShifting() {
    std::cout << "\n=== Pitch Shifting Benchmark ===\n";

    PitchShifter shifter;
    const float sampleRate = 44100.0f;
    const int bufferSize = 512;
    const int iterations = 1000;

    shifter.initialize(sampleRate, 8192);

    // Generate test signal
    auto testInput = generateTestAudio(440.0f, sampleRate, bufferSize);
    std::vector<float> testOutput(bufferSize);

    // Warmup
    for (int i = 0; i < 10; i++) {
        shifter.process(testInput.data(), testOutput.data(), bufferSize, 1.1f);
    }

    // Benchmark
    auto start = high_resolution_clock::now();

    for (int i = 0; i < iterations; i++) {
        shifter.process(testInput.data(), testOutput.data(), bufferSize, 1.1f);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();

    double avgTime = (double)duration / iterations;
    double realTimeRatio = (bufferSize / sampleRate * 1000000.0) / avgTime;

    std::cout << "Buffer size: " << bufferSize << " samples\n";
    std::cout << "Iterations: " << iterations << "\n";
    std::cout << "Average time: " << avgTime << " μs\n";
    std::cout << "Real-time ratio: " << realTimeRatio << "x\n";
    std::cout << "CPU usage (approx): " << (100.0 / realTimeRatio) << "%\n";
}

void benchmarkFullProcessor() {
    std::cout << "\n=== Full Audio Processor Benchmark ===\n";

    AudioProcessor processor;
    const float sampleRate = 44100.0f;
    const int bufferSize = 512;
    const int iterations = 1000;

    processor.initialize(sampleRate, 8192);

    // Generate test signal
    auto testInput = generateTestAudio(440.0f, sampleRate, bufferSize);
    std::vector<float> testOutput(bufferSize);

    // Warmup
    for (int i = 0; i < 10; i++) {
        processor.process(testInput.data(), testOutput.data(), bufferSize,
                         50.0f, 0, 2, 100.0f, 80.0f);
    }

    // Benchmark
    auto start = high_resolution_clock::now();

    for (int i = 0; i < iterations; i++) {
        processor.process(testInput.data(), testOutput.data(), bufferSize,
                         50.0f, 0, 2, 100.0f, 80.0f);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();

    double avgTime = (double)duration / iterations;
    double realTimeRatio = (bufferSize / sampleRate * 1000000.0) / avgTime;

    std::cout << "Buffer size: " << bufferSize << " samples\n";
    std::cout << "Iterations: " << iterations << "\n";
    std::cout << "Average time: " << avgTime << " μs\n";
    std::cout << "Real-time ratio: " << realTimeRatio << "x\n";
    std::cout << "CPU usage (approx): " << (100.0 / realTimeRatio) << "%\n";
    std::cout << "Latency: " << (bufferSize / sampleRate * 1000.0) << " ms\n";
}

void benchmarkDifferentBufferSizes() {
    std::cout << "\n=== Buffer Size Comparison ===\n";

    AudioProcessor processor;
    const float sampleRate = 44100.0f;
    const int iterations = 500;

    int bufferSizes[] = {128, 256, 512, 1024, 2048};

    for (int bufferSize : bufferSizes) {
        processor.initialize(sampleRate, bufferSize * 4);

        auto testInput = generateTestAudio(440.0f, sampleRate, bufferSize);
        std::vector<float> testOutput(bufferSize);

        // Warmup
        for (int i = 0; i < 10; i++) {
            processor.process(testInput.data(), testOutput.data(), bufferSize,
                             50.0f, 0, 2, 100.0f, 80.0f);
        }

        // Benchmark
        auto start = high_resolution_clock::now();

        for (int i = 0; i < iterations; i++) {
            processor.process(testInput.data(), testOutput.data(), bufferSize,
                             50.0f, 0, 2, 100.0f, 80.0f);
        }

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();

        double avgTime = (double)duration / iterations;
        double latency = bufferSize / sampleRate * 1000.0;
        double realTimeRatio = (bufferSize / sampleRate * 1000000.0) / avgTime;

        std::cout << "\nBuffer: " << bufferSize << " samples\n";
        std::cout << "  Latency: " << latency << " ms\n";
        std::cout << "  Processing time: " << avgTime << " μs\n";
        std::cout << "  Real-time ratio: " << realTimeRatio << "x\n";
        std::cout << "  CPU usage: ~" << (100.0 / realTimeRatio) << "%\n";
    }
}

int main() {
    std::cout << "========================================\n";
    std::cout << "FreeAutotune Performance Benchmarks\n";
    std::cout << "========================================\n";

    try {
        benchmarkPitchDetection();
        benchmarkPitchShifting();
        benchmarkFullProcessor();
        benchmarkDifferentBufferSizes();

        std::cout << "\n========================================\n";
        std::cout << "Benchmark complete!\n";
        std::cout << "========================================\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nBenchmark failed: " << e.what() << "\n";
        return 1;
    }
}
