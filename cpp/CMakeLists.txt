
set (SRC
        src/Configuration.cc
        src/HitDigitizer.cc
        src/HitTube.cc
        src/HitTubeCollection.cc
        src/MDTManager.cc
        src/MTRandom.cc
        src/PMTAfterpulse.cc
        src/PMTNoise.cc
        src/PMTResponse.cc
        src/TriggerAlgo.cc
        src/TriggerInfo.cc
        src/TrueHit.cc
        )
set (HEADERS 
        include/Configuration.h
        include/HitDigitizer.h
        include/HitTube.h
        include/HitTubeCollection.h
        include/MDTManager.h
        include/MTRandom.h
        include/PMTAfterpulse.h
        include/PMTNoise.h
        include/PMTResponse.h
        include/TriggerAlgo.h
        include/TriggerInfo.h
        include/TrueHit.h
        )

pbuilder_library(
        TARGET MDT
        SOURCES ${SRC}
        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)


pbuilder_component_install_and_export(
        COMPONENT Library
        LIBTARGETS MDT
)

pbuilder_install_headers(${HEADERS})
