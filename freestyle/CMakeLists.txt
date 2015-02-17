set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h)
FILE(GLOB folder_shader  shaders/*.* shaders/*/*.*)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})
SOURCE_GROUP("Shader Files" FILES ${folder_shader})

QT5_WRAP_CPP(freestyle_MOC_SRCS ${freestyle_MOC_HDRS})
QT5_WRAP_UI(freestyle_UIS_HDRS mainwindow.ui parametersdialog.ui toolsdialog.ui)
include_directories(
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_SOURCE_DIR}/freestyle/engine
   ${CMAKE_SOURCE_DIR}/freestyle/sculptor
   ${GLM_INC}
   ${ASSIMP_INC}
   ${OPENMESH_INC}
   ${CMAKE_SOURCE_DIR}/freestyle/freestyle
)


add_executable(freestyle ${folder_source} ${folder_header} ${freestyle_RCCS} ${freestyle_UIS_HDRS} ${folder_shader})
target_link_libraries(freestyle ${OPENGL_LIBRARIES} ${ASSIMP_LIB} ${OPENMESH_LIB} Qt5::Gui Qt5::Widgets Qt5::OpenGL vortexengine sculptor)

