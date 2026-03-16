cmake_minimum_required(VERSION 3.20)
project(LumenThreaded
	VERSION 1.0.0
	LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG master
)
FetchContent_MakeAvailable(stb)

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/thread_pool.cpp
	src/image_processor.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
	include
	${stb_SOURCE_DIR}
)

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
endif()


option(ENABLE_SANITIZERS "Enable ASan and TSan" OFF)
if(ENABLE_SANITIZERS AND NOT MSVC)
	target_link_libraries(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=thread -g)
	target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=thread)
endif()

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
