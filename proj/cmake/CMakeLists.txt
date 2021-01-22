cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

message("CMAKE_CXX_STANDARD=${SET17}")
if( SET17 ) 
set(CMAKE_CXX_STANDARD 17)
endif()


set(CMAKE_CXX_STANDARD 17)

project( Framed )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Cinder" ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )


set( SRC_FILES
		${APP_PATH}/src/FramedApp.cpp
    	${APP_PATH}/src/Helpers/GlobalSettings.cpp
    	${APP_PATH}/src/Helpers/GlobalSettings.h
		${APP_PATH}/src/UI/Slider.cpp
		${APP_PATH}/src/UI/ColorSlider.cpp
		${APP_PATH}/src/UI/TouchButton.cpp
		${APP_PATH}/src/UI/TouchUI.cpp
		${APP_PATH}/src/Helpers/LineManager.cpp
		${APP_PATH}/src/Helpers/BrushManager.cpp
		${APP_PATH}/src/Helpers/FrameManager.cpp
        ${APP_PATH}/src/Helpers/Frame.cpp
        ${APP_PATH}/src/Pen/OSX/ofxTablet.cpp
        ${APP_PATH}/src/Pen/OSX/tabletCocoa.mm

		)


#source_group(Helpers FILES ${APP_PATH}/src/Helpers/GlobalSettings.cpp  ${APP_PATH}/src/Helpers/GlobalSettings.h)


set( BLOCKSLIST
	OSC
	TUIO
    Base
	poScene
)

ci_make_app(
	SOURCES     ${SRC_FILES}
	CINDER_PATH ${CINDER_PATH}
	BLOCKS      ${BLOCKSLIST}
)
