#pragma once
#include "pch.hpp"
#include <string>
#include <random>
#include "custom_layers/base/BaseCustomLayer.hpp"

namespace roulette::utils
{
	inline int randomInt(int min, int max)
	{
		std::random_device device;
		std::mt19937 generator(device());
		std::uniform_int_distribution<int> distribution(min, max);

		return distribution(generator);
	}

	inline void setClipboardText(std::string text)
	{
		if (OpenClipboard(0))
		{
			HGLOBAL clipbuffer;
			char* buffer;
			EmptyClipboard();

			clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
			if (clipbuffer)
			{
				buffer = static_cast<char*>(GlobalLock(clipbuffer));
				if (buffer)
				{
					strcpy(buffer, text.c_str());
					GlobalUnlock(clipbuffer);
					SetClipboardData(CF_TEXT, clipbuffer);
				}
			}

			CloseClipboard();
		}
	}

	template<typename T, std::size_t S>
	inline std::ptrdiff_t getIndexOf(const std::array<T, S>& arr, T to_find)
	{
		auto it = std::find(arr.cbegin(), arr.cend(), to_find);

		return it != arr.cend() ? (it - arr.cbegin()) : -1;
	}

	inline CCLabelBMFont* createTextLabel(const std::string& text, const CCPoint& position, const float scale, CCNode* menu, const char* font = "bigFont.fnt")
	{
		CCLabelBMFont* bmFont = CCLabelBMFont::create(text.c_str(), font);
		bmFont->setPosition(position);
		bmFont->setScale(scale);
		menu->addChild(bmFont);

		return bmFont;
	}

	inline CCMenuItemSpriteExtra* createButton(BaseCustomLayer* self, const char* texture, CCPoint position, SEL_MenuHandler callback, int tag = -1, float textureScale = 1.f, float sizeMult = 1.2f)
	{
		auto buttonSprite = CCSprite::createWithSpriteFrameName(texture);
		buttonSprite->setScale(textureScale);
		auto button = CCMenuItemSpriteExtra::create(
			buttonSprite,
			self,
			callback
		);
		button->setPosition(position);
		button->setSizeMult(sizeMult);
		if (tag != -1)
			button->setTag(tag);
		self->addChild(button);

		return button;
	}

