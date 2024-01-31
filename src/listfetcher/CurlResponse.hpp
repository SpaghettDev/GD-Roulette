#pragma once
#include "../pch.hpp"
#include <string>

struct curlResponse
{
	CURLcode responseCode;
	std::string info;
	nlohmann::json jsonResponse;

	curlResponse()
		: responseCode(CURLcode::CURLE_HTTP_RETURNED_ERROR), info(""), jsonResponse({}) {}

	curlResponse(CURLcode responseCode, std::string info, nlohmann::json jsonResponse)
		: responseCode(responseCode), info(info), jsonResponse(jsonResponse) {}
};
