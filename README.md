# Android9 ADB Shell Password 保護

開発用途向けの簡易 **ADB shell 制限機構**です。
`/system/bin/sh` をラップすることで **ADB shell 実行時にパスワード制御を追加**します。

主に以下の用途を想定しています。

* 開発端末での **設定変更防止**
* **APK install の制限**
* ADB shell の利用を **パスワード付きにする**


---

# 概要

この仕組みでは以下のように動作します。

1. 既存 `/system/bin/sh` を改造して `xh` に変更
2. 新しい `sh` を配置
3. `sh → xh` を呼び出すラッパーとして利用
4. `adb shell` 実行時にパスワード制御

また、特定の `prop` 値で **パスワードの有無を切り替え可能**です。

---

# 必要条件

* `system.img` の編集が可能
* root / system 書き込み可能環境

---

# 導入方法

## 1. system.img を編集

端末の `system.img` を展開して編集します。

---

## 2. `/system/bin/sh` をバックアップ

```
/system/bin/sh
```

を PC にコピーしてバックアップします。

---

## 3. バイナリエディタで文字列を書き換え

PC のバイナリエディタで `sh` を開き、
以下のような文字列を探します。

```
/system/bin/sh
```

この部分の **`sh` を `xh` に変更**します。

例

```
/system/bin/sh
↓
/system/bin/xh
```

---

## 4. 編集したバイナリを配置

編集した `sh` を

```
xh
```

にリネームして以下へ配置します。

```
/system/bin/xh
```

権限を設定します。

```
chmod 0775 /system/bin/xh
chcon u:object_r:shell_exec:s0 /system/bin/xh
```

---

## 5. 新しい sh を配置

以下の URL から `sh` をダウンロードします。

```
(https://github.com/coara-chocomaru/shell_lock/releases/download/test/sh)
```

ダウンロードした `sh` を配置します。

```
/system/bin/sh
```

同じく権限を設定します。

```
chmod 0775 /system/bin/sh
chcon u:object_r:shell_exec:s0 /system/bin/sh
```

---

# 導入完了

これで導入は完了です。

---

# 機能

### 1. ADB shell パスワード制限

`adb shell` 実行時にパスワードを要求します。

---

### 2. prop による制御

特定の `prop` 値により

* パスワード **有効**
* パスワード **無効**

を切り替えることができます。

---

# 利用用途

主に以下のような用途で使用できます。

* 端末設定の変更防止
* `adb install` の防止
* ADB shell の制限

---

# デメリット

以下の制限があります。

```
adb shell -c
```

のような **コマンド実行モードは使用できません。**

---

# 注意

この機能は **開発用途向けの簡易的な制御**です。
