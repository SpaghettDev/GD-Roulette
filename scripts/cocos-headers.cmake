include(FetchContent)

FetchContent_Declare(
    cocos-headers
    GIT_REPOSITORY https://github.com/HJfod/cocos-headers.git
    GIT_TAG        bb1ee6ec99d94d02fb1654cae1ebc9b196cda8dc
    GIT_PROGRESS TRUE
)
message("Fetching cocos-headers")
FetchContent_MakeAvailable(cocos-headers)