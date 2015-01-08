SET( LIBRARY_VERSION "0.0.1" )
SET( LIBRARY_SOVERSION "0.0.1" )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

find_package(Qt5Widgets REQUIRED)

include_directories(
   ${OPENMESH_INC}
)

ADD_LIBRARY( sculptor SHARED
        ${folder_source}
        ${folder_header}
)

SET_TARGET_PROPERTIES( sculptor PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${LIBRARY_SOVERSION}
)

TARGET_LINK_LIBRARIES( sculptor)

INSTALL(TARGETS sculptor DESTINATION ${LIB_INSTALL_DIR} )
