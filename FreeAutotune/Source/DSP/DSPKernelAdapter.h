#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

NS_ASSUME_NONNULL_BEGIN

/// Objective-C wrapper for DSPKernel to bridge Swift and C++
@interface DSPKernelAdapter : NSObject

/// Initialize with sample rate and channel configuration
- (instancetype)initWithSampleRate:(double)sampleRate
                      channelCount:(int)channelCount
                         maxFrames:(int)maxFrames;

/// Reset the DSP state
- (void)reset;

/// Set parameter values
- (void)setRetuneSpeed:(float)value;
- (void)setKey:(int)value;
- (void)setScale:(int)value;
- (void)setMix:(float)value;
- (void)setFormantPreserve:(float)value;

/// Get current pitch information
- (float)detectedPitch;
- (float)targetPitch;

/// Process audio buffer
- (void)processWithInput:(const AudioBufferList *)input
                  output:(AudioBufferList *)output
              frameCount:(AUAudioFrameCount)frameCount;

@end

NS_ASSUME_NONNULL_END
