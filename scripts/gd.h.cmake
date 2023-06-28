include(FetchContent)

FetchContent_Declare(
    gd.h
    GIT_REPOSITORY https://github.com/SpaghettDev/gd.h.git
    GIT_TAG        4ed18379f97742cbd8258a5f4f89905dbb9d6f6f
    GIT_PROGRESS TRUE
)
message("Fetching gd.h")
FetchContent_MakeAvailable(gd.h)