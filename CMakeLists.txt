cmake_minimum_required(VERSION 3.17)

include(cmake/main.cmake)

# pcap linking
add_library(wpcap IMPORTED SHARED )
add_library(Packet IMPORTED SHARED)
link_libraries(ws2_32 wsock32)
set_property(TARGET wpcap PROPERTY IMPORTED_LOCATION "C:/Windows/System32/Npcap/wpcap.dll")
set_property(TARGET Packet PROPERTY IMPORTED_LOCATION "C:/Windows/System32/Npcap/Packet.dll")

project(NastyTraffic
        VERSION 0.0.1
        DESCRIPTION "nasty-sniffer"
        HOMEPAGE_URL ""
        LANGUAGES CXX)

find_package(GTest CONFIG REQUIRED)

find_package(PkgConfig)

pkg_check_modules(libpcap REQUIRED IMPORTED_TARGET libpcap)

set(CMAKE_CXX_STANDARD 14)
# Qt adding
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:/msys64/mingw64/")
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


initialize_project()
