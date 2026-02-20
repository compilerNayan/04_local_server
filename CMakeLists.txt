cmake_minimum_required(VERSION 3.14)
project(04_local_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch springbootplusplus_web (HTTP request/response, dispatcher, etc.)
FetchContent_Declare(
    springbootplusplus_web
    GIT_REPOSITORY https://github.com/compilerNayan/springbootplusplus_web.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(springbootplusplus_web)

# Header-only INTERFACE library; exposes springbootplusplus_web
add_library(04_local_server INTERFACE)

set_target_properties(04_local_server PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_include_directories(04_local_server INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link springbootplusplus_web (provides HttpRequestManager, HttpRequestDispatcher, etc.)
if(TARGET springbootplusplus_web::springbootplusplus_web)
    target_link_libraries(04_local_server INTERFACE springbootplusplus_web::springbootplusplus_web)
elseif(TARGET springbootplusplus_web)
    target_link_libraries(04_local_server INTERFACE springbootplusplus_web)
else()
    FetchContent_GetProperties(springbootplusplus_web)
    if(springbootplusplus_web_SOURCE_DIR)
        target_include_directories(04_local_server INTERFACE
            $<BUILD_INTERFACE:${springbootplusplus_web_SOURCE_DIR}/src>
        )
    endif()
endif()

include(GNUInstallDirs)

install(TARGETS 04_local_server
    EXPORT 04_local_serverTargets
)

install(EXPORT 04_local_serverTargets
    FILE 04_local_serverTargets.cmake
    NAMESPACE 04_local_server::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/04_local_server
)
