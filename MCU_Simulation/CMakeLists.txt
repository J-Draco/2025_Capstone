cmake_minimum_required(VERSION 3.24)
project(MCU_Simulation C)

set(CMAKE_C_STANDARD 11)

set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIBRARIES "${OPENSSL_ROOT_DIR}/lib")

include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})
add_executable(MCU_Simulation main.c
        sha256.c global_data.h openssl_en_de.h)
target_link_libraries(MCU_Simulation ssl crypto)
