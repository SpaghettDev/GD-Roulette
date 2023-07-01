include(FetchContent)

FetchContent_Declare(
    gd.h
    GIT_REPOSITORY https://github.com/SpaghettDev/gd.h.git
    GIT_TAG        8fa5fbdaac85ea01ee3bbdeb6fd15ad49fabb186
    GIT_PROGRESS TRUE
)
message("Fetching gd.h")
FetchContent_MakeAvailable(gd.h)