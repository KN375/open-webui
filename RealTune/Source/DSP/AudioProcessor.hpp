#ifndef AudioProcessor_hpp
#define AudioProcessor_hpp

#include "PitchDetector.hpp"
#include "PitchShifter.hpp"
#include <vector>
#include <cmath>

/// Main audio processor combining pitch detection and correction
/// Optimized for real-time performance with minimal latency
class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();

    /// Initialize processor
    void initialize(float sampleRate, int maxBufferSize);

    /// Process audio buffer with autotune
    /// @param input Input audio samples
    /// @param output Output audio samples
    /// @param bufferSize Number of samples to process
    /// @param retuneSpeed Pitch correction speed (0-100%)
    /// @param targetKey Target musical key (0-11, where 0=C)
    /// @param scale Scale type (0=Major, 1=Minor, 2=Chromatic)
    /// @param mix Dry/wet mix (0-100%)
    /// @param formantPreserve Formant preservation amount (0-100%)
    void process(const float* input, float* output, int bufferSize,
                float retuneSpeed, int targetKey, int scale, float mix, float formantPreserve);

    /// Reset processor state
    void reset();

    /// Get last detected pitch
    float getDetectedPitch() const { return detectedPitch_; }

    /// Get target pitch
    float getTargetPitch() const { return targetPitch_; }

private:
    float sampleRate_;
    int maxBufferSize_;

    PitchDetector pitchDetector_;
    PitchShifter pitchShifter_;

    float detectedPitch_;
    float targetPitch_;
    float smoothedPitch_;

    // Pitch smoothing for natural sound
    static constexpr float PITCH_SMOOTH_FACTOR = 0.8f;

    // Helper functions
    float findNearestScaleNote(float frequency, int key, int scale);
    float noteToFrequency(int noteNumber);
    int frequencyToNote(float frequency);
    bool isNoteInScale(int note, int key, int scale);

    // Musical scales
    static constexpr int MAJOR_SCALE[7] = {0, 2, 4, 5, 7, 9, 11};
    static constexpr int MINOR_SCALE[7] = {0, 2, 3, 5, 7, 8, 10};
    static constexpr float A4_FREQUENCY = 440.0f;
    static constexpr int A4_NOTE = 69; // MIDI note number for A4
};

#endif /* AudioProcessor_hpp */
