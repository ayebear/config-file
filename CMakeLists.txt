cmake_minimum_required (VERSION 3.1.3)
project (ConfigFile)

set (CFG_SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/configfile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/configoption.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/strlib.cpp
)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /Ox")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -O3")
endif(MSVC)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
add_library (cfgfile_s STATIC ${CFG_SOURCE})

set_property(TARGET cfgfile_s PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET cfgfile_s PROPERTY CXX_STANDARD 14)
