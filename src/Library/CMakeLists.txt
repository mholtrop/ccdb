cmake_minimum_required(VERSION 3.3)
project(CCDB_lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CCDB_INCLUDE ../../include/CCDB)

include_directories("../../include")
include_directories("../../include/SQLite")
include_directories(${MYSQL_INCLUDE_DIR})

set(SOURCE_FILES

         # GLOBAL objects

        "${CCDB_INCLUDE}/Log.h"
        "Log.cc"

        "${CCDB_INCLUDE}/CCDBError.h"
        "CCDBError.cc"

        #user api
        "${CCDB_INCLUDE}/Calibration.h"
        "Calibration.cc"

        "${CCDB_INCLUDE}/CalibrationGenerator.h"
        "CalibrationGenerator.cc"

        "${CCDB_INCLUDE}/SQLiteCalibration.h"
        "SQLiteCalibration.cc"

        "${CCDB_INCLUDE}/MySQLCalibration.h"
        "MySQLCalibration.cc"

        # HELPERS
        "${CCDB_INCLUDE}/Helpers/StringUtils.h"
        "Helpers/StringUtils.cc"

        "${CCDB_INCLUDE}/Helpers/PathUtils.h"
        "Helpers/PathUtils.cc"

        "${CCDB_INCLUDE}/Helpers/WorkUtils.h"
        "Helpers/WorkUtils.cc"

        "${CCDB_INCLUDE}/Helpers/TimeProvider.h"
        "Helpers/TimeProvider.cc"

        # MODEL

        "${CCDB_INCLUDE}/Model/Assignment.h"
        "Model/Assignment.cc"

        "${CCDB_INCLUDE}/Model/ConstantsTypeColumn.h"
        "Model/ConstantsTypeColumn.cc"

        "${CCDB_INCLUDE}/Model/ConstantsTypeTable.h"
        "Model/ConstantsTypeTable.cc"

        "${CCDB_INCLUDE}/Model/Directory.h"
        "Model/Directory.cc"

        "${CCDB_INCLUDE}/Model/RunRange.h"
        "Model/RunRange.cc"

        "${CCDB_INCLUDE}/Model/Variation.h"
        "Model/Variation.cc"

        # PROVIDERS
        "${CCDB_INCLUDE}/Providers/DataProvider.h"
        "Providers/DataProvider.cc"

        "${CCDB_INCLUDE}/Providers/FileDataProvider.h"
        "Providers/FileDataProvider.cc"

        "${CCDB_INCLUDE}/Providers/SQLiteDataProvider.h"
        "Providers/SQLiteDataProvider.cc"

        "${CCDB_INCLUDE}/Providers/SQLiteDataProvider.h"
        "Providers/SQLiteDataProvider.cc"

        "${CCDB_INCLUDE}/Providers/MySQLDataProvider.h"
        "Providers/MySQLDataProvider.cc"

        "${CCDB_INCLUDE}/Providers/MySQLConnectionInfo.h"
        "Providers/MySQLConnectionInfo.cc"


        #user api



        #for clion convenience
        ../../include/CCDB/Helpers/StopWatch.h
        )


add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${MYSQL_LIB}  CCDB_sqlite)


# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
        PROPERTIES POSITION_INDEPENDENT_CODE ON)


SET(GCC_COVERAGE_COMPILE_FLAGS "-I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing    -g -DNDEBUG")
SET(GCC_COVERAGE_LINK_FLAGS    "-L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )