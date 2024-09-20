include(FetchContent)

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

FetchContent_Declare(gcem
    GIT_REPOSITORY https://github.com/kthohr/gcem.git
    GIT_TAG        v1.18.0
)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)

FetchContent_MakeAvailable(json gcem glm)
