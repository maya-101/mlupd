#pragma once

#include <string>
#include <windows.h>

namespace mlupd::pathmap {

	// ���s�t�@�C���p�X����SHA1�n�b�V���i40������16�i�j���擾
	std::string HashPath(const std::string& path);

	// AppData\mlupd\pathmap\<hash>\path.json �̃p�X���擾
	std::string GetPathmapJsonPath(const std::string& exePath);

	// ���̃p�X��pathmap�ɕۑ��i�������݁j
	bool WritePathmap(const std::string& exePath);

	// ���̃p�X��pathmap����ǂݏo���i�ǂݍ��݁j
	bool ReadPathmap(const std::string& exePath, std::string& outPath);

} // namespace mlupd::pathmap
