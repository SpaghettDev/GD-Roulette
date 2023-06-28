include(FetchContent)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG        0f25a2449b3cf878bcbdbf91b693c38149ecf029
    GIT_PROGRESS TRUE
)
message("Fetching MinHook")
FetchContent_MakeAvailable(minhook)
set_property(TARGET minhook PROPERTY CXX_STANDARD 23)