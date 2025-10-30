import SwiftUI
import AudioToolbox
import CoreAudioKit

/// SwiftUI-based Audio Unit View Controller
/// Provides modern, intuitive interface for RealTune
public class AudioUnitViewController: AUViewController, AUAudioUnitFactory {

    private var audioUnit: RealTuneAU?
    private var observation: NSKeyValueObservation?

    public override func viewDidLoad() {
        super.viewDidLoad()

        // Set preferred content size
        preferredContentSize = NSSize(width: 480, height: 360)
    }

    // MARK: - AUAudioUnitFactory

    public func createAudioUnit(with componentDescription: AudioComponentDescription) throws -> AUAudioUnit {
        let audioUnit = try RealTuneAU(componentDescription: componentDescription)
        self.audioUnit = audioUnit

        // Create SwiftUI view and embed it
        let contentView = RealTuneView(audioUnit: audioUnit)
        let hostingController = NSHostingController(rootView: contentView)

        addChild(hostingController)
        view.addSubview(hostingController.view)

        hostingController.view.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            hostingController.view.topAnchor.constraint(equalTo: view.topAnchor),
            hostingController.view.bottomAnchor.constraint(equalTo: view.bottomAnchor),
            hostingController.view.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            hostingController.view.trailingAnchor.constraint(equalTo: view.trailingAnchor)
        ])

        return audioUnit
    }
}

// MARK: - SwiftUI Views

struct RealTuneView: View {
    @ObservedObject var viewModel: RealTuneViewModel

    init(audioUnit: RealTuneAU) {
        self.viewModel = RealTuneViewModel(audioUnit: audioUnit)
    }

    var body: some View {
        ZStack {
            // Background gradient
            LinearGradient(
                gradient: Gradient(colors: [
                    Color(red: 0.1, green: 0.1, blue: 0.15),
                    Color(red: 0.15, green: 0.15, blue: 0.2)
                ]),
                startPoint: .topLeading,
                endPoint: .bottomTrailing
            )
            .ignoresSafeArea()

            VStack(spacing: 24) {
                // Header
                HeaderView()

                // Pitch visualization
                PitchVisualizationView(
                    detectedPitch: viewModel.detectedPitch,
                    targetPitch: viewModel.targetPitch
                )
                .frame(height: 80)
                .padding(.horizontal)

                // Main controls
                VStack(spacing: 16) {
                    // Retune Speed
                    ParameterSliderView(
                        title: "Retune Speed",
                        value: $viewModel.retuneSpeed,
                        range: 0...100,
                        unit: "%"
                    )

                    // Mix
                    ParameterSliderView(
                        title: "Mix",
                        value: $viewModel.mix,
                        range: 0...100,
                        unit: "%"
                    )
                }
                .padding(.horizontal)

                // Key and Scale selection
                HStack(spacing: 20) {
                    // Key selector
                    VStack(alignment: .leading, spacing: 8) {
                        Text("Key")
                            .font(.system(size: 12, weight: .medium))
                            .foregroundColor(.white.opacity(0.7))

                        Picker("", selection: $viewModel.key) {
                            ForEach(0..<12) { index in
                                Text(KeyNote.allCases[index].rawValue)
                                    .tag(index)
                            }
                        }
                        .pickerStyle(MenuPickerStyle())
                        .frame(width: 100)
                    }

                    // Scale selector
                    VStack(alignment: .leading, spacing: 8) {
                        Text("Scale")
                            .font(.system(size: 12, weight: .medium))
                            .foregroundColor(.white.opacity(0.7))

                        Picker("", selection: $viewModel.scale) {
                            Text("Major").tag(0)
                            Text("Minor").tag(1)
                            Text("Chromatic").tag(2)
                        }
                        .pickerStyle(SegmentedPickerStyle())
                        .frame(width: 240)
                    }
                }
                .padding(.horizontal)

                Spacer()
            }
            .padding(.vertical, 20)
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
}

struct HeaderView: View {
    var body: some View {
        VStack(spacing: 4) {
            Text("RealTune")
                .font(.system(size: 24, weight: .bold))
                .foregroundColor(.white)

            Text("High-Performance Pitch Correction")
                .font(.system(size: 12, weight: .regular))
                .foregroundColor(.white.opacity(0.6))
        }
    }
}

struct PitchVisualizationView: View {
    let detectedPitch: Double
    let targetPitch: Double

    var body: some View {
        HStack(spacing: 20) {
            // Detected pitch
            VStack(spacing: 4) {
                Text("Detected")
                    .font(.system(size: 11, weight: .medium))
                    .foregroundColor(.white.opacity(0.6))

                Text(detectedPitch > 0 ? String(format: "%.1f Hz", detectedPitch) : "--")
                    .font(.system(size: 18, weight: .semibold))
                    .foregroundColor(Color.blue)
            }
            .frame(maxWidth: .infinity)
            .padding()
            .background(Color.white.opacity(0.05))
            .cornerRadius(8)

            // Arrow
            Image(systemName: "arrow.right")
                .foregroundColor(.white.opacity(0.3))

            // Target pitch
            VStack(spacing: 4) {
                Text("Target")
                    .font(.system(size: 11, weight: .medium))
                    .foregroundColor(.white.opacity(0.6))

                Text(targetPitch > 0 ? String(format: "%.1f Hz", targetPitch) : "--")
                    .font(.system(size: 18, weight: .semibold))
                    .foregroundColor(Color.green)
            }
            .frame(maxWidth: .infinity)
            .padding()
            .background(Color.white.opacity(0.05))
            .cornerRadius(8)
        }
    }
}

struct ParameterSliderView: View {
    let title: String
    @Binding var value: Double
    let range: ClosedRange<Double>
    let unit: String

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack {
                Text(title)
                    .font(.system(size: 12, weight: .medium))
                    .foregroundColor(.white.opacity(0.7))

                Spacer()

                Text(String(format: "%.0f%@", value, unit))
                    .font(.system(size: 12, weight: .semibold))
                    .foregroundColor(.white)
                    .frame(width: 60, alignment: .trailing)
            }

            Slider(value: $value, in: range)
                .accentColor(Color.blue)
        }
        .padding()
        .background(Color.white.opacity(0.05))
        .cornerRadius(8)
    }
}

