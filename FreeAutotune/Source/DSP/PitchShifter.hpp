#ifndef PitchShifter_hpp
#define PitchShifter_hpp

#include <vector>
#include <cmath>
#include <algorithm>

/// Time-Domain Pitch Synchronous Overlap and Add (TD-PSOLA) implementation
/// Preserves formants while shifting pitch for natural sound
class PitchShifter {
public:
    PitchShifter();
    ~PitchShifter();

    /// Initialize with sample rate
    void initialize(float sampleRate, int maxBufferSize);

    /// Shift pitch of audio buffer
    /// @param inputBuffer Input audio samples
    /// @param outputBuffer Output audio samples
    /// @param bufferSize Number of samples
    /// @param pitchRatio Pitch shift ratio (1.0 = no shift, 2.0 = octave up)
    void process(const float* inputBuffer, float* outputBuffer, int bufferSize, float pitchRatio);

    /// Reset internal state
    void reset();

private:
    float sampleRate_;
    int maxBufferSize_;

    // Circular buffers for pitch shifting
    std::vector<float> inputHistory_;
    std::vector<float> outputHistory_;
    int historySize_;
    int writeIndex_;

    // Peak detection for PSOLA
    std::vector<int> peakPositions_;

    // Window functions
    std::vector<float> hanningWindow_;

    void findPeaks(const float* buffer, int bufferSize, float period);
    void applyPSOLA(const float* inputBuffer, float* outputBuffer, int bufferSize, float pitchRatio);
    float getHanningWindow(int index, int windowSize);

    static constexpr int MAX_PEAKS = 512;
    static constexpr int HISTORY_SAMPLES = 8192;
    static constexpr float MIN_PERIOD = 40.0f;  // ~1000 Hz at 44.1kHz
    static constexpr float MAX_PERIOD = 500.0f; // ~88 Hz at 44.1kHz
};

#endif /* PitchShifter_hpp */
