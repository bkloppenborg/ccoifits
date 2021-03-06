# Create a file containing version information
EXECUTE_PROCESS(
    COMMAND git describe --abbrev=0 --tags
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
EXECUTE_PROCESS(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# strip the leading 'v' off of the git version tag:
string(LENGTH ${GIT_VERSION_TAG} GIT_TAG_LENGTH)
string(SUBSTRING ${GIT_VERSION_TAG} 1 -1 GIT_VERSION_TAG)

CONFIGURE_FILE(
    ${CMAKE_SOURCE_DIR}/src/version.h.in
    ${CMAKE_SOURCE_DIR}/src/version.h
)
