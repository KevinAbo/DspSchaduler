cmake_minimum_required(VERSION 2.6)

PROJECT(dsp_call)
SET(CMAKE_AR ar)


FIND_PATH(LIBEVENT_LIB libevent.a /usr/local/lib /usr/lib /usr/lib64 /opt/local/lib /app/dev/common/libevent-1.3e/lib)
if(NOT LIBEVENT_LIB)
  FIND_PATH(LIBEVENT_LIB libevent.so /usr/local/lib /usr/lib /usr/lib64 /opt/local/lib)
endif(NOT LIBEVENT_LIB)
LINK_LIBRARIES(rt pthread)

IF (NOT PROJECT_TITLE)
    SET(PROJECT_TITLE "dsp_call")
ENDIF (NOT PROJECT_TITLE)

IF (PREFIX)
    SET(CMAKE_INSTALL_PREFIX ${PREFIX})
ELSE (PREFIX)
    SET(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/product_output")
ENDIF (PREFIX)

SET(DEFINITIONS_FLAGS -O2 -g -std=c++11 -W -Wall -Wextra
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL 
 -D_FILE_OFFSET_BITS=64 -D_TALUS_PROJECT_NAME="${PROJECT_TITLE}" -D_TALUS_TAG="${TAG}" -D_TALUS_REVISION="${REVISION}"
 )

SUBDIRS(
	src
)

INSTALL(DIRECTORY DESTINATION bin)
INSTALL(DIRECTORY DESTINATION conf)
INSTALL(DIRECTORY DESTINATION meta)
INSTALL(DIRECTORY DESTINATION log)
INSTALL(DIRECTORY DESTINATION run)