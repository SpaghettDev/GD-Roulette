include(FetchContent)

FetchContent_Declare(
    RTRP
    GIT_REPOSITORY https://github.com/SpaghettDev/RTRP.git
    GIT_TAG        7d1a721893c117aa6ae3cc61d261013852b605ee
    GIT_PROGRESS TRUE
)
message("Fetching RTRP")
FetchContent_MakeAvailable(RTRP)

target_include_directories(${PROJECT_NAME} PRIVATE ${RTRP_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} RTRP)
