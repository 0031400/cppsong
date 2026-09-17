cmake_minimum_required(VERSION 3.10)
project(one LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Boost CONFIG REQUIRED COMPONENTS system json program_options)
find_package(OpenSSL REQUIRED)
add_executable(one
    src/main.cpp
    src/tcp/tcpclient.cpp
    src/udp/udpclient.cpp
    src/transports/tcptransport.cpp
    src/connections/tcpconnection.cpp
    src/connections/wsconnection.cpp
    src/listeners/listener.cpp
    src/listeners/tcplistener.cpp
    src/listeners/tlslistener.cpp
    src/listeners/wslistener.cpp
    src/listeners/wsslistener.cpp
    src/utils/bytes.cpp
    src/utils/log.cpp
    src/dns/message.cpp
    src/dns/dns.cpp
)

target_include_directories(one PRIVATE include src)
target_compile_definitions(one PRIVATE BOOST_ASIO_NO_DEPRECATED)
target_link_libraries(
    one PRIVATE Boost::system OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
    target_compile_definitions(one PRIVATE
        _WIN32_WINNT=0x0A00
    )
endif()
# if(MSVC)
#     target_compile_options(one PRIVATE /bigobj)
# endif()
