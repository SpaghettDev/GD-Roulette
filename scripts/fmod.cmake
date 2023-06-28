include(FetchContent)

FetchContent_Declare(
    fmod
    GIT_REPOSITORY https://github.com/SpaghettDev/fmod.git
    GIT_TAG        f51f917afe73bc7c51734aa01c7837ac321384cf
    GIT_PROGRESS TRUE
)
message("Fetching fmod")
FetchContent_MakeAvailable(fmod)