# Contributing to Real Tune

Real Tuneへの貢献に興味を持っていただき、ありがとうございます！このドキュメントは、プロジェクトに貢献する方法を説明します。

## 目次

1. [行動規範](#行動規範)
2. [貢献の方法](#貢献の方法)
3. [開発環境のセットアップ](#開発環境のセットアップ)
4. [コーディング規約](#コーディング規約)
5. [プルリクエストの作成](#プルリクエストの作成)
6. [イシューの報告](#イシューの報告)

## 行動規範

このプロジェクトは以下の原則に基づいています：

- 建設的で敬意あるコミュニケーション
- 多様性と包括性の尊重
- オープンで透明性のある開発プロセス

## 貢献の方法

### バグ報告

バグを発見した場合：

1. GitHubのIssuesで既存の報告を確認
2. 重複がなければ新しいIssueを作成
3. 以下の情報を含める：
   - macOSバージョン
   - Logic Proバージョン
   - Real Tuneバージョン
   - 再現手順
   - 期待される動作
   - 実際の動作
   - スクリーンショット（可能であれば）

### 機能リクエスト

新機能の提案：

1. GitHubのDiscussionsで議論を開始
2. ユースケースを説明
3. 実装案があれば共有
4. コミュニティのフィードバックを待つ

### コード貢献

1. **フォークとクローン**
   ```bash
   git clone https://github.com/your-username/Real Tune.git
   cd Real Tune
   ```

2. **ブランチ作成**
   ```bash
   git checkout -b feature/your-feature-name
   # または
   git checkout -b fix/your-bug-fix
   ```

3. **変更を実装**
   - コーディング規約に従う
   - テストを追加
   - ドキュメントを更新

4. **コミット**
   ```bash
   git add .
   git commit -m "Add: 機能の説明"
   ```

5. **プッシュ**
   ```bash
   git push origin feature/your-feature-name
   ```

6. **プルリクエスト作成**

## 開発環境のセットアップ

### 必要なツール

```bash
# Xcode Command Line Tools
xcode-select --install

# CMake (Homebrewで)
brew install cmake

# (オプション) clang-format
brew install clang-format
```

### ビルド手順

```bash
cd Real Tune
mkdir build && cd build
cmake -G Xcode ..
open Real Tune.xcodeproj
```

### テストの実行

```bash
# ユニットテスト
./build/test_pitch_detection

# ベンチマーク
./build/benchmark

# Audio Unit検証
auval -v aufx Fatu Free
```

## コーディング規約

### C++コード

**命名規則**
```cpp
// クラス: PascalCase
class PitchDetector {
public:
    // メソッド: camelCase
    void detectPitch();

private:
    // メンバ変数: camelCase_
    float sampleRate_;
    int bufferSize_;

    // 定数: UPPER_CASE
    static constexpr float MIN_FREQUENCY = 80.0f;
};
```

**フォーマット**
- インデント: 4スペース
- 中括弧: K&Rスタイル
- 行の長さ: 100文字以内

**コメント**
```cpp
/// Brief description
/// @param parameter Parameter description
/// @return Return value description
float calculateFrequency(float period);
```

### Swiftコード

**命名規則**
```swift
// クラス/構造体: PascalCase
class AudioUnitViewController {
    // プロパティ: camelCase
    var audioUnit: Real TuneAU?

    // メソッド: camelCase
    func createAudioUnit() {
        // ...
    }

    // 定数: camelCase
    private let maxFrames = 512
}
```

**SwiftUIビュー**
```swift
struct ParameterSliderView: View {
    // @Bindingの使用
    @Binding var value: Double

    var body: some View {
        // Viewの実装
    }
}
```

### コードレビュー基準

プルリクエストは以下を確認します：

1. **機能性**
   - 意図した通りに動作するか
   - エッジケースを処理しているか
   - リグレッションがないか

2. **パフォーマンス**
   - リアルタイム制約を満たすか
   - メモリリークがないか
   - CPU使用率が適切か

3. **コード品質**
   - 可読性が高いか
   - 適切にコメントされているか
   - DRY原則に従っているか

4. **テスト**
   - ユニットテストがあるか
   - テストカバレッジが十分か
   - エッジケースをテストしているか

5. **ドキュメント**
   - APIドキュメントが更新されているか
   - READMEが更新されているか
   - 使用例があるか

## プルリクエストの作成

### PRテンプレート

```markdown
## 変更の概要
[変更内容を簡潔に説明]

## 変更の種類
- [ ] バグ修正
- [ ] 新機能
- [ ] パフォーマンス改善
- [ ] ドキュメント更新
- [ ] リファクタリング

## テスト
- [ ] ユニットテストを追加/更新
- [ ] 手動テストを実施
- [ ] auval検証をパス

## チェックリスト
- [ ] コーディング規約に従っている
- [ ] ドキュメントを更新した
- [ ] コミットメッセージが明確
- [ ] ビルドエラーがない
- [ ] 既存のテストがパスする

## 関連Issue
Closes #[issue番号]

## スクリーンショット
[該当する場合、スクリーンショットを追加]
```

### コミットメッセージ規約

**フォーマット**
```
タイプ: 簡潔な説明 (50文字以内)

詳細な説明 (必要に応じて)

Closes #123
```

**タイプ**
- `Add`: 新機能追加
- `Fix`: バグ修正
- `Update`: 既存機能の更新
- `Refactor`: リファクタリング
- `Docs`: ドキュメント変更
- `Test`: テスト追加/修正
- `Perf`: パフォーマンス改善
- `Style`: コードスタイル修正

**例**
```
Add: YINアルゴリズムにパラボリック補間を追加

ピッチ検出の精度を向上させるため、YINアルゴリズムに
パラボリック補間を実装しました。

- サブサンプル精度の推定が可能に
- 精度が約5%向上

Closes #42
```

## イシューの報告

### バグレポートテンプレート

```markdown
## バグの説明
[バグの明確で簡潔な説明]

## 再現手順
1. [最初のステップ]
2. [2番目のステップ]
3. [...]

## 期待される動作
[何が起こるべきかの明確で簡潔な説明]

## 実際の動作
[実際に何が起こったかの説明]

## スクリーンショット
[該当する場合、スクリーンショットを追加]

## 環境
- macOSバージョン: [例: 14.2]
- Logic Proバージョン: [例: 10.8]
- Real Tuneバージョン: [例: 1.0.0]
- チップ: [例: Apple M1]

## 追加情報
[その他の関連情報]
```

### 機能リクエストテンプレート

```markdown
## 機能の説明
[機能の明確で簡潔な説明]

## ユースケース
[この機能がどのように使用されるか]

## 提案する実装
[実装のアイデアがあれば]

## 代替案
[検討した代替ソリューション]

## 追加情報
[その他の関連情報]
```

## 開発のベストプラクティス

### リアルタイム処理

```cpp
// ❌ 悪い例
void processAudio(float* buffer, int size) {
    std::vector<float> temp(size);  // アロケーション!
    mutex.lock();                    // ロック!
    printf("Processing...");         // システムコール!
}

// ✅ 良い例
void processAudio(float* buffer, int size) {
    // 事前にアロケートされたバッファを使用
    std::copy(buffer, buffer + size, preAllocatedBuffer_);
    // アトミック操作のみ
    // ロックフリーアルゴリズム
}
```

### メモリ管理

```cpp
// 初期化時にアロケート
void initialize(float sampleRate, int maxBufferSize) {
    inputBuffer_.resize(maxBufferSize);
    outputBuffer_.resize(maxBufferSize);
}

// 処理時はアロケーションしない
void process(const float* input, float* output, int size) {
    // 既存のバッファを再利用
}
```

### エラーハンドリング

```cpp
// ❌ 悪い例
float detectPitch(const float* buffer, int size) {
    // バウンダリーチェックなし
    return computePitch(buffer, size);
}

// ✅ 良い例
float detectPitch(const float* buffer, int size) {
    if (!buffer || size <= 0 || size > maxBufferSize_) {
        return 0.0f;  // エラー値を返す
    }
    return computePitch(buffer, size);
}
```

## コミュニティ

### サポートを受ける

- **GitHub Discussions**: 一般的な質問や議論
- **GitHub Issues**: バグ報告や機能リクエスト
- **Pull Requests**: コードレビューとフィードバック

### 貢献者になる

定期的に高品質な貢献をする方には：
- CONTRIBUTORS.mdへの追加
- プロジェクトの意思決定への参加
- メンテナー権限の付与（適切な場合）

## 謝辞

Real Tuneは以下の方々の貢献により成り立っています：
- すべてのコントリビューター
- イシューを報告してくれた方々
- ドキュメントを改善してくれた方々
- プロジェクトを使用・フィードバックをくれた方々

## 質問がある場合

わからないことがあれば、遠慮なく：
- GitHub Discussionsで質問
- Issueを作成
- プルリクエストでドラフトとしてフィードバックを求める

私たちは初心者の貢献者を歓迎します！

---

**ハッピーコーディング！** 🎵
