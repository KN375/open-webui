#include "PitchShifter.hpp"
#include <Accelerate/Accelerate.h>

PitchShifter::PitchShifter()
    : sampleRate_(44100.0f)
    , maxBufferSize_(8192)
    , historySize_(HISTORY_SAMPLES)
    , writeIndex_(0)
{
}

PitchShifter::~PitchShifter() {
}

void PitchShifter::initialize(float sampleRate, int maxBufferSize) {
    sampleRate_ = sampleRate;
    maxBufferSize_ = maxBufferSize;

    // Allocate history buffers
    inputHistory_.resize(HISTORY_SAMPLES, 0.0f);
    outputHistory_.resize(HISTORY_SAMPLES, 0.0f);

    // Pre-compute Hanning window
    hanningWindow_.resize(HISTORY_SAMPLES);
    for (int i = 0; i < HISTORY_SAMPLES; i++) {
        hanningWindow_[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (HISTORY_SAMPLES - 1)));
    }

    peakPositions_.reserve(MAX_PEAKS);
    writeIndex_ = 0;
}

void PitchShifter::reset() {
    std::fill(inputHistory_.begin(), inputHistory_.end(), 0.0f);
    std::fill(outputHistory_.begin(), outputHistory_.end(), 0.0f);
    writeIndex_ = 0;
}

void PitchShifter::process(const float* inputBuffer, float* outputBuffer,
                           int bufferSize, float pitchRatio) {
    if (pitchRatio <= 0.0f) {
        pitchRatio = 1.0f;
    }

    // If pitch ratio is very close to 1.0, just copy input to output
    if (std::abs(pitchRatio - 1.0f) < 0.001f) {
        std::copy(inputBuffer, inputBuffer + bufferSize, outputBuffer);
        return;
    }

    // Add input to history buffer
    for (int i = 0; i < bufferSize; i++) {
        inputHistory_[writeIndex_] = inputBuffer[i];
        writeIndex_ = (writeIndex_ + 1) % historySize_;
    }

    // Apply PSOLA pitch shifting
    applyPSOLA(inputBuffer, outputBuffer, bufferSize, pitchRatio);
}

void PitchShifter::findPeaks(const float* buffer, int bufferSize, float period) {
    peakPositions_.clear();

    if (period < MIN_PERIOD || period > MAX_PERIOD) {
        period = 100.0f; // Default period
    }

    // Find peaks using simple peak detection
    int step = static_cast<int>(period * 0.5f);
    if (step < 1) step = 1;

    for (int i = step; i < bufferSize - step; i += step) {
        float maxVal = 0.0f;
        int maxPos = i;

        // Find local maximum in window
        for (int j = i - step; j < i + step && j < bufferSize; j++) {
            float absVal = std::abs(buffer[j]);
            if (absVal > maxVal) {
                maxVal = absVal;
                maxPos = j;
            }
        }

        if (maxVal > 0.01f) { // Threshold for peak detection
            peakPositions_.push_back(maxPos);
        }
    }
}

float PitchShifter::getHanningWindow(int index, int windowSize) {
    if (index < 0 || index >= windowSize || windowSize >= hanningWindow_.size()) {
        return 0.0f;
    }

    // Scale index to Hanning window
    float scaledIndex = (float)index * (hanningWindow_.size() - 1) / windowSize;
    int idx = static_cast<int>(scaledIndex);

    if (idx >= hanningWindow_.size() - 1) {
        return hanningWindow_.back();
    }

    // Linear interpolation
    float frac = scaledIndex - idx;
    return hanningWindow_[idx] * (1.0f - frac) + hanningWindow_[idx + 1] * frac;
}

void PitchShifter::applyPSOLA(const float* inputBuffer, float* outputBuffer,
                              int bufferSize, float pitchRatio) {
    // Clear output buffer
    std::fill(outputBuffer, outputBuffer + bufferSize, 0.0f);

    // Estimate period from pitch ratio
    float basePeriod = 100.0f; // Default period at 441 Hz for 44.1kHz
    float inputPeriod = basePeriod;
    float outputPeriod = inputPeriod * pitchRatio;

    // Find peaks in input
    findPeaks(inputBuffer, bufferSize, inputPeriod);

    if (peakPositions_.empty()) {
        // No peaks found, just copy input
        std::copy(inputBuffer, inputBuffer + bufferSize, outputBuffer);
        return;
    }

    // PSOLA: Overlap and add grains at new positions
    int outputPos = 0;

    for (size_t peakIdx = 0; peakIdx < peakPositions_.size() && outputPos < bufferSize; peakIdx++) {
        int peakPos = peakPositions_[peakIdx];

        // Window size around peak
        int windowSize = static_cast<int>(inputPeriod * 2.0f);
        if (windowSize > bufferSize / 2) {
            windowSize = bufferSize / 2;
        }

        int startPos = peakPos - windowSize / 2;
        int endPos = peakPos + windowSize / 2;

        // Apply windowed grain
        for (int i = 0; i < windowSize && outputPos + i < bufferSize; i++) {
            int inputIdx = startPos + i;

            if (inputIdx >= 0 && inputIdx < bufferSize) {
                float windowValue = getHanningWindow(i, windowSize);
                outputBuffer[outputPos + i] += inputBuffer[inputIdx] * windowValue;
            }
        }

        // Move output position by output period
        outputPos += static_cast<int>(outputPeriod);
    }

    // Normalize output to prevent clipping
    float maxVal = 0.0f;
    vDSP_maxmgv(outputBuffer, 1, &maxVal, bufferSize);

    if (maxVal > 1.0f) {
        float scale = 0.95f / maxVal;
        vDSP_vsmul(outputBuffer, 1, &scale, outputBuffer, 1, bufferSize);
    }
}
