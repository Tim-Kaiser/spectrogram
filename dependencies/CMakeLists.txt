include(FetchContent)

Set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(rreav
    GIT_REPOSITORY https://github.com/Tim-Kaiser/RREAV.git
    GIT_TAG 1.5
    GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(rreav)
