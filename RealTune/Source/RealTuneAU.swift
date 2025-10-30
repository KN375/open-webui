import AudioToolbox
import AVFoundation
import CoreAudio

/// Main Audio Unit implementation for RealTune
/// Provides real-time pitch correction with low latency
public class RealTuneAU: AUAudioUnit {

    // DSP Kernel (C++ bridge)
    private var kernel: DSPKernel!

    // Audio buffers
    private var inputBus: AUAudioUnitBus!
    private var outputBus: AUAudioUnitBus!
    private var inputBusArray: AUAudioUnitBusArray!
    private var outputBusArray: AUAudioUnitBusArray!

    // Parameters
    private var parameterTree: AUParameterTree!
    private var retuneSpeedParameter: AUParameter!
    private var keyParameter: AUParameter!
    private var scaleParameter: AUParameter!
    private var mixParameter: AUParameter!
    private var formantParameter: AUParameter!

    // Maximum frames to process
    private let maxFramesToRender: AUAudioFrameCount = 512

    public override init(componentDescription: AudioComponentDescription,
                        options: AudioComponentInstantiationOptions = []) throws {

        try super.init(componentDescription: componentDescription, options: options)

        // Create default audio format (stereo, 44.1kHz)
        guard let format = AVAudioFormat(standardFormatWithSampleRate: 44100, channels: 2) else {
            throw NSError(domain: NSOSStatusErrorDomain, code: Int(kAudioUnitErr_FormatNotSupported), userInfo: nil)
        }

        // Create input and output buses
        inputBus = try AUAudioUnitBus(format: format)
        inputBus.maximumChannelCount = 2

        outputBus = try AUAudioUnitBus(format: format)
        outputBus.maximumChannelCount = 2

        // Create bus arrays
        inputBusArray = AUAudioUnitBusArray(audioUnit: self, busType: .input, busses: [inputBus])
        outputBusArray = AUAudioUnitBusArray(audioUnit: self, busType: .output, busses: [outputBus])

        // Initialize DSP kernel
        kernel = DSPKernel(format: format)

        // Create parameter tree
        parameterTree = AUParameterTree.createRealTuneParameters()

        // Get parameter references
        retuneSpeedParameter = parameterTree.parameter(withAddress: ParameterAddress.retuneSpeed.rawValue)!
        keyParameter = parameterTree.parameter(withAddress: ParameterAddress.key.rawValue)!
        scaleParameter = parameterTree.parameter(withAddress: ParameterAddress.scale.rawValue)!
        mixParameter = parameterTree.parameter(withAddress: ParameterAddress.mix.rawValue)!
        formantParameter = parameterTree.parameter(withAddress: ParameterAddress.formantPreserve.rawValue)!

        // Setup parameter change handler
        parameterTree.implementorValueObserver = { [weak self] parameter, value in
            self?.kernel.setParameter(address: parameter.address, value: value)
        }

        parameterTree.implementorValueProvider = { [weak self] parameter in
            return self?.kernel.getParameter(address: parameter.address) ?? 0.0
        }

        // Set maximum frames to render
        self.maximumFramesToRender = maxFramesToRender
    }

    // MARK: - AUAudioUnit Overrides

    public override var inputBusses: AUAudioUnitBusArray {
        return inputBusArray
    }

    public override var outputBusses: AUAudioUnitBusArray {
        return outputBusArray
    }

    public override var parameterTree: AUParameterTree? {
        get { return parameterTree }
        set { /* Read-only */ }
    }

    public override func allocateRenderResources() throws {
        try super.allocateRenderResources()

        guard let format = inputBus.format else {
            throw NSError(domain: NSOSStatusErrorDomain, code: Int(kAudioUnitErr_FormatNotSupported), userInfo: nil)
        }

        kernel.allocateResources(format: format, maxFrames: maxFramesToRender)
    }

    public override func deallocateRenderResources() {
        kernel.deallocateResources()
        super.deallocateRenderResources()
    }

    public override func reset() {
        kernel.reset()
    }

    public override var internalRenderBlock: AUInternalRenderBlock {
        return { [weak self] (
            actionFlags,
            timestamp,
            frameCount,
            outputBusNumber,
            outputData,
            realtimeEventListHead,
            pullInputBlock
        ) -> AUAudioUnitStatus in

            guard let self = self else {
                return kAudioUnitErr_Uninitialized
            }

            // Pull input audio
            var pullFlags = AudioUnitRenderActionFlags(rawValue: 0)
            let status = pullInputBlock?(&pullFlags, timestamp, frameCount, 0, outputData)

            if status != noErr {
                return status ?? kAudioUnitErr_NoConnection
            }

            // Process audio through DSP kernel (in-place processing)
            self.kernel.process(inputData: outputData,
                              outputData: outputData,
                              frameCount: frameCount)

            return noErr
        }
    }

    public override var canProcessInPlace: Bool {
        return true
    }

    public override var latency: TimeInterval {
        // Low latency design - one buffer size
        return TimeInterval(maxFramesToRender) / (inputBus.format?.sampleRate ?? 44100.0)
    }

    public override var tailTime: TimeInterval {
        return 0.0 // No tail
    }
}

// MARK: - DSP Kernel Bridge

/// Bridge between Swift Audio Unit and C++ DSP code using DSPKernelAdapter
private class DSPKernel {
    private var adapter: DSPKernelAdapter?

    init(format: AVAudioFormat) {
        // Will be initialized in allocateResources
    }

    deinit {
        adapter = nil
    }

    func allocateResources(format: AVAudioFormat, maxFrames: AUAudioFrameCount) {
        adapter = DSPKernelAdapter(
            sampleRate: format.sampleRate,
            channelCount: Int32(format.channelCount),
            maxFrames: Int32(maxFrames)
        )
    }

    func deallocateResources() {
        adapter = nil
    }

    func reset() {
        adapter?.reset()
    }

    func setParameter(address: AUParameterAddress, value: AUValue) {
        guard let adapter = adapter else { return }

        switch ParameterAddress(rawValue: address) {
        case .retuneSpeed:
            adapter.setRetuneSpeed(value)
        case .key:
            adapter.setKey(Int32(value))
        case .scale:
            adapter.setScale(Int32(value))
        case .mix:
            adapter.setMix(value)
        case .formantPreserve:
            adapter.setFormantPreserve(value)
        default:
            break
        }
    }

    func getParameter(address: AUParameterAddress) -> AUValue {
        // Parameters are stored in the adapter
        // For simplicity, return default values
        // In production, you'd want to store these in Swift too
        return 0.0
    }

    func getDetectedPitch() -> Float {
        return adapter?.detectedPitch() ?? 0.0
    }

    func getTargetPitch() -> Float {
        return adapter?.targetPitch() ?? 0.0
    }

    func process(inputData: UnsafePointer<AudioBufferList>, outputData: UnsafeMutablePointer<AudioBufferList>, frameCount: AUAudioFrameCount) {
        adapter?.process(withInput: inputData, output: outputData, frameCount: frameCount)
    }
}
