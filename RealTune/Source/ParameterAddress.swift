import AudioToolbox

enum ParameterAddress: AUParameterAddress {
    case retuneSpeed = 0
    case key = 1
    case scale = 2
    case mix = 3
    case formantPreserve = 4
}

extension AUParameterTree {
    static func createRealTuneParameters() -> AUParameterTree {
        // Retune Speed (0-100%)
        let retuneSpeedParam = AUParameterTree.createParameter(
            withIdentifier: "retuneSpeed",
            name: "Retune Speed",
            address: ParameterAddress.retuneSpeed.rawValue,
            min: 0.0,
            max: 100.0,
            unit: .percent,
            unitName: nil,
            flags: [.flag_IsReadable, .flag_IsWritable],
            valueStrings: nil,
            dependentParameters: nil
        )
        retuneSpeedParam.value = 50.0

        // Key (0-11: C, C#, D, D#, E, F, F#, G, G#, A, A#, B)
        let keyParam = AUParameterTree.createParameter(
            withIdentifier: "key",
            name: "Key",
            address: ParameterAddress.key.rawValue,
            min: 0,
            max: 11,
            unit: .indexed,
            unitName: nil,
            flags: [.flag_IsReadable, .flag_IsWritable],
            valueStrings: ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"],
            dependentParameters: nil
        )
        keyParam.value = 0 // C

        // Scale (0: Major, 1: Minor, 2: Chromatic)
        let scaleParam = AUParameterTree.createParameter(
            withIdentifier: "scale",
            name: "Scale",
            address: ParameterAddress.scale.rawValue,
            min: 0,
            max: 2,
            unit: .indexed,
            unitName: nil,
            flags: [.flag_IsReadable, .flag_IsWritable],
            valueStrings: ["Major", "Minor", "Chromatic"],
            dependentParameters: nil
        )
        scaleParam.value = 2 // Chromatic default

        // Mix (0-100%)
        let mixParam = AUParameterTree.createParameter(
            withIdentifier: "mix",
            name: "Mix",
            address: ParameterAddress.mix.rawValue,
            min: 0.0,
            max: 100.0,
            unit: .percent,
            unitName: nil,
            flags: [.flag_IsReadable, .flag_IsWritable],
            valueStrings: nil,
            dependentParameters: nil
        )
        mixParam.value = 100.0

        // Formant Preserve (0-100%)
        let formantParam = AUParameterTree.createParameter(
            withIdentifier: "formantPreserve",
            name: "Formant Preserve",
            address: ParameterAddress.formantPreserve.rawValue,
            min: 0.0,
            max: 100.0,
            unit: .percent,
            unitName: nil,
            flags: [.flag_IsReadable, .flag_IsWritable],
            valueStrings: nil,
            dependentParameters: nil
        )
        formantParam.value = 80.0

        return AUParameterTree.createTree(withChildren: [
            retuneSpeedParam,
            keyParam,
            scaleParam,
            mixParam,
            formantParam
        ])
    }
}
