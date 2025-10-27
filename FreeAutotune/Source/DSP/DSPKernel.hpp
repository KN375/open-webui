#ifndef DSPKernel_hpp
#define DSPKernel_hpp

#import <AudioToolbox/AudioToolbox.h>
#include "AudioProcessor.hpp"

/// C-compatible DSP kernel interface for Swift bridging
class DSPKernel {
public:
    DSPKernel();
    ~DSPKernel();

    // Resource management
    void initialize(double sampleRate, int channelCount, int maxFrames);
    void reset();

    // Parameter control
    void setRetuneSpeed(float value);
    void setKey(int value);
    void setScale(int value);
    void setMix(float value);
    void setFormantPreserve(float value);

    float getRetuneSpeed() const;
    int getKey() const;
    int getScale() const;
    float getMix() const;
    float getFormantPreserve() const;

    // Pitch info
    float getDetectedPitch() const;
    float getTargetPitch() const;

    // Audio processing
    void processAudio(const float* input, float* output, int frameCount, int channelCount);

private:
    AudioProcessor processor_;

    // Current parameter values
    float retuneSpeed_;
    int key_;
    int scale_;
    float mix_;
    float formantPreserve_;

    double sampleRate_;
    int channelCount_;
    int maxFrames_;

    bool initialized_;
};

// C interface for Objective-C++ bridging
#ifdef __cplusplus
extern "C" {
#endif

void* DSPKernel_Create();
void DSPKernel_Destroy(void* kernel);
void DSPKernel_Initialize(void* kernel, double sampleRate, int channelCount, int maxFrames);
void DSPKernel_Reset(void* kernel);
void DSPKernel_SetRetuneSpeed(void* kernel, float value);
void DSPKernel_SetKey(void* kernel, int value);
void DSPKernel_SetScale(void* kernel, int value);
void DSPKernel_SetMix(void* kernel, float value);
void DSPKernel_SetFormantPreserve(void* kernel, float value);
float DSPKernel_GetDetectedPitch(void* kernel);
float DSPKernel_GetTargetPitch(void* kernel);
void DSPKernel_ProcessAudio(void* kernel, const float* input, float* output, int frameCount, int channelCount);

#ifdef __cplusplus
}
#endif

#endif /* DSPKernel_hpp */
