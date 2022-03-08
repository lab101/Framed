cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

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
        ${APP_PATH}/src/Helpers/OverlayManager.cpp
        ${APP_PATH}/src/Helpers/TemplateManager.cpp
        ${APP_PATH}/src/Helpers/Frame.cpp
		${APP_PATH}/src/Helpers/NetworkManager.cpp
        ${APP_PATH}/src/VO/PointsPackage.cpp
        ${APP_PATH}/src/UI/ColorPicker.cpp
        ${APP_PATH}/src/UI/ScrollBox.cpp
		)


if(APPLE)
  list(APPEND SRC_FILES ${APP_PATH}/src/Pen/OSX/ofxTablet.cpp)
  list(APPEND SRC_FILES ${APP_PATH}/src/Pen/OSX/tabletCocoa.mm)
endif()


set( BLOCKSLIST
	CinderSpout
    Cinder-Syphon
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
if(WIN32)
	target_link_libraries(Framed Spout.dll)
endif()

add_custom_command( TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${APP_PATH}../../../../Cinder/blocks/Cinder-Syphon/lib/Syphon.framework
    ${CMAKE_BUILD_TYPE}/${APP_NAME}/${APP_NAME}.app/Contents/Frameworks/Syphon.framework
)
