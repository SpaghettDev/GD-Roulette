include(FetchContent)

FetchContent_Declare(
    libcurl
    GIT_REPOSITORY https://github.com/SpaghettDev/libcurl.git
    GIT_TAG        940b62c90605f3630818c17548d6beefb417b0dd
    GIT_PROGRESS TRUE
)
message("Fetching libcurl")
FetchContent_MakeAvailable(libcurl)