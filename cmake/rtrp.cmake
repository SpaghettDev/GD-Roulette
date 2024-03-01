include(FetchContent)

FetchContent_Declare(
    RTRP
    GIT_REPOSITORY https://github.com/SpaghettDev/RTRP.git
    GIT_TAG        53fe19f867b4b52b241f4c0522299896cb32c628
    GIT_PROGRESS TRUE
)
message("Fetching RTRP")
FetchContent_MakeAvailable(RTRP)
