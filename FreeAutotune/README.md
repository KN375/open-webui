# FreeAutotune - Logic Pro用高性能オートチューンプラグイン

Logic Pro対応の無料・高性能オートチューンプラグイン。リアルタイム処理に最適化された設計。

## 特徴

- **超低レイテンシー**: リアルタイムレコーディングに対応
- **高精度ピッチ検出**: YINアルゴリズムベース
- **自然なピッチ補正**: フォルマント保存型PSOLA実装
- **直感的UI**: SwiftUIによるモダンなインターフェース
- **Audio Unit V3**: Logic Pro完全対応

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

## ビルド方法

```bash
cd FreeAutotune
mkdir build && cd build
cmake -G Xcode ..
open FreeAutotune.xcodeproj
```

Xcodeでビルド後、プラグインは自動的にインストールされます。

## インストール

ビルド後、以下にプラグインがインストールされます：
```
~/Library/Audio/Plug-Ins/Components/FreeAutotune.component
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
