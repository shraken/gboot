project( gflash )
cmake_minimum_required(VERSION 2.8)

# header files
set (MAIN_INCLUDE ${PROJECT_SOURCE_DIR}) 
set (HIDAPI_INCLUDE ${PROJECT_SOURCE_DIR}/../common/hidapi/hidapi)
set (GBOOT_INCLUDE ${PROJECT_SOURCE_DIR}/..)
set (LIBUSB_INCLUDE /usr/include/libusb-1.0/)

set (GFLASH_INCLUDE_DIRS ${MAIN_INCLUDE} ${HIDAPI_INCLUDE} ${GBOOT_INCLUDE} ${LIBUSB_INCLUDE})

# source files

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(HIDAPI_SOURCES ${PROJECT_SOURCE_DIR}/../common/hidapi/windows/hid.c)
    set(EXTRA_LIBS ${IOKIT_LIBRARY} setupapi.lib)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(HIDAPI_SOURCES ${PROJECT_SOURCE_DIR}/../common/hidapi/libusb/hid.c)
    set(EXTRA_LIBS ${IOKIT_LIBRARY} udev usb-1.0 pthread)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(HIDAPI_SOURCES ${PROJECT_SOURCE_DIR}/../common/hidapi/mac/hid.c)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
    mark_as_advanced(IOKIT_LIBRARY CORE_FOUNDATION_LIBRARY)
    set(EXTRA_LIBS ${IOKIT_LIBRARY} ${CORE_FOUNDATION_LIBRARY})
else ()
    message( FATAL_ERROR "The host build platform is not supported." )
endif()

set (GBOOT_SOURCES ${LIBBUDDY_SOURCES} ${HIDAPI_SOURCES})

include_directories(${GFLASH_INCLUDE_DIRS})
add_executable (gflash gflash_cli.c gflash_lib.c ${GBOOT_SOURCES})
target_link_libraries(gflash ${EXTRA_LIBS})