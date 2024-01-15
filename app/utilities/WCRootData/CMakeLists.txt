
include_directories(BEFORE
        ${PROJECT_SOURCE_DIR}/app/utilities/WCRootData/include
    )

# Don't include Utilities
#file(GLOB_RECURSE SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
#file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

set (SRC 
        src/BeamTiming.cc
        src/SpillAnalyzer.cc
        src/WCRootData.cc
        src/WCRootDataBeamBkg.cc
        src/WCRootDataIDNuInt.cc
        src/WCRootDataNuInt.cc
        src/WCRootDataPileUpSpill.cc
        )
set (HEADERS 
        include/BeamTiming.h
        include/SpillAnalyzer.h
        include/WCRootData.h
        include/WCRootDataBeamBkg.h
        include/WCRootDataIDNuInt.h
        include/WCRootDataNuInt.h
        include/WCRootDataPileUpSpill.h
        )

pbuilder_library(
        TARGET WCRootData
        SOURCES ${SRC}
        PROJECT_LIBRARIES MDT
        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)

pbuilder_install_headers(${HEADERS})
