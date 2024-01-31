#pragma once

#include <nlohmann/json.hpp>

#include <windows.h>
#include <psapi.h>
#include <WinSock2.h>
#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif
#include <curl/curl.h>
#include <fstream>

#include "common.hpp"
