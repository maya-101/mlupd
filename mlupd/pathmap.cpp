#include "framework.h"
#include "mlupd.h"
#include "curl/curl.h"
#include "json.hpp"
#include "Resource.h"
#include "pathmap.h"

// pathmap_utils.cpp

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace mlupd::pathmap {

    std::string HashPath(const std::string& path) {
        HCRYPTPROV hProv = 0;
        HCRYPTHASH hHash = 0;
        BYTE rgbHash[20]; // SHA1 = 20 bytes
        DWORD cbHash = sizeof(rgbHash);
        CHAR rgbDigits[] = "0123456789abcdef";

        if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) return "";
        if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) {
            CryptReleaseContext(hProv, 0);
            return "";
        }

        CryptHashData(hHash, (BYTE*)path.data(), (DWORD)path.size(), 0);
        CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0);

        std::ostringstream oss;
        for (DWORD i = 0; i < cbHash; ++i)
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)rgbHash[i];

        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return oss.str();
    }

    std::string GetAppDataPath() {
        char path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
            return std::string(path);
        }
        return "";
    }

    std::string GetPathmapJsonPath(const std::string& exePath) {
        std::string hash = HashPath(exePath);
        std::string base = GetAppDataPath() + "\\mlupd\\pathmap\\" + hash;
        fs::create_directories(base);
        return base + "\\path.json";
    }

    bool WritePathmap(const std::string& exePath) {
        std::string jsonPath = GetPathmapJsonPath(exePath);
        json j;
        j["path"] = exePath;
        std::ofstream ofs(jsonPath);
        if (!ofs) return false;
        ofs << j.dump(2);
        return true;
    }

    bool ReadPathmap(const std::string& exePath, std::string& outPath) {
        std::string jsonPath = GetPathmapJsonPath(exePath);
        std::ifstream ifs(jsonPath);
        if (!ifs) return false;
        json j;
        ifs >> j;
        if (j.contains("path")) {
            outPath = j["path"].get<std::string>();
            return true;
        }
        return false;
    }

} // namespace mlupd::pathmap
