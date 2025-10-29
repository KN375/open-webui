#import "DSPKernelAdapter.h"
#include "DSPKernel.hpp"
#include <vector>

@implementation DSPKernelAdapter {
    DSPKernel *_kernel;
    double _sampleRate;
    int _channelCount;
}

- (instancetype)initWithSampleRate:(double)sampleRate
                      channelCount:(int)channelCount
                         maxFrames:(int)maxFrames {
    self = [super init];
    if (self) {
        _sampleRate = sampleRate;
        _channelCount = channelCount;

        // Create C++ DSP kernel
        _kernel = new DSPKernel();
        _kernel->initialize(sampleRate, channelCount, maxFrames);
    }
    return self;
}

- (void)dealloc {
    if (_kernel) {
        delete _kernel;
        _kernel = nullptr;
    }
}

- (void)reset {
    if (_kernel) {
        _kernel->reset();
    }
}

- (void)setRetuneSpeed:(float)value {
    if (_kernel) {
        _kernel->setRetuneSpeed(value);
    }
}

- (void)setKey:(int)value {
    if (_kernel) {
        _kernel->setKey(value);
    }
}

- (void)setScale:(int)value {
    if (_kernel) {
        _kernel->setScale(value);
    }
}

- (void)setMix:(float)value {
    if (_kernel) {
        _kernel->setMix(value);
    }
}

- (void)setFormantPreserve:(float)value {
    if (_kernel) {
        _kernel->setFormantPreserve(value);
    }
}

- (float)detectedPitch {
    return _kernel ? _kernel->getDetectedPitch() : 0.0f;
}

- (float)targetPitch {
    return _kernel ? _kernel->getTargetPitch() : 0.0f;
}

- (void)processWithInput:(const AudioBufferList *)input
                  output:(AudioBufferList *)output
              frameCount:(AUAudioFrameCount)frameCount {
    if (!_kernel || !input || !output) {
        return;
    }

    // Process each buffer
    UInt32 bufferCount = output->mNumberBuffers;

    for (UInt32 i = 0; i < bufferCount; i++) {
        const float *inputPtr = (const float *)input->mBuffers[i].mData;
        float *outputPtr = (float *)output->mBuffers[i].mData;

        if (inputPtr && outputPtr) {
            _kernel->processAudio(inputPtr, outputPtr, frameCount, _channelCount);
        }
    }
}

@end
