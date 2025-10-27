#include "AudioProcessor.hpp"
#include <Accelerate/Accelerate.h>

AudioProcessor::AudioProcessor()
    : sampleRate_(44100.0f)
    , maxBufferSize_(8192)
    , detectedPitch_(0.0f)
    , targetPitch_(0.0f)
    , smoothedPitch_(0.0f)
{
}

AudioProcessor::~AudioProcessor() {
}

void AudioProcessor::initialize(float sampleRate, int maxBufferSize) {
    sampleRate_ = sampleRate;
    maxBufferSize_ = maxBufferSize;

    pitchDetector_.initialize(sampleRate, maxBufferSize);
    pitchShifter_.initialize(sampleRate, maxBufferSize);
}

void AudioProcessor::reset() {
    pitchShifter_.reset();
    detectedPitch_ = 0.0f;
    targetPitch_ = 0.0f;
    smoothedPitch_ = 0.0f;
}

void AudioProcessor::process(const float* input, float* output, int bufferSize,
                             float retuneSpeed, int targetKey, int scale,
                             float mix, float formantPreserve) {
    // Detect pitch
    detectedPitch_ = pitchDetector_.detectPitch(input, bufferSize);

    if (detectedPitch_ < 80.0f || detectedPitch_ > 1000.0f) {
        // No valid pitch detected, pass through
        std::copy(input, input + bufferSize, output);
        return;
    }

    // Get confidence
    float confidence = pitchDetector_.getConfidence();

    if (confidence < 0.5f) {
        // Low confidence, pass through
        std::copy(input, input + bufferSize, output);
        return;
    }

    // Find target pitch based on scale
    targetPitch_ = findNearestScaleNote(detectedPitch_, targetKey, scale);

    // Smooth pitch for natural sound
    if (smoothedPitch_ == 0.0f) {
        smoothedPitch_ = detectedPitch_;
    }

    // Apply retune speed (0-100%)
    float retuneAmount = retuneSpeed / 100.0f;
    float targetSmoothed = detectedPitch_ + (targetPitch_ - detectedPitch_) * retuneAmount;

    // Additional smoothing
    smoothedPitch_ = smoothedPitch_ * PITCH_SMOOTH_FACTOR +
                     targetSmoothed * (1.0f - PITCH_SMOOTH_FACTOR);

    // Calculate pitch shift ratio
    float pitchRatio = smoothedPitch_ / detectedPitch_;

    // Limit pitch ratio to reasonable range
    if (pitchRatio < 0.5f) pitchRatio = 0.5f;
    if (pitchRatio > 2.0f) pitchRatio = 2.0f;

    // Apply pitch shifting
    std::vector<float> processedBuffer(bufferSize);
    pitchShifter_.process(input, processedBuffer.data(), bufferSize, pitchRatio);

    // Apply mix (dry/wet)
    float mixAmount = mix / 100.0f;
    float dryAmount = 1.0f - mixAmount;

    for (int i = 0; i < bufferSize; i++) {
        output[i] = input[i] * dryAmount + processedBuffer[i] * mixAmount;
    }
}

float AudioProcessor::findNearestScaleNote(float frequency, int key, int scale) {
    // Convert frequency to MIDI note number
    int noteNumber = frequencyToNote(frequency);

    // Chromatic scale - snap to nearest semitone
    if (scale == 2) {
        return noteToFrequency(noteNumber);
    }

    // Find nearest note in scale
    int octave = noteNumber / 12;
    int noteInOctave = noteNumber % 12;

    // Adjust for key
    int relativeNote = (noteInOctave - key + 12) % 12;

    // Find nearest scale degree
    const int* scalePattern = (scale == 0) ? MAJOR_SCALE : MINOR_SCALE;
    int nearestScaleDegree = 0;
    int minDistance = 12;

    for (int i = 0; i < 7; i++) {
        int distance = std::abs(relativeNote - scalePattern[i]);

        // Also check octave wrapping
        int wrapDistance = std::abs(relativeNote - (scalePattern[i] + 12));
        int wrapDistanceDown = std::abs(relativeNote - (scalePattern[i] - 12));

        distance = std::min({distance, wrapDistance, wrapDistanceDown});

        if (distance < minDistance) {
            minDistance = distance;
            nearestScaleDegree = scalePattern[i];
        }
    }

    // Calculate target note
    int targetNote = (nearestScaleDegree + key) % 12;
    int targetNoteNumber = octave * 12 + targetNote;

    // Check if we need to adjust octave
    if (std::abs(targetNoteNumber - noteNumber) > 6) {
        if (targetNoteNumber > noteNumber) {
            targetNoteNumber -= 12;
        } else {
            targetNoteNumber += 12;
        }
    }

    return noteToFrequency(targetNoteNumber);
}

float AudioProcessor::noteToFrequency(int noteNumber) {
    // MIDI note to frequency: f = 440 * 2^((n-69)/12)
    return A4_FREQUENCY * std::pow(2.0f, (noteNumber - A4_NOTE) / 12.0f);
}

int AudioProcessor::frequencyToNote(float frequency) {
    // Frequency to MIDI note: n = 69 + 12*log2(f/440)
    if (frequency <= 0.0f) return 0;

    float noteNumber = A4_NOTE + 12.0f * std::log2(frequency / A4_FREQUENCY);
    return static_cast<int>(std::round(noteNumber));
}

bool AudioProcessor::isNoteInScale(int note, int key, int scale) {
    if (scale == 2) return true; // Chromatic - all notes

    int relativeNote = (note - key + 12) % 12;
    const int* scalePattern = (scale == 0) ? MAJOR_SCALE : MINOR_SCALE;

    for (int i = 0; i < 7; i++) {
        if (scalePattern[i] == relativeNote) {
            return true;
        }
    }

    return false;
}
