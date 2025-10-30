#include "../Source/DSP/PitchDetector.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>

// Simple test framework
#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    std::cout << "Running " #name "... "; \
    test_##name(); \
    std::cout << "PASSED\n"; \
} while(0)

// Generate a sine wave at a specific frequency
std::vector<float> generateSineWave(float frequency, float sampleRate, int numSamples) {
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

TEST(pitch_detector_initialization) {
    PitchDetector detector;
    detector.initialize(44100.0f, 2048);

    // Should not crash
    std::vector<float> testBuffer(2048, 0.0f);
    float pitch = detector.detectPitch(testBuffer.data(), 2048);

    // Silent signal should return 0 or low confidence
    assert(pitch == 0.0f || detector.getConfidence() < 0.5f);
}

TEST(pitch_detector_440hz) {
    PitchDetector detector;
    detector.initialize(44100.0f, 2048);

    // Generate 440 Hz sine wave (A4)
    auto testSignal = generateSineWave(440.0f, 44100.0f, 2048);

    float detectedPitch = detector.detectPitch(testSignal.data(), 2048);
    float confidence = detector.getConfidence();

    std::cout << "\n  Detected: " << detectedPitch << " Hz (expected 440 Hz)\n";
    std::cout << "  Confidence: " << confidence << "\n";

    // Allow 5% error margin
    assert(detectedPitch > 418.0f && detectedPitch < 462.0f);
    assert(confidence > 0.7f);
}

TEST(pitch_detector_262hz) {
    PitchDetector detector;
    detector.initialize(44100.0f, 2048);

    // Generate 262 Hz sine wave (C4)
    auto testSignal = generateSineWave(262.0f, 44100.0f, 2048);

    float detectedPitch = detector.detectPitch(testSignal.data(), 2048);
    float confidence = detector.getConfidence();

    std::cout << "\n  Detected: " << detectedPitch << " Hz (expected 262 Hz)\n";
    std::cout << "  Confidence: " << confidence << "\n";

    // Allow 5% error margin
    assert(detectedPitch > 249.0f && detectedPitch < 275.0f);
    assert(confidence > 0.6f);
}

TEST(pitch_detector_low_frequency) {
    PitchDetector detector;
    detector.initialize(44100.0f, 2048);

    // Generate 82 Hz sine wave (E2 - lowest guitar string)
    auto testSignal = generateSineWave(82.0f, 44100.0f, 2048);

    float detectedPitch = detector.detectPitch(testSignal.data(), 2048);

    std::cout << "\n  Detected: " << detectedPitch << " Hz (expected 82 Hz)\n";

    // Should detect or return 0 (out of range)
    assert(detectedPitch == 0.0f || (detectedPitch > 78.0f && detectedPitch < 86.0f));
}

TEST(pitch_detector_silence) {
    PitchDetector detector;
    detector.initialize(44100.0f, 2048);

    // Silent buffer
    std::vector<float> silence(2048, 0.0f);

    float detectedPitch = detector.detectPitch(silence.data(), 2048);
    float confidence = detector.getConfidence();

    std::cout << "\n  Detected pitch: " << detectedPitch << " Hz\n";
    std::cout << "  Confidence: " << confidence << "\n";

    // Should return 0 or very low confidence
    assert(detectedPitch == 0.0f || confidence < 0.3f);
}

TEST(pitch_detector_noise) {
    PitchDetector detector;
    detector.initialize(44100.0f, 2048);

    // Random noise
    std::vector<float> noise(2048);
    for (int i = 0; i < 2048; i++) {
        noise[i] = (float)rand() / RAND_MAX * 0.1f - 0.05f;
    }

    float detectedPitch = detector.detectPitch(noise.data(), 2048);
    float confidence = detector.getConfidence();

    std::cout << "\n  Detected pitch: " << detectedPitch << " Hz\n";
    std::cout << "  Confidence: " << confidence << "\n";

    // Should have low confidence
    assert(confidence < 0.5f);
}

int main() {
    std::cout << "========================================\n";
    std::cout << "FreeAutotune Pitch Detection Tests\n";
    std::cout << "========================================\n\n";

    try {
        RUN_TEST(pitch_detector_initialization);
        RUN_TEST(pitch_detector_440hz);
        RUN_TEST(pitch_detector_262hz);
        RUN_TEST(pitch_detector_low_frequency);
        RUN_TEST(pitch_detector_silence);
        RUN_TEST(pitch_detector_noise);

        std::cout << "\n========================================\n";
        std::cout << "All tests passed!\n";
        std::cout << "========================================\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\nTest failed with unknown exception\n";
        return 1;
    }
}
