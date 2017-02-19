cmake_minimum_required(VERSION 3.0)

project(taihen-parser-bench)

add_definitions(-Os)

# Alt Implementation
add_executable(tpb-main-alt main.c)
target_link_libraries(tpb-main-alt taihen-parser-alt)

add_executable(tpb-bench1-alt bench1.c)
target_link_libraries(tpb-bench1-alt taihen-parser-alt)

# Original Implementation
if (TARGET taihenparser)
    macro(target_link target)
    target_include_directories(${target} PRIVATE ../taihen-parser/include)
    target_link_libraries(${target} taihenparser)
    endmacro(target_link)

    add_executable(tpb-main main.c)
    target_link(tpb-main)

    add_executable(tpb-bench1 bench1.c)
    target_link(tpb-bench1)
endif(TARGET taihenparser)
