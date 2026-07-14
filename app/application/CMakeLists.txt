
#################
#Response library
#################

set (SRC
        PMTResponse3inchR12199_02.cc
)

set (HEADERS
        PMTResponse3inchR12199_02.h
)


pbuilder_library(
        TARGET PMTResponse
        SOURCES ${SRC}
        PROJECT_LIBRARIES MDT
        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)

pbuilder_install_headers(${HEADERS})

#################
#Executables
#################
set(exe_sources
        appIWCDSingleEvent.cc
        appWCTESingleEvent.cc
        appGenPileUpSpill.cc
)

pbuilder_executables(
        SOURCES ${exe_sources}
        TARGETS_VAR programs
        PROJECT_LIBRARIES WCRootData MDT PMTResponse
        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)

#########
# target exportation
#########

pbuilder_component_install_and_export(
        COMPONENT Executable Libraries
        LIBTARGETS PMTResponse
        EXETARGETS ${programs}
)