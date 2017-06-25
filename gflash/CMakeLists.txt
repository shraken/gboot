project( gflash )
cmake_minimum_required(VERSION 2.8)

# header files
set (MAIN_INCLUDE ${PROJECT_SOURCE_DIR}) 
set (LIBUSB_INCLUDE ${PROJECT_SOURCE_DIR}/../common/libusb-1.0.21/include/libusb-1.0) 
set (GBOOT_INCLUDE ${PROJECT_SOURCE_DIR}/..)

set (GFLASH_INCLUDE_DIRS ${MAIN_INCLUDE} ${LIBUSB_INCLUDE} ${GBOOT_INCLUDE})

# source files

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(EXTRA_LIBS setupapi ole32 advapi32 libusb-1.0)

    set (LIBUSB_LIB_PATH ${PROJECT_SOURCE_DIR}/../common/libusb-1.0.21/MS32/static)
    link_directories(${LIBUSB_LIB_PATH})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(EXTRA_LIBS usb-1.0)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    #find_library(IOKIT_LIBRARY IOKit)
    #find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
    #mark_as_advanced(IOKIT_LIBRARY CORE_FOUNDATION_LIBRARY)
    set(EXTRA_LIBS ${IOKIT_LIBRARY} ${CORE_FOUNDATION_LIBRARY})
else ()
    message( FATAL_ERROR "The host build platform is not supported." )
endif()

# debug: print variables to screen
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

include_directories(${GFLASH_INCLUDE_DIRS})
add_executable (gflash gflash_cli.c gflash_lib.c)
target_link_libraries(gflash ${EXTRA_LIBS})