project(IsoSurface)
set(APP IsoSurface)

cmake_minimum_required(VERSION 2.8)

set(OF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../of/" CACHE PATH "The root directory of ofnode/of project.")
include(${OF_ROOT}/openFrameworks.cmake)

set(VTK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../VTK/build")
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

#ofxaddon()

option(COTIRE "Use cotire" OFF)

set(SOURCES
    src/main.cpp
    src/ofApp.cpp
    src/ImageReader.cpp
)

add_executable(
    ${APP}
    MACOSX_BUNDLE
    ${SOURCES}
    ${OFXADDONS_SOURCES}
)

target_link_libraries(
    ${APP}
    ${OPENFRAMEWORKS_LIBRARIES}
    ${VTK_LIBRARIES}
)

set_target_properties(
    ${APP} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST
    "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set_target_properties( ${APP} PROPERTIES OUTPUT_NAME "${APP}-Debug")
endif()

if (CMAKE_CROSSCOMPILING)
    set_target_properties( ${APP} PROPERTIES OUTPUT_NAME
      "${APP}-${OF_PLATFORM}-${CMAKE_BUILD_TYPE}")
endif()

if (COTIRE)
    cotire(${APP})
endif()
