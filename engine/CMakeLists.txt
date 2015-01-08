SET( LIBRARY_VERSION "0.2.0" )
SET( LIBRARY_SOVERSION "0.1" )

SET( HEADER_PATH ./ )
FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header ${HEADER_PATH}/*.h)

find_package(OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)

include_directories(
   ${CMAKE_SOURCE_DIR}/src/engine
   ${GLM_INC}
   ${ASSIMP_INC}
   ${HEADER_PATH}
)

set(LIBS Qt5::Widgets Qt5::OpenGL Qt5::Gui ${OPENGL_LIBRARIES} ${ASSIMP_LIB})

ADD_LIBRARY( vortexengine SHARED
        ${folder_source}
        ${folder_header}
)

SET_TARGET_PROPERTIES( vortexengine PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${LIBRARY_SOVERSION}
)

TARGET_LINK_LIBRARIES( vortexengine ${LIBS} )
INSTALL(TARGETS vortexengine DESTINATION ${LIB_INSTALL_DIR} )
