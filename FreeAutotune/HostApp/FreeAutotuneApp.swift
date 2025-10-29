import SwiftUI

@main
struct FreeAutotuneApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}

struct ContentView: View {
    var body: some View {
        VStack(spacing: 30) {
            Image(systemName: "waveform")
                .font(.system(size: 80))
                .foregroundColor(.blue)

            Text("FreeAutotune")
                .font(.system(size: 36, weight: .bold))

            Text("High-Performance Autotune Plugin")
                .font(.system(size: 16))
                .foregroundColor(.secondary)

            Divider()
                .padding(.horizontal, 40)

            VStack(alignment: .leading, spacing: 12) {
                HStack {
                    Image(systemName: "checkmark.circle.fill")
                        .foregroundColor(.green)
                    Text("Audio Unit V3 Plugin Installed")
                }

                HStack {
                    Image(systemName: "checkmark.circle.fill")
                        .foregroundColor(.green)
                    Text("Compatible with Logic Pro")
                }

                HStack {
                    Image(systemName: "checkmark.circle.fill")
                        .foregroundColor(.green)
                    Text("Low Latency Real-time Processing")
                }
            }
            .padding()
            .background(Color.gray.opacity(0.1))
            .cornerRadius(12)

            Text("Usage")
                .font(.headline)
                .padding(.top)

            VStack(alignment: .leading, spacing: 8) {
                Text("1. Open Logic Pro")
                Text("2. Add Audio FX → Audio Units → FreeAutotune")
                Text("3. Adjust parameters for your desired effect")
            }
            .font(.system(size: 14))
            .frame(maxWidth: .infinity, alignment: .leading)
            .padding()
            .background(Color.blue.opacity(0.1))
            .cornerRadius(12)

            Spacer()

            Text("Version 1.0.0 • MIT License")
                .font(.caption)
                .foregroundColor(.secondary)
        }
        .padding(40)
        .frame(width: 600, height: 700)
    }
}

#Preview {
    ContentView()
}
