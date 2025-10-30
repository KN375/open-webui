#ifndef PitchDetector_hpp
#define PitchDetector_hpp

#include <vector>
#include <cmath>
#include <algorithm>

/// High-performance YIN pitch detection algorithm
/// Optimized for real-time audio processing
class PitchDetector {
public:
    PitchDetector();
    ~PitchDetector();

    /// Initialize the detector with sample rate
    void initialize(float sampleRate, int bufferSize);

    /// Detect pitch from audio buffer
    /// @param buffer Input audio samples
    /// @param bufferSize Number of samples
    /// @return Detected frequency in Hz (0.0 if no pitch detected)
    float detectPitch(const float* buffer, int bufferSize);

    /// Get confidence of last detection (0.0-1.0)
    float getConfidence() const { return confidence_; }

    /// Set threshold for pitch detection (lower = more sensitive)
    void setThreshold(float threshold) { threshold_ = threshold; }

private:
    float sampleRate_;
    int bufferSize_;
    float threshold_;
    float confidence_;

    std::vector<float> yinBuffer_;
    std::vector<float> processBuffer_;

    // YIN algorithm steps
    void differenceFunction(const float* buffer);
    void cumulativeMeanNormalizedDifference();
    int absoluteThreshold();
    float parabolicInterpolation(int tauEstimate);

    // Constants
    static constexpr float MIN_FREQUENCY = 80.0f;   // E2
    static constexpr float MAX_FREQUENCY = 1000.0f; // C6
    static constexpr float DEFAULT_THRESHOLD = 0.15f;
};

#endif /* PitchDetector_hpp */
