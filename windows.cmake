set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/build/vsstudio/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/build/vsstudio/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/build/vsstudio/")

set(CMAKE_CXX_FLAGS_RELEASE			"${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG			"${CMAKE_CXX_FLAGS_DEBUG} /MTd")
set(CMAKE_EXE_LINKER_FLAGS			"${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
set(CMAKE_CXX_STANDARD 17)

set(BGFX_LIB_FOLDER					"${CMAKE_SOURCE_DIR}/libs/bgfx/.build/win64_vs2017/bin/")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(BGFX_LIBRARY					"${BGFX_LIB_FOLDER}/bgfxDebug.lib")
set(BX_LIBRARY						"${BGFX_LIB_FOLDER}/bxDebug.lib")
set(BIMG_LIBRARY					"${BGFX_LIB_FOLDER}/bimgDebug.lib")
set(BIMG_DECODE_LIBRARY				"${BGFX_LIB_FOLDER}/bimg_decodeDebug.lib")

set(SDL2_INCLUDE_DIRS				"${LIB_FOLDER}/SDL2/SDL2-2.0.10/include")
set(SDL2_IMAGE_INCLUDE_DIR			"${LIB_FOLDER}/SDL2/SDL2_image-2.0.5/include")
set(SDL2_LIBRARY					"${LIB_FOLDER}/SDL2/SDL2-2.0.10/lib/x64/SDL2.lib")
set(SDL2_IMAGE_LIBRARIES			"${LIB_FOLDER}/SDL2/SDL2_image-2.0.5/lib/x64/SDL2_image.lib")
set(C_RUNTIME						"libucrtd.lib")



include_directories(libs/bx/include/compat/msvc)

add_executable(RadRTS ${SOURCES})
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
set_property(TARGET RadRTS PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/vsstudio/Debug")

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCES})

foreach(FILE SOURCES) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "Source Files${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
       set(GROUP "Header Files${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

include_directories(${SDL2_INCLUDE_DIRS}
					${SDL2_IMAGE_INCLUDE_DIR})

target_link_libraries(RadRTS 
    ${C_RUNTIME}
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
    ${BIMG_DECODE_LIBRARY}
        
    ${BGFX_LIBRARY}
    ${BIMG_LIBRARY}
    ${BX_LIBRARY}
    
)