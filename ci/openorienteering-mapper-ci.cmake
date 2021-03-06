# This file is part of OpenOrienteering.

# Copyright 2016-2020 Kai Pastor
#
# Redistribution and use is allowed according to the terms of the BSD license:
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products 
#    derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set(Mapper_CI_SOURCE_DIR "NOTFOUND" CACHE STRING "Mapper (CI): Source code directory")
set(Mapper_CI_VERSION_DISPLAY "ci" CACHE STRING "Mapper (CI): Version display string")
set(Mapper_CI_APP_ID "org.openorienteering.mapper.ci" CACHE STRING "Mapper (CI): Android App ID")
set(Mapper_CI_LICENSING_PROVIDER "OFF" CACHE STRING "Mapper (CI): Provider for 3rd-party licensing information")
set(Mapper_CI_QT_VERSION "5.12" CACHE STRING "Mapper (CI): Qt version")
option(Mapper_CI_ENABLE_COVERAGE "Mapper: Enable testing coverage analysis" OFF)
option(Mapper_CI_ENABLE_GDAL "Mapper: Enable GDAL" ON)
option(Mapper_CI_ENABLE_POSITIONING "Mapper: Enable positioning" ON)
option(Mapper_CI_MANUAL_PDF "Mapper (git): Provide the manual as PDF file (needs pdflatex)" OFF)
set(Mapper_CI_GDAL_DATA_DIR "NOTFOUND" CACHE STRING "Mapper (CI): GDAL data directory")

# Run the AutosaveTest, too, but not on macOS where it is flaky.
set(patch_command PATCH_COMMAND sed -e s/autosave_t.MANUAL/autosave_t/ -i -- test/CMakeLists.txt)
if(APPLE)
	set(patch_command )
endif()

add_custom_target(openorienteering-mapper-ci-source)
set_property(TARGET openorienteering-mapper-ci-source
  PROPERTY SB_SOURCE_DIR "${Mapper_CI_SOURCE_DIR}"
)

superbuild_package(
  NAME           openorienteering-mapper
  VERSION        ci
  DEPENDS
    gdal
    libpolyclipping
    proj
    qtandroidextras-${Mapper_CI_QT_VERSION}
    qtbase-${Mapper_CI_QT_VERSION}
    qtimageformats-${Mapper_CI_QT_VERSION}
    qtlocation-${Mapper_CI_QT_VERSION}
    qtsensors-${Mapper_CI_QT_VERSION}
    qttools-${Mapper_CI_QT_VERSION}
    qttranslations-${Mapper_CI_QT_VERSION}
    zlib
    host:doxygen
    host:qttools-${Mapper_CI_QT_VERSION}

  USING
    Mapper_CI_VERSION_DISPLAY
    Mapper_CI_APP_ID
    Mapper_CI_LICENSING_PROVIDER
    Mapper_CI_ENABLE_COVERAGE
    Mapper_CI_ENABLE_GDAL
    Mapper_CI_ENABLE_POSITIONING
    Mapper_CI_MANUAL_PDF
    Mapper_CI_GDAL_DATA_DIR

  BUILD [[
    CMAKE_ARGS
      "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
      "-UCMAKE_DISABLE*"
      "-UCMAKE_STAGING_PREFIX"
      "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
      "-DBUILD_SHARED_LIBS=0"
      "-DMapper_AUTORUN_SYSTEM_TESTS=0"
      "-DLICENSING_PROVIDER=${Mapper_CI_LICENSING_PROVIDER}"
      "-DMapper_VERSION_DISPLAY=${Mapper_CI_VERSION_DISPLAY}"
      "-DMapper_MANUAL_PDF=$<BOOL:@Mapper_CI_MANUAL_PDF@>"
      "-DMapper_USE_GDAL=$<BOOL:@Mapper_CI_ENABLE_GDAL@>"
    $<$<BOOL:@Mapper_CI_ENABLE_GDAL@>:
      "-DGDAL_DATA_DIR=${Mapper_CI_GDAL_DATA_DIR}"
    >
    $<$<BOOL:@ANDROID@>:
      "-DCMAKE_DISABLE_FIND_PACKAGE_Qt5PrintSupport=TRUE"
      "-DMAPPER_APP_ID=${Mapper_CI_APP_ID}"
      "-DKEYSTORE_URL=${KEYSTORE_URL}"
      "-DKEYSTORE_ALIAS=${KEYSTORE_ALIAS}"
      $<$<NOT:$<VERSION_LESS:@CMAKE_VERSION@,3.15.0>>:
        $<0: https://gitlab.kitware.com/cmake/cmake/issues/19515 >
        "-DCMAKE_C_COMPILER_FRONTEND_VARIANT=GNU"
        "-DCMAKE_CXX_COMPILER_FRONTEND_VARIANT=GNU"
      >
    >
    $<$<NOT:$<BOOL:@Mapper_CI_ENABLE_POSITIONING@>>:
      "-DCMAKE_DISABLE_FIND_PACKAGE_Qt5Positioning:BOOL=TRUE"
      "-UQt5Positioning_DIR"
      "-DCMAKE_DISABLE_FIND_PACKAGE_Qt5Sensors:BOOL=TRUE"
      "-UQt5Sensors_DIR"
    >
    $<$<BOOL:@CMAKE_CROSSCOMPILING@>:
      "-DCMAKE_PROGRAM_PATH=@HOST_DIR@/bin"
    >
    $<$<BOOL:@Mapper_CI_ENABLE_COVERAGE@>:
      "-DMapper_DEVELOPMENT_BUILD:BOOL=FALSE"
      "-DCMAKE_DISABLE_FIND_PACKAGE_ClangTidy:BOOL=TRUE"
      "-DCMAKE_DISABLE_FIND_PACKAGE_IWYU:BOOL=TRUE"
    >
    BUILD_ALWAYS 1
    INSTALL_COMMAND
      "${CMAKE_COMMAND}" --build . --target package$<IF:$<STREQUAL:@CMAKE_GENERATOR@,Ninja>,,/fast>
  $<$<NOT:$<BOOL:@CMAKE_CROSSCOMPILING@>>:
    TEST_COMMAND
      "${CMAKE_CTEST_COMMAND}" -T Test --no-compress-output
    $<$<BOOL:@Mapper_CI_ENABLE_COVERAGE@>:
        --exclude-regex symbol_set_t
    COMMAND
      "${CMAKE_CTEST_COMMAND}" -T Coverage
    >
    TEST_BEFORE_INSTALL 1
  >
  ]]
)
