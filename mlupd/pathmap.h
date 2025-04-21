#pragma once

#include <string>
#include <windows.h>

namespace mlupd::pathmap {

	// 実行ファイルパスからSHA1ハッシュ（40文字の16進）を取得
	std::string HashPath(const std::string& path);

	// AppData\mlupd\pathmap\<hash>\path.json のパスを取得
	std::string GetPathmapJsonPath(const std::string& exePath);

	// 実体パスをpathmapに保存（書き込み）
	bool WritePathmap(const std::string& exePath);

	// 実体パスをpathmapから読み出し（読み込み）
	bool ReadPathmap(const std::string& exePath, std::string& outPath);

} // namespace mlupd::pathmap
