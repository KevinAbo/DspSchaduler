INCLUDE_DIRECTORIES( . .. /opt/baselib/include)
LINK_DIRECTORIES(/opt/baselib/lib)

IF (DEFINITIONS_FLAGS)
    ADD_DEFINITIONS(${DEFINITIONS_FLAGS})
ELSE (DEFINITIONS_FLAGS)
    ADD_DEFINITIONS(-g -std=c++11 -O2 -W -Wall -D_FILE_OFFSET_BITS=64 -D_TALUS_PROJECT_NAME="${PROJECT_TITLE}" -D_TALUS_TAG="${TAG}" -D_TALUS_REVISION="${REVISION}")
ENDIF (DEFINITIONS_FLAGS)

LINK_LIBRARIES(talus talco event dl pthread rt config memcached config++ crypto z mcrypt)

ADD_EXECUTABLE(dsp_call
    main.cpp
	DSPScheduler.h
	http_client.h
	track_data_notify.h
)

INSTALL(TARGETS dsp_call RUNTIME DESTINATION bin COMPONENT binaries)