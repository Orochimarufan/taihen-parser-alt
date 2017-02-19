cmake_minimum_required(VERSION 3.0)

project(taihen-parser-alt)

add_definitions(-Os -ffunction-sections -fdata-sections)

set(C_HDRS
    ${PROJECT_SOURCE_DIR}/include/taihen/lexer.h
    ${PROJECT_SOURCE_DIR}/include/taihen/parser.h
)

set(C_SRCS
    ${PROJECT_SOURCE_DIR}/src/lexer.c
    ${PROJECT_SOURCE_DIR}/src/parser.c
)

add_library(taihen-parser-alt ${C_SRCS})
target_include_directories(taihen-parser-alt PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(taihen-parser-alt PUBLIC -DTAIHEN_PARSER_ALT)
set_target_properties(taihen-parser-alt PROPERTIES PUBLIC_HEADER "${C_HDRS}")
