include(FetchContent)

set(BUILD_CURL_EXE OFF CACHE INTERNAL "" FORCE)
set(BUILD_TESTING OFF CACHE INTERNAL "" FORCE)
set(HTTP_ONLY ON CACHE INTERNAL "" FORCE)
if(NOT CMAKE_USE_OPENSSL)
    set(CURL_CA_PATH "none" CACHE INTERNAL "" FORCE)
endif()
set(SSL_ENABLED ${CMAKE_USE_OPENSSL} CACHE INTERNAL "" FORCE)

FetchContent_Declare(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG        curl-8_0_1
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
message("Fetching curl")

FetchContent_MakeAvailable(curl)