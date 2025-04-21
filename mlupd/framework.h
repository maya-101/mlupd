#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <shellapi.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <variant>
#include <shlobj.h>     // SHGetKnownFolderPath
#include <wincrypt.h>
#include <iomanip>
#include <thread>
#include <future>
#include <filesystem>

#include "curl/curl.h"

#include "json.hpp"
using json = nlohmann::json;