// MARK: - View Model

class RealTuneViewModel: ObservableObject {
    private let audioUnit: RealTuneAU

    @Published var retuneSpeed: Double = 50.0 {
        didSet { updateParameter(.retuneSpeed, value: Float(retuneSpeed)) }
    }

    @Published var key: Int = 0 {
        didSet { updateParameter(.key, value: Float(key)) }
    }

    @Published var scale: Int = 2 {
        didSet { updateParameter(.scale, value: Float(scale)) }
    }

    @Published var mix: Double = 100.0 {
        didSet { updateParameter(.mix, value: Float(mix)) }
    }

    @Published var detectedPitch: Double = 0.0
    @Published var targetPitch: Double = 0.0

    private var updateTimer: Timer?

    init(audioUnit: RealTuneAU) {
        self.audioUnit = audioUnit

        // Start timer to update pitch display
        updateTimer = Timer.scheduledTimer(withTimeInterval: 0.05, repeats: true) { [weak self] _ in
            self?.updatePitchDisplay()
        }
    }

    deinit {
        updateTimer?.invalidate()
    }

    private func updateParameter(_ address: ParameterAddress, value: Float) {
        guard let parameter = audioUnit.parameterTree?.parameter(withAddress: address.rawValue) else {
            return
        }

        parameter.value = value
    }

    private func updatePitchDisplay() {
        // This would read pitch values from the audio unit
        // For now, placeholder values
    }
}

enum KeyNote: String, CaseIterable {
    case c = "C"
    case cSharp = "C#"
    case d = "D"
    case dSharp = "D#"
    case e = "E"
    case f = "F"
    case fSharp = "F#"
    case g = "G"
    case gSharp = "G#"
    case a = "A"
    case aSharp = "A#"
    case b = "B"
}
