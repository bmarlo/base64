cmake_minimum_required(VERSION 3.10)

project(base64)

function(cpp17 target)
    set_target_properties(${target}
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )

    target_compile_options(${target}
        PRIVATE
            -Wall -Wextra -Wconversion -pedantic -Wno-sign-conversion
    )
endfunction()

add_library(base64 STATIC src/base64.cpp)
cpp17(base64)
target_include_directories(base64 PUBLIC include)
