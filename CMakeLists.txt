cmake_minimum_required(VERSION 3.5.1)
project(RadRts)

set_property(GLOBAL PROPERTY USE_FOLDERS On)
set(LIB_FOLDER "${CMAKE_SOURCE_DIR}/libs")

# Put source files in this glob
file(GLOB SOURCES 
"src/*.cpp" 
"src/*.hpp" 
"src/*/*.cpp"
"src/*/*.hpp"
"libs/bgfx/3rdparty/meshoptimizer/src/*.cpp"
"libs/bgfx/3rdparty/dear-imgui/*.cpp"
"libs/bgfx/examples/common/imgui/imgui.cpp" 
)

#file(GLOB IMGUI_SRCS 
#)

#add_library(IMGUI_LIBRARY STATIC
#${IMGUI_SRCS}
#)

# LIBS
include_directories(src)
include_directories(libs/bx/include)
include_directories(libs/bgfx/include)
include_directories(libs/bimg/include)
include_directories(libs/bgfx/include)
include_directories(libs/bgfx/3rdparty/meshoptimizer/src)
include_directories(libs/bgfx/3rdparty)
include_directories(libs/bgfx/examples/common/)

if (UNIX OR EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

if (WIN32)
	include("${CMAKE_CURRENT_SOURCE_DIR}/windows.cmake")
endif()

if (NOT WIN32)
    add_executable(RadRTS ${SOURCES})
endif()

if (UNIX AND NOT EMSCRIPTEN)
    set(BGFX_LIB_FOLDER "${CMAKE_SOURCE_DIR}/libs/bgfx/.build/linux64_gcc/bin/")
endif()

if(EMSCRIPTEN)
    set(BGFX_LIB_FOLDER "${CMAKE_SOURCE_DIR}/libs/bgfx/.build/asmjs/bin/")
endif()

if (UNIX OR EMSCRIPTEN)
    set(BGFX_LIBRARY         "${BGFX_LIB_FOLDER}/libbgfxDebug.a")
    set(BX_LIBRARY           "${BGFX_LIB_FOLDER}/libbxDebug.a")
    set(BIMG_LIBRARY         "${BGFX_LIB_FOLDER}/libbimgDebug.a")
    set(BIMG_DECODE_LIBRARY  "${BGFX_LIB_FOLDER}/libbimg_decodeDebug.a")
    set(BIMG_ENCODE_LIBRARY  "${BGFX_LIB_FOLDER}/libbimg_encodeDebug.a")
endif()

if (UNIX AND NOT EMSCRIPTEN)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

	find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
	include_directories(${SDL2_INCLUDE_DIRS}
						${SDL2_IMAGE_INCLUDE_DIR})

    target_link_libraries(RadRTS 
        ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARIES}
        ${BIMG_ENCODE_LIBRARY}
        ${BIMG_DECODE_LIBRARY}
        
        ${BGFX_LIBRARY}
        ${BIMG_LIBRARY}
        ${BX_LIBRARY}
        ${CMAKE_DL_LIBS}
        GL
        X11
        pthread
        )
endif()

# Runs if building for Emscripten (web)
# Excerpt from https://discourse.libsdl.org/t/sdl2-desktop-bridge-emscripten-questions/22632/2
if(EMSCRIPTEN)
    # Make emscripten output a html page instead of just the javascript (for easier testing).
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # This should make it easier to debug runtime problems on the browser.
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASSERTIONS=2 -s DEMANGLE_SUPPORT=1")
    endif()

    # SDL2 and SDL_image. Also using OPENGL_ES 2 emulation.
    # See https://kripken.github.io/emscripten-site/docs/porting/multimedia_and_graphics/OpenGL-support.html for more options
    # I haven't fiddled around with this for a while and I'm not sure if all of this is actually necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s USE_SDL_IMAGE=2")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_SDL=2 -s USE_SDL_IMAGE=2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_SDL=2 -s USE_SDL_IMAGE=2")

    #
    # Embedding asset files etc. to the javascript output:
    #

    #  "For SDL2_image in order to be useful, you need to specify the image formats you are planning on using with -s SDL2_IMAGE_FORMATS='["png"]'. This will also ensure that IMG_Init works properly. Alternatively, you can specify 'emcc --use-preload-plugins' but then you calls to IMG_Init will fail."
    # See https://github.com/kripken/emscripten/pull/3831 fro more info
    #
    # Had some issues with this not sure if it works yet. see https://github.com/emscripten-ports/SDL2_image/issues/4
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s SDL2_IMAGE_FORMATS='[\"png\"]'")
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --use-preload-plugins")

    # This maps a dir called data in my build hierarchy to a virtual dir called data embedded in the final javascript output.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file assets")

    # Another option you can try if having trouble with loading images:
    # https://groups.google.com/forum/#!topic/emscripten-discuss/0qxtF7zyG0s
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s STB_IMAGE=1")

    # Might need to play around with this if emscripten gives errors saying your program is too big.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s TOTAL_MEMORY=67108864")

    target_link_libraries(RadRTS
        ${BGFX_LIBRARY}
        ${BX_LIBRARY}
        ${BIMG_LIBRARY})
endif()

# Setup shader and mesh folders
install(DIRECTORY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/build/assets/")
install(DIRECTORY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/build/assets/shaders")
install(DIRECTORY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/build/assets/meshes")

# Copy over assets
add_custom_command(TARGET RadRTS PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
    COMMENT "copying over assets"
VERBATIM)