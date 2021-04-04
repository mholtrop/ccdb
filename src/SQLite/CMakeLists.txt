cmake_minimum_required(VERSION 3.3)
project(ccdb_sqlite)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package (Threads REQUIRED)

include_directories("../../include/SQLite")

set(SOURCE_FILES "sqlite3.c")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})


target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ccdb>
                           $<INSTALL_INTERFACE:include/ccdb>
                           $<INSTALL_INTERFACE:include/ccdb/SQLite>
                           )


# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
        PROPERTIES POSITION_INDEPENDENT_CODE ON)

#set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
#        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

# Install the library, but add it to the exports of ccdb.
install(TARGETS ${PROJECT_NAME}
        EXPORT ccdb-exports
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
#
