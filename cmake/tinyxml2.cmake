SET(LIB_TINYXML2_SOURCE_DIR "${LIB_SOURCE_DIR}/tinyxml2")
SET(LIB_TINYXML2_SOURCE "${LIB_TINYXML2_SOURCE_DIR}/tinyxml2.cpp")
SET_SOURCE_FILES_PROPERTIES(${LIB_TINYXML2_SOURCE} PROPERTIES COMPILE_FLAGS "-w")
ADD_DEFINITIONS(-DTINYXML2_LANG_CXX11=1)

ADD_LIBRARY(tinyxml2 ${LIB_TINYXML2_SOURCE} ${LIB_INCLUDE_DIR})

