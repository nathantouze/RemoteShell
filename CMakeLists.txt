project(RemoteShell)
cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(EXISTS cmake-build-debug/conanbuildinfo.cmake) #Clion, with conanbuildinfo.cmake in root
    include(build/conanbuildinfo.cmake)
else()
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake) # Not CLion
endif()
conan_basic_setup()

set(SRC_COMMON ./common/src)
set(SRC_SERV ./server/src)
set(SRC_SERV_NETWORK ./server/Network)
set(SRC_CLIENT ./client/src)
set(SRC_CLIENT_NETWORK ./client/Network)
set(INCLUDE_FWN ./common/FWNetwork)

add_executable(RemoteShell_Server)
add_executable(RemoteShell_Client)

target_sources(RemoteShell_Server PUBLIC
    ${SRC_SERV_NETWORK}/src/NetworkServer.cpp
    ${SRC_SERV}/CoreServer.cpp
    ${SRC_SERV}/main.cpp
    ${SRC_COMMON}/FPSManager.cpp
)

target_include_directories(RemoteShell_Server PUBLIC
    ./server/include
    ./server/Network/include
    ./common/include
    ${INCLUDE_FWN}/AsioTCP
    ${INCLUDE_FWN}/AsioUDP
    ${INCLUDE_FWN}/common
)

target_sources(RemoteShell_Client PUBLIC
    ${SRC_CLIENT}/main.cpp
    ${SRC_CLIENT}/CLI.cpp
    ${SRC_CLIENT}/ClientCore.cpp
    ${SRC_CLIENT_NETWORK}/src/Network.cpp
    ./common/src/FPSManager.cpp
)

target_include_directories(RemoteShell_Client PUBLIC
    ./client/include
    ./client/Network/include
    ./common/include
    ${INCLUDE_FWN}/AsioTCP
    ${INCLUDE_FWN}/AsioUDP
    ${INCLUDE_FWN}/common
)


target_compile_options(RemoteShell_Server PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /MD -D_WIN32_WINNT=0x0A00 /D_ITERATOR_DEBUG_LEVEL=0 /permissive- /TP /std:c++17>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>: -Wall -Werror -fPIC -ldl -pthread -O3 -g>
)

target_compile_options(RemoteShell_Client PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /MD -D_WIN32_WINNT=0x0A00 /D_ITERATOR_DEBUG_LEVEL=0 /permissive- /TP /std:c++17>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>: -Wall -fPIC -ldl -pthread -O3 -g>
)

target_link_libraries(RemoteShell_Server ${CONAN_LIBS} ${CMAKE_DL_LIBS})
target_link_libraries(RemoteShell_Client ${CONAN_LIBS} ${CMAKE_DL_LIBS})