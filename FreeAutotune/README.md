# FreeAutotune - Logic Pro用高性能オートチューンプラグイン

Logic Pro対応の無料・高性能オートチューンプラグイン。リアルタイム処理に最適化された設計。

## ⚡ 特徴

- **Audio Unit V3 (AUV3)**: 最新のプラグイン規格に完全対応
- **超低レイテンシー**: リアルタイムレコーディングに対応（約11ms）
- **高精度ピッチ検出**: YINアルゴリズムベース（80-1000Hz対応）
- **自然なピッチ補正**: フォルマント保存型TD-PSOLA実装
- **直感的UI**: SwiftUIによるモダンなインターフェース
- **高パフォーマンス**: Accelerate Framework活用、Apple Silicon最適化

## 主な機能

- リアルタイムピッチ補正
- 補正強度調整（Retune Speed）
- キー/スケール設定
- 自然〜ロボット声まで対応
- CPU使用率最適化

## システム要件

- macOS 11.0以降
- Logic Pro 10.7以降
- Apple Silicon / Intel両対応

## 🔧 ビルド方法

FreeAutotuneはAudio Unit V3 (AUV3) App Extensionとして実装されています。

### 必要なもの
- macOS 11.0以降
- Xcode 13.0以降
- Apple Developer証明書（開発用）

### Xcodeでビルド（推奨）

詳細な手順は **[BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)** を参照してください。

#### クイックスタート

1. **Xcodeプロジェクトを作成**
   ```bash
   cd FreeAutotune
   # Xcodeで手動プロジェクト作成（File → New → Project）
   # または generate_xcode_project.rb を使用
   ```

2. **ソースファイルを追加**
   - ホストアプリ: `HostApp/*`
   - Audio Unit Extension: `Source/*`

3. **ビルド設定**
   - Swift-C++ Bridging Header を設定
   - C++17標準を指定
   - Accelerate Frameworkをリンク

4. **ビルド**
   ```bash
   xcodebuild -scheme FreeAutotune -configuration Release
   ```

### インストール

ビルド後、App全体を `/Applications` にコピー：
```bash
cp -R build/Release/FreeAutotune.app /Applications/
```

プラグイン（App Extension）は以下に含まれます：
```
/Applications/FreeAutotune.app/Contents/PlugIns/FreeAutotuneAU.appex
```

## 使い方

1. Logic Proでトラックを選択
2. Audio FX → Audio Units → FreeAutotune を追加
3. キーとスケールを設定
4. Retune Speedで補正強度を調整

## パラメータ

- **Retune Speed**: ピッチ補正の速度（0-100%）
- **Key**: 基準キー（C-B）
- **Scale**: スケール（Major/Minor/Chromatic）
- **Mix**: Dry/Wetバランス

## ライセンス

MIT License - 商用・非商用問わず自由に使用可能

## 技術仕様

- ピッチ検出: YINアルゴリズム
- ピッチシフト: TD-PSOLA
- バッファサイズ: 512サンプル
- サンプリングレート: 44.1kHz - 192kHz対応
