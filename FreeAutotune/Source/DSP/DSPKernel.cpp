#include "DSPKernel.hpp"
#include <algorithm>

DSPKernel::DSPKernel()
    : retuneSpeed_(50.0f)
    , key_(0)
    , scale_(2)
    , mix_(100.0f)
    , formantPreserve_(80.0f)
    , sampleRate_(44100.0)
    , channelCount_(2)
    , maxFrames_(512)
    , initialized_(false)
{
}

DSPKernel::~DSPKernel() {
}

void DSPKernel::initialize(double sampleRate, int channelCount, int maxFrames) {
    sampleRate_ = sampleRate;
    channelCount_ = channelCount;
    maxFrames_ = maxFrames;

    processor_.initialize(static_cast<float>(sampleRate), maxFrames);
    initialized_ = true;
}

void DSPKernel::reset() {
    if (initialized_) {
        processor_.reset();
    }
}

void DSPKernel::setRetuneSpeed(float value) {
    retuneSpeed_ = std::clamp(value, 0.0f, 100.0f);
}

void DSPKernel::setKey(int value) {
    key_ = std::clamp(value, 0, 11);
}

void DSPKernel::setScale(int value) {
    scale_ = std::clamp(value, 0, 2);
}

void DSPKernel::setMix(float value) {
    mix_ = std::clamp(value, 0.0f, 100.0f);
}

void DSPKernel::setFormantPreserve(float value) {
    formantPreserve_ = std::clamp(value, 0.0f, 100.0f);
}

float DSPKernel::getRetuneSpeed() const {
    return retuneSpeed_;
}

int DSPKernel::getKey() const {
    return key_;
}

int DSPKernel::getScale() const {
    return scale_;
}

float DSPKernel::getMix() const {
    return mix_;
}

float DSPKernel::getFormantPreserve() const {
    return formantPreserve_;
}

float DSPKernel::getDetectedPitch() const {
    return initialized_ ? processor_.getDetectedPitch() : 0.0f;
}

float DSPKernel::getTargetPitch() const {
    return initialized_ ? processor_.getTargetPitch() : 0.0f;
}

void DSPKernel::processAudio(const float* input, float* output, int frameCount, int channelCount) {
    if (!initialized_) {
        // Bypass if not initialized
        std::copy(input, input + frameCount * channelCount, output);
        return;
    }

    // For stereo, process left and right channels separately
    if (channelCount == 2) {
        // Deinterleave
        std::vector<float> leftIn(frameCount);
        std::vector<float> rightIn(frameCount);
        std::vector<float> leftOut(frameCount);
        std::vector<float> rightOut(frameCount);

        for (int i = 0; i < frameCount; i++) {
            leftIn[i] = input[i * 2];
            rightIn[i] = input[i * 2 + 1];
        }

        // Process each channel
        processor_.process(leftIn.data(), leftOut.data(), frameCount,
                          retuneSpeed_, key_, scale_, mix_, formantPreserve_);

        processor_.process(rightIn.data(), rightOut.data(), frameCount,
                          retuneSpeed_, key_, scale_, mix_, formantPreserve_);

        // Interleave
        for (int i = 0; i < frameCount; i++) {
            output[i * 2] = leftOut[i];
            output[i * 2 + 1] = rightOut[i];
        }
    } else {
        // Mono
        processor_.process(input, output, frameCount,
                          retuneSpeed_, key_, scale_, mix_, formantPreserve_);
    }
}

// C interface implementation
void* DSPKernel_Create() {
    return new DSPKernel();
}

void DSPKernel_Destroy(void* kernel) {
    delete static_cast<DSPKernel*>(kernel);
}

void DSPKernel_Initialize(void* kernel, double sampleRate, int channelCount, int maxFrames) {
    static_cast<DSPKernel*>(kernel)->initialize(sampleRate, channelCount, maxFrames);
}

void DSPKernel_Reset(void* kernel) {
    static_cast<DSPKernel*>(kernel)->reset();
}

void DSPKernel_SetRetuneSpeed(void* kernel, float value) {
    static_cast<DSPKernel*>(kernel)->setRetuneSpeed(value);
}

void DSPKernel_SetKey(void* kernel, int value) {
    static_cast<DSPKernel*>(kernel)->setKey(value);
}

void DSPKernel_SetScale(void* kernel, int value) {
    static_cast<DSPKernel*>(kernel)->setScale(value);
}

void DSPKernel_SetMix(void* kernel, float value) {
    static_cast<DSPKernel*>(kernel)->setMix(value);
}

void DSPKernel_SetFormantPreserve(void* kernel, float value) {
    static_cast<DSPKernel*>(kernel)->setFormantPreserve(value);
}

float DSPKernel_GetDetectedPitch(void* kernel) {
    return static_cast<DSPKernel*>(kernel)->getDetectedPitch();
}

float DSPKernel_GetTargetPitch(void* kernel) {
    return static_cast<DSPKernel*>(kernel)->getTargetPitch();
}

void DSPKernel_ProcessAudio(void* kernel, const float* input, float* output, int frameCount, int channelCount) {
    static_cast<DSPKernel*>(kernel)->processAudio(input, output, frameCount, channelCount);
}
