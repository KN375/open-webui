# FreeAutotune ビルド手順（完全版）

このドキュメントでは、FreeAutotuneをゼロからビルドしてLogic Proで使用可能にするまでの完全な手順を説明します。

## 📋 目次

1. [前提条件](#前提条件)
2. [プロジェクト構造の理解](#プロジェクト構造の理解)
3. [Xcodeプロジェクトの作成（ステップバイステップ）](#xcodeプロジェクトの作成ステップバイステップ)
4. [ビルドと実行](#ビルドと実行)
5. [トラブルシューティング](#トラブルシューティング)

---

## 前提条件

### 必要なソフトウェア

- **macOS**: 11.0 (Big Sur) 以降
- **Xcode**: 13.0以降
- **Command Line Tools**: インストール済み

```bash
# Command Line Toolsのインストール（未インストールの場合）
xcode-select --install
```

### 必要な知識

- 基本的なXcodeの操作
- ターミナルの基本操作

---

## プロジェクト構造の理解

FreeAutotuneはAudio Unit V3 (AUV3) App Extensionとして実装されています。

```
FreeAutotune.app                    ← ホストアプリ（空っぽでOK）
└── Contents/
    └── PlugIns/
        └── FreeAutotuneAU.appex   ← 実際のプラグイン（Audio Unit Extension）
```

**なぜこの構造？**
- AUV3は単体の.componentファイルではなく、App Extensionとして配布する必要があります
- ホストアプリはプラグインを含むコンテナの役割を果たします
- Logic ProはApp内のExtensionを自動的に検出して使用します

---

## Xcodeプロジェクトの作成（ステップバイステップ）

### ステップ1: 新規プロジェクトの作成

1. **Xcodeを起動**

2. **File → New → Project** を選択

3. **テンプレート選択:**
   - 左側: **macOS** を選択
   - 右側: **App** を選択
   - **Next** をクリック

4. **プロジェクト設定:**
   ```
   Product Name: FreeAutotune
   Team: (あなたのApple Developer Team)
   Organization Identifier: com.freeaudio
   Bundle Identifier: com.freeaudio.FreeAutotune (自動生成)
   Interface: SwiftUI
   Language: Swift
   ☐ Use Core Data (チェックなし)
   ☐ Include Tests (チェックなし)
   ```
   - **Next** をクリック

5. **保存場所:**
   - FreeAutotuneフォルダと同じ階層を選択
   - **Create** をクリック

### ステップ2: Audio Unit Extension Targetの追加

1. **プロジェクトナビゲーター（左側）でプロジェクトファイルをクリック**
   - 青い "FreeAutotune" アイコンをクリック

2. **下部の + ボタンをクリック** (TARGETS の下)

3. **テンプレート選択:**
   - 左側: **macOS** を選択
   - 右側: **Audio Unit Extension** を選択
   - **Next** をクリック

4. **Extension設定:**
   ```
   Product Name: FreeAutotuneAU
   Team: (あなたのApple Developer Team)
   Organization Identifier: com.freeaudio.FreeAutotune
   Bundle Identifier: com.freeaudio.FreeAutotune.AudioUnit (自動生成)
   Language: Swift

   Audio Unit Properties:
   Manufacturer Code: Free (4文字)
   Subtype Code: Fatu (4文字)
   Type: Effect (aufx)
   ```
   - **Finish** をクリック

5. **アクティベーション確認ダイアログ:**
   - "Activate "FreeAutotuneAU" scheme?" と聞かれたら
   - **Activate** をクリック

### ステップ3: 自動生成ファイルの削除

Xcodeが自動生成したテンプレートファイルを削除します（私たちの実装と置き換えるため）。

**ホストアプリ (FreeAutotune target):**
1. `FreeAutotuneApp.swift` - 削除
2. `ContentView.swift` - 削除
3. `Assets.xcassets` - 保持

**Audio Unit Extension (FreeAutotuneAU target):**
1. `AudioUnitViewController.swift` - 削除
2. `FreeAutotuneAUAudioUnit.swift` - 削除
3. `Parameters.swift` - 削除
4. `DSPKernel.hpp` - 削除
5. `DSPKernel.mm` - 削除
6. `BufferedAudioBus.hpp` - 削除

### ステップ4: ソースファイルの追加

#### 4.1 ホストアプリのファイル追加

1. **FreeAutotune groupを右クリック → Add Files to "FreeAutotune"...**

2. **以下のファイルを選択:**
   ```
   FreeAutotune/HostApp/FreeAutotuneApp.swift
   ```

3. **Options:**
   - ✅ Copy items if needed (チェック)
   - ✅ Create groups (選択)
   - Add to targets: ✅ FreeAutotune のみチェック
   - **Add** をクリック

#### 4.2 Audio Unit ExtensionのSwiftファイル追加

1. **FreeAutotuneAU groupを右クリック → Add Files to "FreeAutotune"...**

2. **以下のファイルを選択:**
   ```
   FreeAutotune/Source/FreeAutotuneAU.swift
   FreeAutotune/Source/AudioUnitViewController.swift
   FreeAutotune/Source/ParameterAddress.swift
   ```

3. **Options:**
   - ✅ Copy items if needed
   - ✅ Create groups
   - Add to targets: ✅ FreeAutotuneAU のみチェック
   - **Add** をクリック

#### 4.3 DSPファイル（C++）の追加

1. **FreeAutotuneAU groupを右クリック → New Group**
   - 名前: `DSP`

2. **DSP groupを右クリック → Add Files to "FreeAutotune"...**

3. **以下のファイルを選択:**
   ```
   FreeAutotune/Source/DSP/PitchDetector.hpp
   FreeAutotune/Source/DSP/PitchDetector.cpp
   FreeAutotune/Source/DSP/PitchShifter.hpp
   FreeAutotune/Source/DSP/PitchShifter.cpp
   FreeAutotune/Source/DSP/AudioProcessor.hpp
   FreeAutotune/Source/DSP/AudioProcessor.cpp
   FreeAutotune/Source/DSP/DSPKernel.hpp
   FreeAutotune/Source/DSP/DSPKernel.cpp
   FreeAutotune/Source/DSP/DSPKernelAdapter.h
   FreeAutotune/Source/DSP/DSPKernelAdapter.mm
   ```

4. **Options:**
   - ✅ Copy items if needed
   - ✅ Create groups
   - Add to targets: ✅ FreeAutotuneAU のみチェック
   - **Add** をクリック

#### 4.4 Bridging Headerの追加

1. **FreeAutotuneAU groupを右クリック → Add Files to "FreeAutotune"...**

2. **ファイルを選択:**
   ```
   FreeAutotune/Source/FreeAutotune-Bridging-Header.h
   ```

3. **Options:**
   - ✅ Copy items if needed
   - Add to targets: ✅ FreeAutotuneAU のみチェック
   - **Add** をクリック

### ステップ5: Info.plistファイルの置き換え

#### 5.1 ホストアプリのInfo.plist

1. プロジェクトナビゲーターで自動生成された `Info.plist` を削除

2. **FreeAutotune group → Add Files to "FreeAutotune"...**
   ```
   FreeAutotune/HostApp/Info.plist
   ```

3. **Add to targets: ✅ FreeAutotune**

#### 5.2 Audio Unit ExtensionのInfo.plist

1. プロジェクトナビゲーターで自動生成された `Info.plist` を削除

2. **FreeAutotuneAU group → Add Files to "FreeAutotune"...**
   ```
   FreeAutotune/AudioUnitExtension-Info.plist
   ```

3. **Add to targets: ✅ FreeAutotuneAU**

### ステップ6: Entitlementsの追加

#### 6.1 ホストアプリのEntitlements

1. **FreeAutotune group → Add Files to "FreeAutotune"...**
   ```
   FreeAutotune/FreeAutotune.entitlements
   ```

2. **Add to targets: ✅ FreeAutotune**

#### 6.2 Audio Unit ExtensionのEntitlements

1. **FreeAutotuneAU group → Add Files to "FreeAutotune"...**
   ```
   FreeAutotune/FreeAutotuneAU.entitlements
   ```

2. **Add to targets: ✅ FreeAutotuneAU**

### ステップ7: ビルド設定

#### 7.1 FreeAutotuneAU Target（Audio Unit Extension）

1. **プロジェクトナビゲーター → プロジェクトファイルをクリック**

2. **TARGETS → FreeAutotuneAU を選択**

3. **Build Settings タブ**

4. **検索ボックスに "bridging" と入力**

5. **Swift Compiler - General → Objective-C Bridging Header**
   ```
   $(SRCROOT)/FreeAutotune/Source/FreeAutotune-Bridging-Header.h
   ```
   または相対パス:
   ```
   Source/FreeAutotune-Bridging-Header.h
   ```

6. **検索ボックスに "c++ language" と入力**

7. **Apple Clang - Language - C++ → C++ Language Dialect**
   ```
   C++17 [-std=c++17]
   ```

8. **検索ボックスに "c++ library" と入力**

9. **Apple Clang - Language - C++ → C++ Standard Library**
   ```
   libc++ (LLVM C++ standard library with C++11 support)
   ```

10. **検索ボックスに "info plist" と入力**

11. **Packaging → Info.plist File**
    ```
    AudioUnitExtension-Info.plist
    ```

12. **検索ボックスに "code signing entitlements" と入力**

13. **Signing → Code Signing Entitlements**
    ```
    FreeAutotuneAU.entitlements
    ```

#### 7.2 FreeAutotune Target（ホストアプリ）

1. **TARGETS → FreeAutotune を選択**

2. **Build Settings タブ**

3. **Packaging → Info.plist File**
   ```
   HostApp/Info.plist
   ```

4. **Signing → Code Signing Entitlements**
   ```
   FreeAutotune.entitlements
   ```

#### 7.3 Frameworksのリンク（FreeAutotuneAU Target）

1. **TARGETS → FreeAutotuneAU を選択**

2. **Build Phases タブ**

3. **Link Binary With Libraries を展開**

4. **+ ボタンをクリック**

5. **以下を順番に追加:**
   - `Accelerate.framework`
   - `AudioToolbox.framework`
   - `AVFoundation.framework`
   - `CoreAudio.framework`

#### 7.4 Compile Sources の確認（FreeAutotuneAU Target）

1. **Build Phases タブ**

2. **Compile Sources を展開**

3. **以下のファイルがすべて含まれていることを確認:**
   ```
   FreeAutotuneAU.swift
   AudioUnitViewController.swift
   ParameterAddress.swift
   PitchDetector.cpp
   PitchShifter.cpp
   AudioProcessor.cpp
   DSPKernel.cpp
   DSPKernelAdapter.mm
   ```

4. **`.mm` ファイルの Compiler Flags を確認:**
   - `DSPKernelAdapter.mm` をダブルクリック
   - Compiler Flags: `-x objective-c++` (自動設定されているはず)

### ステップ8: Signing & Capabilities

#### 8.1 FreeAutotuneAU Target

1. **TARGETS → FreeAutotuneAU を選択**

2. **Signing & Capabilities タブ**

3. **Team:** あなたのApple Developer Teamを選択

4. **Signing Certificate:**
   - Development: "Apple Development"
   - Release: "Developer ID Application"

5. **App Sandbox が有効になっていることを確認**
   - ✅ App Sandbox
   - ✅ Audio Input (必要に応じて)

#### 8.2 FreeAutotune Target

1. **TARGETS → FreeAutotune を選択**

2. **Signing & Capabilities タブ**

3. **Team:** 同じTeamを選択

4. **App Sandbox:** OFF (プラグインのため無効)

### ステップ9: Embed App Extension

1. **TARGETS → FreeAutotune を選択**

2. **General タブ**

3. **Frameworks, Libraries, and Embedded Content セクション**

4. **+ ボタンをクリック**

5. **FreeAutotuneAU.appex を選択**

6. **Embed:** "Embed & Sign" を選択

---

## ビルドと実行

### ステップ1: Schemeの選択

1. **Xcodeのツールバー（上部）でSchemeを選択**
   - **FreeAutotune** を選択（FreeAutotuneAUではない）

2. **デバイス:** "My Mac" を選択

### ステップ2: ビルド

1. **Product → Build** (⌘B)

2. **ビルドログを確認:**
   - 下部のパネルで進行状況を確認
   - エラーがないことを確認

### ステップ3: ビルド成果物の確認

1. **Product → Show Build Folder in Finder**

2. **以下の構造を確認:**
   ```
   Build/Products/Debug/
   └── FreeAutotune.app
       └── Contents/
           └── PlugIns/
               └── FreeAutotuneAU.appex
   ```

### ステップ4: インストール

#### 方法1: 手動コピー

```bash
# ビルドされた.appを /Applications にコピー
cp -R ~/Library/Developer/Xcode/DerivedData/FreeAutotune-*/Build/Products/Debug/FreeAutotune.app /Applications/

# または Release ビルドの場合
cp -R ~/Library/Developer/Xcode/DerivedData/FreeAutotune-*/Build/Products/Release/FreeAutotune.app /Applications/
```

#### 方法2: ビルドスクリプト使用

```bash
cd FreeAutotune
./build.sh
```

### ステップ5: Audio Component キャッシュのリセット

```bash
# Audio Component Registrarをリセット
killall -9 AudioComponentRegistrar

# Audio Unitキャッシュをクリア（オプション）
rm -rf ~/Library/Caches/AudioUnitCache
```

### ステップ6: Logic Proで確認

1. **Logic Proを起動**

2. **新規プロジェクトまたは既存プロジェクトを開く**

3. **オーディオトラックを選択**

4. **Audio FX → Audio Units → FreeAutotune**

5. **プラグインが表示されることを確認**

---

## トラブルシューティング

### ビルドエラー: "Bridging header not found"

**原因:** Bridging Headerのパスが間違っている

**解決策:**
1. Build Settings → Swift Compiler → Objective-C Bridging Header
2. パスを確認:
   ```
   $(SRCROOT)/FreeAutotune/Source/FreeAutotune-Bridging-Header.h
   ```
3. または、ファイルを右クリック → Show in Finder でパスを確認

### ビルドエラー: "Undefined symbols for architecture x86_64"

**原因:** C++ファイルがコンパイルされていない

**解決策:**
1. Build Phases → Compile Sources を確認
2. すべての.cppファイルが含まれているか確認
3. 含まれていない場合、+ ボタンで追加

### ビルドエラー: "Use of undeclared identifier"（C++コード内）

**原因:** C++標準が正しく設定されていない

**解決策:**
1. Build Settings → C++ Language Dialect
2. `C++17 [-std=c++17]` を選択

### Logic Proでプラグインが表示されない

**原因1:** App Extensionが正しく埋め込まれていない

**解決策:**
1. ホストアプリのGeneral → Frameworks, Libraries, and Embedded Content
2. FreeAutotuneAU.appex が "Embed & Sign" になっているか確認

**原因2:** Bundle Identifierが間違っている

**解決策:**
1. FreeAutotuneAU target → General → Bundle Identifier
2. `com.freeaudio.FreeAutotune.AudioUnit` であることを確認

**原因3:** Audio Component キャッシュが古い

**解決策:**
```bash
killall -9 AudioComponentRegistrar
rm -rf ~/Library/Caches/AudioUnitCache
```

### auvalで検証

プラグインが正しくインストールされているか確認:

```bash
# すべてのAudio Unitを表示
auval -a

# FreeAutotuneを検証
auval -v aufx Fatu Free

# 詳細出力
auval -v aufx Fatu Free -de
```

成功すると以下のような出力が得られます:
```
--------------------------------------------------
VALIDATING AUDIO UNIT: 'aufx' - 'Fatu' - 'Free'
--------------------------------------------------
Manufacturer String: Free
AudioUnit Name: FreeAutotune
Component Version: 1.0.0 (0x00010000)

* * PASS
--------------------------------------------------
```

### コード署名エラー

**原因:** Developer証明書がない、または期限切れ

**解決策:**
1. Xcode → Preferences → Accounts
2. Apple IDが追加されているか確認
3. "Manage Certificates..." で証明書を確認
4. 必要に応じて新しい証明書を作成

### ビルドは成功するが、音が出ない

**原因:** DSPコードが正しく接続されていない

**解決策:**
1. FreeAutotuneAU.swift の `internalRenderBlock` を確認
2. DSPKernelAdapter が正しく呼び出されているか確認
3. デバッガーでブレークポイントを設定して確認

---

## 高度な設定

### Release ビルド

1. **Product → Scheme → Edit Scheme...**

2. **Run → Info タブ**

3. **Build Configuration:** Release を選択

4. **Product → Build**

### コード署名とNotarization（配布用）

配布する場合は、Developer ID証明書でコード署名し、Appleの公証を受ける必要があります。

```bash
# コード署名
codesign --deep --force --verify --verbose \
  --sign "Developer ID Application: Your Name (TEAM_ID)" \
  /Applications/FreeAutotune.app

# 検証
codesign --verify --deep --strict --verbose=2 /Applications/FreeAutotune.app

# 公証（macOS 11.0以降）
xcrun notarytool submit FreeAutotune.zip \
  --apple-id "your@email.com" \
  --password "app-specific-password" \
  --team-id "TEAM_ID"
```

---

## まとめ

これで完全なビルド手順が完了しました！

**重要なポイント:**
1. AUV3はApp Extension形式
2. Bridging HeaderでSwiftとC++を接続
3. 正しいBundle Identifier階層
4. FrameworksとEntitlementsの設定
5. App Extensionの埋め込み

**次のステップ:**
- [USAGE.md](USAGE.md) で使い方を学ぶ
- [TECHNICAL.md](TECHNICAL.md) で内部実装を理解する
- パラメータを調整して最適な音を見つける

**問題が発生した場合:**
- GitHubのIssuesで質問
- auvalコマンドで検証
- Xcodeのビルドログを確認

Happy Autotuning! 🎵
