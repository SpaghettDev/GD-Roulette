include(FetchContent)

FetchContent_Declare(
    gd.h
    GIT_REPOSITORY https://github.com/SpaghettDev/gd.h.git
    GIT_TAG        87360632027b9b2bdde67e46459c01288b4f41fb
    GIT_PROGRESS TRUE
)
message("Fetching gd.h")
FetchContent_MakeAvailable(gd.h)