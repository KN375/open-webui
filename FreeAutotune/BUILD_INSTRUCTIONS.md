# FreeAutotune ビルド手順（AUV3版）

## Audio Unit V3 (AUV3) について

FreeAutotuneはAudio Unit V3 (AUV3) App Extensionとして実装されています。
AUV3プラグインは以下の構造を持ちます：

```
FreeAutotune.app (ホストアプリ)
└── Contents/
    └── PlugIns/
        └── FreeAutotuneAU.appex (App Extension - 実際のプラグイン)
```

## 前提条件

- macOS 11.0以降
- Xcode 13.0以降
- Apple Developer アカウント（開発用証明書）
- Command Line Tools

## ビルド方法

### オプション1: Xcodeで手動ビルド（推奨）

AUV3プラグインは複雑な構造を持つため、Xcodeでのビルドを推奨します。

#### 1. Xcodeプロジェクトの作成

```bash
cd FreeAutotune
open .
```

Xcodeを開いて、以下の手順でプロジェクトを作成します：

#### 2. 新規プロジェクト作成

1. **File → New → Project**
2. **macOS → App** を選択
3. Product Name: `FreeAutotune`
4. Bundle Identifier: `com.freeaudio.FreeAutotune`
5. Interface: SwiftUI
6. Language: Swift

#### 3. Audio Unit Extension Targetを追加

1. プロジェクトファイルを選択
2. 下部の **+ ボタン** をクリック
3. **Audio Unit Extension** を選択
4. Product Name: `FreeAutotuneAU`
5. Bundle Identifier: `com.freeaudio.FreeAutotune.AudioUnit`

#### 4. ファイルを追加

**ホストアプリ (FreeAutotune target)**
- `HostApp/FreeAutotuneApp.swift`
- `HostApp/Info.plist`

**Audio Unit Extension (FreeAutotuneAU target)**
- `Source/FreeAutotuneAU.swift`
- `Source/AudioUnitViewController.swift`
- `Source/ParameterAddress.swift`
- `Source/FreeAutotune-Bridging-Header.h`
- `Source/DSP/*.cpp`
- `Source/DSP/*.hpp`
- `Source/DSP/*.h`
- `Source/DSP/*.mm`
- `AudioUnitExtension-Info.plist`

#### 5. ビルド設定

**Audio Unit Extension Target (FreeAutotuneAU)**

- **Build Settings → Swift Compiler - General**
  - Objective-C Bridging Header: `Source/FreeAutotune-Bridging-Header.h`

- **Build Settings → Apple Clang - Language - C++**
  - C++ Language Dialect: `C++17 [-std=c++17]`

- **Build Settings → Linking**
  - Other Linker Flags: `-framework Accelerate -framework AudioToolbox -framework AVFoundation`

- **Build Phases → Compile Sources**
  - すべての.cppと.mmファイルを追加
  - .mmファイルのCompiler Flagsに `-x objective-c++` を追加（自動）

- **Signing & Capabilities**
  - Signing Certificate: Development
  - App Sandbox: ON
  - Audio Input: ON（必要に応じて）

**ホストアプリ Target (FreeAutotune)**

- **General → Frameworks, Libraries, and Embedded Content**
  - FreeAutotuneAU.appex を Embed

- **Signing & Capabilities**
  - Signing Certificate: Development
  - App Sandbox: OFF（プラグインのため）

#### 6. ビルドと実行

1. Scheme: **FreeAutotune** を選択
2. **Product → Build** (⌘B)
3. ビルド成功後、.app を `/Applications` にコピー
4. Logic Proを起動してプラグインを確認

### オプション2: シンプルな.componentビルド

AUV3の完全な構造が不要な場合、シンプルな.componentとしてビルドすることも可能です。

#### プロジェクトファイルの作成

```bash
# Xcodeプロジェクトテンプレートを作成
mkdir FreeAutotune.xcodeproj
cat > FreeAutotune.xcodeproj/project.pbxproj << 'EOF'
// Xcode project file
// ※実際のファイルは手動で作成する必要があります
EOF
```

## 手動インストール

ビルド後、プラグインをインストール：

```bash
# App全体をApplicationsにコピー
cp -R build/Release/FreeAutotune.app /Applications/

# Logic Proを再起動
killall "Logic Pro"
```

または、Audio Unit extensionのみをインストール：

```bash
# Audio Componentディレクトリにコピー
cp -R build/Release/FreeAutotune.app/Contents/PlugIns/FreeAutotuneAU.appex \
     ~/Library/Audio/Plug-Ins/Components/FreeAutotune.component

# Audio Component Registrarをリセット
killall -9 AudioComponentRegistrar
```

## トラブルシューティング

### ビルドエラー

**"Bridging header not found"**
- Build Settings → Swift Compiler → Objective-C Bridging Header のパスを確認
- 相対パスは `$(SRCROOT)/Source/FreeAutotune-Bridging-Header.h`

**"Undefined symbols for C++ classes"**
- すべての.cppファイルがCompile Sourcesに追加されているか確認
- C++ Standard Library を `-lstdc++` でリンク

**"Code signing failed"**
- Signing & Capabilities → Signing Certificate を確認
- Development証明書が有効か確認

### Logic Proで認識されない

```bash
# Audio Unitキャッシュをクリア
killall -9 AudioComponentRegistrar
rm -rf ~/Library/Caches/AudioUnitCache

# Logic Proの設定をリセット
rm ~/Library/Preferences/com.apple.logic10.plist

# Logic Proを再起動
```

### auvalでテスト

```bash
# プラグインを検証
auval -v aufx Fatu Free

# 詳細情報
auval -a
```

## 配布

### Development版

1. .appをzipで圧縮
2. ユーザーは /Applications に展開

### Release版（コード署名＋公証）

```bash
# コード署名
codesign --deep --force --verify --verbose \
  --sign "Developer ID Application: Your Name" \
  FreeAutotune.app

# 公証プロセス
xcrun notarytool submit FreeAutotune.zip \
  --apple-id "your@email.com" \
  --password "app-specific-password" \
  --team-id "TEAMID"

# 配布
# .dmgまたは.pkgとして配布
```

## 参考リンク

- [Audio Unit Programming Guide](https://developer.apple.com/documentation/audiounit)
- [Creating an Audio Unit Extension](https://developer.apple.com/documentation/audiotoolbox/audio_unit_v3_plug-ins/creating_an_audio_unit_extension)
- [App Extension Programming Guide](https://developer.apple.com/library/archive/documentation/General/Conceptual/ExtensibilityPG/)

---

**注意**: 実際の製品レベルのビルドには、適切なプロビジョニングプロファイルとコード署名が必要です。
