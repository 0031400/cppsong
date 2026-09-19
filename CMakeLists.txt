cmake_minimum_required(VERSION 3.10)
project(one LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Boost CONFIG REQUIRED COMPONENTS system json program_options url)
find_package(OpenSSL REQUIRED)
add_executable(one
    src/common/address.cpp
    src/common/app.cpp
    src/common/builder.cpp
    src/config/config.cpp
    src/connections/tcp.cpp
    src/connections/tls.cpp
    src/connections/ws.cpp
    src/connections/wss.cpp
    src/connections/relay.cpp
    src/dns/servers/udp.cpp
    src/dns/dns.cpp
    src/dns/message.cpp
    src/inbounds/inbound.cpp
    src/inbounds/mixed.cpp
    src/listeners/listener.cpp
    src/listeners/tcp.cpp
    src/listeners/tls.cpp
    src/listeners/ws.cpp
    src/listeners/wss.cpp
    src/outbounds/direct.cpp
    src/outbounds/vless.cpp
    src/router/ipnet.cpp
    src/router/router.cpp
    src/router/rule.cpp
    src/sniff/sniff.cpp
    src/tcp/tcpclient.cpp
    src/transports/tcp.cpp
    src/transports/tls.cpp
    src/transports/wss.cpp
    src/udp/udpclient.cpp
    src/utils/bytes.cpp
    src/utils/file.cpp
    src/utils/json.cpp
    src/utils/log.cpp
    src/main.cpp
)

target_include_directories(one PRIVATE include src)
target_compile_definitions(one PRIVATE BOOST_ASIO_NO_DEPRECATED)
target_link_libraries(
    one PRIVATE Boost::system OpenSSL::SSL OpenSSL::Crypto Boost::json Boost::url)
if(WIN32)
    target_compile_definitions(one PRIVATE
        _WIN32_WINNT=0x0A00
    )
endif()
if(MSVC)
    target_compile_options(one PRIVATE /bigobj)
endif()
