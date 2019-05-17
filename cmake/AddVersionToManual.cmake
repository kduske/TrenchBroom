FIND_PACKAGE(Git)

INCLUDE("${CMAKE_SOURCE_DIR}/../cmake/Utils.cmake")
GET_GIT_DESCRIBE("${GIT_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/.." GIT_DESCRIBE)
GET_APP_VERSION(GIT_DESCRIBE APP_VERSION_YEAR APP_VERSION_NUMBER)
SET(APP_BUILD_TYPE "${CMAKE_BUILD_TYPE}")

SET(MANUAL_VERSION_PATTERN "__TB_VERSION__")

FILE(READ "${INPUT}" MANUAL_CONTENTS)
STRING(REGEX REPLACE "${MANUAL_VERSION_PATTERN}" "${APP_VERSION_YEAR}.${APP_VERSION_NUMBER}" MANUAL_CONTENTS "${MANUAL_CONTENTS}")
FILE(WRITE "${OUTPUT}" "${MANUAL_CONTENTS}")