	namespace vars
	{
		inline std::array<const char*, CURLcode::CURL_LAST + 1> curlCodeToString{
			"CURLE_OK",
			"CURLE_UNSUPPORTED_PROTOCOL",
			"CURLE_FAILED_INIT",
			"CURLE_URL_MALFORMAT",
			"CURLE_NOT_BUILT_IN",
			"CURLE_COULDNT_RESOLVE_PROXY",
			"CURLE_COULDNT_RESOLVE_HOST",
			"CURLE_COULDNT_CONNECT",
			"CURLE_WEIRD_SERVER_REPLY",
			"CURLE_REMOTE_ACCESS_DENIED",
			"CURLE_FTP_ACCEPT_FAILED",
			"CURLE_FTP_WEIRD_PASS_REPLY",
			"CURLE_FTP_ACCEPT_TIMEOUT",
			"CURLE_FTP_WEIRD_PASV_REPLY",
			"CURLE_FTP_WEIRD_227_FORMAT",
			"CURLE_FTP_CANT_GET_HOST",
			"CURLE_HTTP2",
			"CURLE_FTP_COULDNT_SET_TYPE",
			"CURLE_PARTIAL_FILE",
			"CURLE_FTP_COULDNT_RETR_FILE",
			"CURLE_OBSOLETE20",
			"CURLE_QUOTE_ERROR",
			"CURLE_HTTP_RETURNED_ERROR",
			"CURLE_WRITE_ERROR",
			"CURLE_OBSOLETE24",
			"CURLE_UPLOAD_FAILED",
			"CURLE_READ_ERROR",
			"CURLE_OUT_OF_MEMORY",
			"CURLE_OPERATION_TIMEDOUT",
			"CURLE_OBSOLETE29",
			"CURLE_FTP_PORT_FAILED",
			"CURLE_FTP_COULDNT_USE_REST",
			"CURLE_OBSOLETE32",
			"CURLE_RANGE_ERROR",
			"CURLE_HTTP_POST_ERROR",
			"CURLE_SSL_CONNECT_ERROR",
			"CURLE_BAD_DOWNLOAD_RESUME",
			"CURLE_FILE_COULDNT_READ_FILE",
			"CURLE_LDAP_CANNOT_BIND",
			"CURLE_LDAP_SEARCH_FAILED",
			"CURLE_OBSOLETE40",
			"CURLE_FUNCTION_NOT_FOUND",
			"CURLE_ABORTED_BY_CALLBACK",
			"CURLE_BAD_FUNCTION_ARGUMENT",
			"CURLE_OBSOLETE44",
			"CURLE_INTERFACE_FAILED",
			"CURLE_OBSOLETE46",
			"CURLE_TOO_MANY_REDIRECTS",
			"CURLE_UNKNOWN_OPTION",
			"CURLE_SETOPT_OPTION_SYNTAX",
			"CURLE_OBSOLETE50",
			"CURLE_OBSOLETE51",
			"CURLE_GOT_NOTHING",
			"CURLE_SSL_ENGINE_NOTFOUND",
			"CURLE_SSL_ENGINE_SETFAILED",
			"CURLE_SEND_ERROR",
			"CURLE_RECV_ERROR",
			"CURLE_OBSOLETE57",
			"CURLE_SSL_CERTPROBLEM",
			"CURLE_SSL_CIPHER",
			"CURLE_PEER_FAILED_VERIFICATION",
			"CURLE_BAD_CONTENT_ENCODING",
			"CURLE_OBSOLETE62",
			"CURLE_FILESIZE_EXCEEDED",
			"CURLE_USE_SSL_FAILED",
			"CURLE_SEND_FAIL_REWIND",
			"CURLE_SSL_ENGINE_INITFAILED",
			"CURLE_LOGIN_DENIED",
			"CURLE_TFTP_NOTFOUND",
			"CURLE_TFTP_PERM",
			"CURLE_REMOTE_DISK_FULL",
			"CURLE_TFTP_ILLEGAL",
			"CURLE_TFTP_UNKNOWNID",
			"CURLE_REMOTE_FILE_EXISTS",
			"CURLE_TFTP_NOSUCHUSER",
			"CURLE_OBSOLETE75",
			"CURLE_OBSOLETE76",
			"CURLE_SSL_CACERT_BADFILE",
			"CURLE_REMOTE_FILE_NOT_FOUND",
			"CURLE_SSH",
			"CURLE_SSL_SHUTDOWN_FAILED",
			"CURLE_AGAIN",
			"CURLE_SSL_CRL_BADFILE",
			"CURLE_SSL_ISSUER_ERROR",
			"CURLE_FTP_PRET_FAILED",
			"CURLE_RTSP_CSEQ_ERROR",
			"CURLE_RTSP_SESSION_ERROR",
			"CURLE_FTP_BAD_FILE_LIST",
			"CURLE_CHUNK_FAILED",
			"CURLE_NO_CONNECTION_AVAILABLE",
			"CURLE_SSL_PINNEDPUBKEYNOTMATCH",
			"CURLE_SSL_INVALIDCERTSTATUS",
			"CURLE_HTTP2_STREAM",
			"CURLE_RECURSIVE_API_CALL",
			"CURLE_AUTH_ERROR",
			"CURLE_HTTP3",
			"CURLE_QUIC_CONNECT_ERROR",
			"CURLE_PROXY",
			"CURLE_SSL_CLIENTCERT",
			"CURLE_UNRECOVERABLE_POLL",
			"CURL_LAST"
		};
	}
}
