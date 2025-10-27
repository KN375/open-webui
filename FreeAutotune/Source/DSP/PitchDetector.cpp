#include "PitchDetector.hpp"
#include <Accelerate/Accelerate.h>

PitchDetector::PitchDetector()
    : sampleRate_(44100.0f)
    , bufferSize_(2048)
    , threshold_(DEFAULT_THRESHOLD)
    , confidence_(0.0f)
{
}

PitchDetector::~PitchDetector() {
}

void PitchDetector::initialize(float sampleRate, int bufferSize) {
    sampleRate_ = sampleRate;
    bufferSize_ = bufferSize;

    // Allocate buffers for YIN algorithm
    yinBuffer_.resize(bufferSize / 2);
    processBuffer_.resize(bufferSize);
}

float PitchDetector::detectPitch(const float* buffer, int bufferSize) {
    if (bufferSize != bufferSize_) {
        return 0.0f;
    }

    // Copy to process buffer
    std::copy(buffer, buffer + bufferSize, processBuffer_.begin());

    // Step 1: Difference function
    differenceFunction(processBuffer_.data());

    // Step 2: Cumulative mean normalized difference
    cumulativeMeanNormalizedDifference();

    // Step 3: Absolute threshold
    int tauEstimate = absoluteThreshold();

    if (tauEstimate == -1) {
        confidence_ = 0.0f;
        return 0.0f;
    }

    // Step 4: Parabolic interpolation
    float betterTau = parabolicInterpolation(tauEstimate);

    // Calculate frequency
    float frequency = sampleRate_ / betterTau;

    // Confidence is inverse of YIN value
    confidence_ = 1.0f - yinBuffer_[tauEstimate];

    // Range check
    if (frequency < MIN_FREQUENCY || frequency > MAX_FREQUENCY) {
        confidence_ = 0.0f;
        return 0.0f;
    }

    return frequency;
}

void PitchDetector::differenceFunction(const float* buffer) {
    int halfSize = bufferSize_ / 2;

    // Using Accelerate framework for optimized performance
    for (int tau = 0; tau < halfSize; tau++) {
        float sum = 0.0f;

        // Vectorized difference calculation
        vDSP_Length length = bufferSize_ - tau;
        float diff;

        for (int i = 0; i < length; i++) {
            diff = buffer[i] - buffer[i + tau];
            sum += diff * diff;
        }

        yinBuffer_[tau] = sum;
    }
}

void PitchDetector::cumulativeMeanNormalizedDifference() {
    yinBuffer_[0] = 1.0f;

    float runningSum = 0.0f;
    for (int tau = 1; tau < yinBuffer_.size(); tau++) {
        runningSum += yinBuffer_[tau];

        if (runningSum == 0.0f) {
            yinBuffer_[tau] = 1.0f;
        } else {
            yinBuffer_[tau] *= tau / runningSum;
        }
    }
}

int PitchDetector::absoluteThreshold() {
    int maxPeriod = static_cast<int>(sampleRate_ / MIN_FREQUENCY);
    int minPeriod = static_cast<int>(sampleRate_ / MAX_FREQUENCY);

    // Start from minimum period
    for (int tau = minPeriod; tau < maxPeriod && tau < yinBuffer_.size(); tau++) {
        if (yinBuffer_[tau] < threshold_) {
            // Find local minimum
            while (tau + 1 < yinBuffer_.size() && yinBuffer_[tau + 1] < yinBuffer_[tau]) {
                tau++;
            }
            return tau;
        }
    }

    return -1; // No pitch found
}

float PitchDetector::parabolicInterpolation(int tauEstimate) {
    if (tauEstimate < 1 || tauEstimate >= yinBuffer_.size() - 1) {
        return static_cast<float>(tauEstimate);
    }

    float s0 = yinBuffer_[tauEstimate - 1];
    float s1 = yinBuffer_[tauEstimate];
    float s2 = yinBuffer_[tauEstimate + 1];

    // Parabolic interpolation formula
    float adjustment = (s2 - s0) / (2.0f * (2.0f * s1 - s2 - s0));

    return static_cast<float>(tauEstimate) + adjustment;
}
