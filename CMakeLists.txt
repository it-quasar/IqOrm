cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)

set (PROJECT IqOrm)
set (VERSION "0.8.0")

if (UNIX)
    string (TOLOWER ${PROJECT} TARGET)
else ()
    set (TARGET ${PROJECT})
endif ()

#option (IqOrm_DEBUG_MODE_ENABLED "Enable IrOrm debug mode" OFF)

set (IqOrm_FOUND ON CACHE BOOL "True if headers and requested libraries were found")
set (IqOrm_VERSION ${VERSION} CACHE STRING "IqOrm version")
set (IqOrm_LIBRARIES ${TARGET} CACHE STRING "IqOrm component libraries to be linked")

project (${PROJECT})

#Qt definitions
set (CMAKE_AUTOMOC ON)

find_package (Qt5Sql REQUIRED)
find_package (Qt5Quick REQUIRED)

#Find sources
file (GLOB_RECURSE SOURCES src/*.cpp)
source_group ("Source Files" FILES ${SOURCES})
file (GLOB_RECURSE HEADERS src/*.h)
source_group ("Header Files" FILES ${HEADERS})

#Find include dirs
set (IqOrm_INCLUDE_DIRS "")
foreach (HEADER_FILE ${HEADERS})
    get_filename_component(DIR ${HEADER_FILE} PATH)
    list (APPEND IqOrm_INCLUDE_DIRS ${DIR})
endforeach()
list(REMOVE_DUPLICATES IqOrm_INCLUDE_DIRS)

include_directories(${IqOrm_INCLUDE_DIRS})
set (IqOrm_INCLUDE_DIRS ${IqOrm_INCLUDE_DIRS} CACHE STRING "IqOrm include directories")

#Set difinitions
add_definitions (-DIQORM_LIBRARY)
add_definitions (-DLIB_VERSION=${VERSION})

#Set Windows res files compiler
set (VERSIONINFO_RES_FILE "")
if(MINGW)
    file (GLOB VERSIONINFO_RES_FILE . src/versioninfo.rc)
    SET (CMAKE_RC_COMPILE_OBJECT
            "<CMAKE_RC_COMPILER> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

add_library (${TARGET} SHARED ${SOURCES} ${VERSIONINFO_RES_FILE})

target_link_libraries (${TARGET} Qt5::Sql Qt5::Quick)

set_target_properties (${TARGET} PROPERTIES
                       VERSION ${VERSION})

#Set C++11
if (MINGW)
    add_definitions(-std=c++11)
else ()
    target_compile_features (${TARGET} PRIVATE cxx_range_for)
endif ()

#Set all warnings enabled
if (MSVC)
    add_definitions (/W4)
elseif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -pedantic)
else ()
    message ("Unknown compiler")
endif ()

#Enable debug mode
if (IqOrm_DEBUG_MODE)
    add_definitions (-DIQORM_DEBUG_MODE)
endif ()
