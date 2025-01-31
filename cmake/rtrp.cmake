include(FetchContent)

FetchContent_Declare(
    RTRP
    GIT_REPOSITORY https://github.com/SpaghettDev/RTRP.git
    GIT_TAG        ef10e5fe9b6988cba2a68aae1a1c520ce205ac2c
    GIT_PROGRESS TRUE
)
message("Fetching RTRP")
FetchContent_MakeAvailable(RTRP)

target_include_directories(${PROJECT_NAME} PRIVATE ${RTRP_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} RTRP)
