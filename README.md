【RAWモードで表示してください。】

【名前】
mlupd

【目的】
配布アプリケーションの自動バージョンチェック、自動ダウンロード、自動アップデートを行う

【動作OS】
windows 10, 11(64bit)

【ライセンス】
*mlupd
私的利用、商用利用を問わず、配布は自由です。
著作者または著作権者は、ソフトウェアまたはソフトウェアの使用に関して一切の責任を負わないこととします。﻿

*curl
本ソフトウェアは、curl（https://curl.se）を利用しています。
curlはDaniel Stenberg氏によって開発され、MITライセンスのもと提供されています。

*nlohmann-json
本ソフトウェアは、nlohmann-jsonを利用しています。
https://github.com/nlohmann/json/blob/develop/LICENSE.MIT

【使用方法】
* mlupd 導入
  1. github から mlupd.exe とその他ファイル（libcurl.dll、curl-ca-bundle.crt) をダウンロードする。
  https://github.com/maya-101/mlupd/releases/latest

* mlupdの事前設定
  1. 配布アプリケーションと同じパスに mlupd.exe とその他ファイルを配置する。
  2. コマンドラインで mlupd.exe --config を実行。
  3. 設定ダイアログでアップデータファイル名、URL、バージョンなど各種設定を行う。OKボタンを押す。
  4. mlupd.exe と同じ場所に、mlupd.config.json ファイルができるので、これをサーバーのアップデーターと同じ場所にアップロードしておく。
  5. アプリケーション配布のイメージに、上記 mlupd.exe とその他ファイル(libcurl.dll、curl-ca-bundle.crt)に加え、mlupd.config.json を含める。

* アプリケーションに組み込み(サンプルコード提示)
  1. 配布アプリケーションで起動時など任意のタイミングで "mlupd.exe --pwd=xxxxxxx --check-only" を実行する。
  2. 戻り値に従い、最新版がある場合は(戻り値1)、ユーザーに問い合わせて、ダウンロードとアップデートを開始する。
     "mlupd.exe --pwd=xxxxxxx" を実行する。
  3. アップデートが始まる前にアプリケーションは終了しておくこと。

* 以降、バージョンアップ時の作業
  1. コマンドラインで mlupd.exe --config を実行。
  2. 設定ダイアログで新しいバージョンに更新する。OKボタンを押す。
  3. mlupd.exe と同じ場所に、mlupd.config.json ファイルができるので、これをサーバーのアップデーターと同じ場所にアップロードしておく。
  4. アプリケーション配布のイメージに含まれている mlupd.config.json を更新する。

【コマンドライン仕様】
mlupd.exe [各種オプション 下を参照]
--configfile= コンフィグファイル名を指定する。省略時は、mlupd.config.jsonという名前のファイルを検索する。コンフィグファイルはmlupd.exeと同じディレクトリに置くこと。
--username= サーバーアクセスのユーザー名。
--password= サーバーアクセスのパスワード。
--check-only サーバーに新しいバージョンのアップデータがあるかチェックする。ダウンロードはしない
--download-only サーバーに新しいバージョンのアップデータが有った場合ダウンロードする。インストールは実行しない。
--config 設定ダイアログを表示する。各種設定を行う。mlupd.exeと同じ場所にコンフィグファイルが出力されるので、これをサーバーのアップデータと同じ場所にコピーしておく。
--help コマンドラインの書式を表示する。
--force-update ユーザーに問い合わせずに強制的に更新する。コンフィグファイルのforce-updateと同じです。
--parent-window-handle=xxx 呼び出し側のウィンドウハンドルを指定する(指定した場合は、そのウィンドウの中央に進捗などを表示します)
--skip-current-version 現在サーバーに上がっているバージョンをスキップします。
--no-version-skip この呼び出し時だけ、バーションスキップを一時的に無効化します。
--cancel-version-skip バージョンスキップを無効化します。

【戻り値】
0 配布アプリケーションは最新である。または正常にダウンロードできた。
1 --check-only指定時に、新しいバージョンが見つかった場合。
0x1001 コンフィグファイルが見つからなかった。
0x1002 コンフィグファイルを開けなかった。
0x1003 コンフィグファイルのダウンロードに失敗した。
0x1004 ファイルを出力できなかった。
0x2nnn CURLエラー。nnnの部分がCURLのエラーコードとなる。 https://curl.se/libcurl/c/libcurl-errors.html を参照。

【コンフィグファイル仕様】
target_filename  : アップデーターのファイル名(パスは含めない)
target_url       : アップデータのダウンロードURL(ファイル名は含めない)
target_version   : アップデータのバージョン
install_command  : 実行するインストーラー名（xxxxxx.exeなど）。パスは含めない。
install_option   : 実行するインストーラーにわたす引数。/quietなど
force_update     : バージョンの如何にかかわらず、アップデートを強制する。
show_progress    : ダウンロード進捗を表示する
expected_sha256  : ハッシュ値(コンフィグが自動計算)あるいは無し。※2

コンフィグファイルの例
{
  "target_filename"   : "MyAppInstaller.exe",
  "target_url"        : "https://example.com/MyAppInstaller.exe",
  "target_version"    : "1.2.2",
  "install_command"   : "MyAppInstaller.exe",
  "install_option"    : "/quiet",
  "skip_version_file" : "",
  "show_progress"     : true,
  "expected_sha256"   : "abc123...789def"
}

【ビルド】

*libcurlビルド
VisualStudio2022でのビルド例を示す。
以下から、curlのソースコードをダウンロードする。https://curl.se/download/
ここでは7.87.0をダウンロードしたものとする。
※最新のだと上手くビルドが通らなかったので、webのビルド例を元に適当な過去バージョンをダウンロードした。
ダウンロードしたファイルを展開する。
x64 Native Tools Command Prompt for VS 2022を起動して
展開したソースコードのwinbuildディレクトリに移動する。
以下を実行
> nmake /f Makefile.vc mode=dll
buildディレクトリにの下、libcurl-vc-x64-release-dll...というフォルダが出来上がってるはずなので
bin, include, libという3つのフォルダを、nmlib/libcurlディレクトリにコピーする。

mlupd/libcurl/
       ├── bin/         <--
       ├── include/     <--
       └── lib/         <-- これら

さらに、binフォルダにlibcurl.dllがあるので、これをmlupd.exeと同じディレクトリにコピーしておく。

*cacert.pemの入手
以下から、
 https://curl.se/docs/caextract.html
cacert.pemをダウンロード。
ダウンロードしたら、curl-ca-bundle.crtという名前にリネームして、mlupd.exeと同じディレクトリにおく。


【現時点での制限】
* ハッシュのチェックは未実装
* コンフィグファイルのアップロードは未実装
