cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)

set (PROJECT IqOrm)
set (VERSION "0.8.0")
string (TOLOWER ${PROJECT} TARGET)

project (${PROJECT})

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

find_package (Qt5Sql REQUIRED)
find_package (Qt5Quick REQUIRED)

file (GLOB SOURCES src/*.cpp)
source_group ("Source Files" FILES ${SOURCES})
file (GLOB HEADERS src/*.h)
source_group ("Header Files" FILES ${HEADERS})

add_definitions (-DIQORM_LIBRARY)
add_definitions (-DLIB_VERSION=${VERSION})
add_library (${TARGET} SHARED ${SOURCES})

target_link_libraries (${TARGET} Qt5::Sql Qt5::Quick)

set_target_properties (${TARGET} PROPERTIES
                       VERSION ${VERSION})

install (TARGETS ${TARGET}
         LIBRARY DESTINATION lib)

target_compile_features (${TARGET} PRIVATE cxx_range_for)

if (MSVC)
    add_definitions (/W4)
elseif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)
    add_definitions (-Wall -pedantic)
else ()
    message ("Unknown compiler")
endif ()
