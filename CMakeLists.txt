cmake_minimum_required(VERSION 3.20)

project(MiniFazenda2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

file(GLOB_RECURSE MINIFAZENDA_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.hpp"
)

add_executable(MiniFazenda2
    src/Principal.cpp
    ${MINIFAZENDA_HEADERS}
)

target_include_directories(MiniFazenda2 PRIVATE src)
target_compile_definitions(MiniFazenda2 PRIVATE SDL_MAIN_HANDLED)

find_package(SDL2 CONFIG QUIET)
if(TARGET SDL2::SDL2)
    target_link_libraries(MiniFazenda2 PRIVATE SDL2::SDL2)
elseif(SDL2_FOUND)
    target_include_directories(MiniFazenda2 PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(MiniFazenda2 PRIVATE ${SDL2_LIBRARIES})
else()
    find_package(SDL2 REQUIRED)
    target_include_directories(MiniFazenda2 PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(MiniFazenda2 PRIVATE ${SDL2_LIBRARIES})
endif()

find_package(SDL2_image CONFIG QUIET)
if(TARGET SDL2_image::SDL2_image)
    target_link_libraries(MiniFazenda2 PRIVATE SDL2_image::SDL2_image)
else()
    find_package(SDL2_image REQUIRED)
    target_include_directories(MiniFazenda2 PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_image_INCLUDE_DIRS})
    target_link_libraries(MiniFazenda2 PRIVATE ${SDL2_IMAGE_LIBRARIES} ${SDL2_image_LIBRARIES})
endif()

find_package(SDL2_ttf CONFIG QUIET)
if(TARGET SDL2_ttf::SDL2_ttf)
    target_link_libraries(MiniFazenda2 PRIVATE SDL2_ttf::SDL2_ttf)
else()
    find_package(SDL2_ttf REQUIRED)
    target_include_directories(MiniFazenda2 PRIVATE ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_ttf_INCLUDE_DIRS})
    target_link_libraries(MiniFazenda2 PRIVATE ${SDL2_TTF_LIBRARIES} ${SDL2_ttf_LIBRARIES})
endif()

find_package(SDL2_mixer CONFIG QUIET)
if(TARGET SDL2_mixer::SDL2_mixer)
    target_link_libraries(MiniFazenda2 PRIVATE SDL2_mixer::SDL2_mixer)
else()
    find_package(SDL2_mixer REQUIRED)
    target_include_directories(MiniFazenda2 PRIVATE ${SDL2_MIXER_INCLUDE_DIRS} ${SDL2_mixer_INCLUDE_DIRS})
    target_link_libraries(MiniFazenda2 PRIVATE ${SDL2_MIXER_LIBRARIES} ${SDL2_mixer_LIBRARIES})
endif()

add_custom_command(TARGET MiniFazenda2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:MiniFazenda2>/assets"
)

enable_testing()

add_executable(MiniFazenda2Tests
    tests/TestesLogica.cpp
    ${MINIFAZENDA_HEADERS}
)

target_include_directories(MiniFazenda2Tests PRIVATE src)

add_test(NAME MiniFazenda2Tests COMMAND MiniFazenda2Tests)
