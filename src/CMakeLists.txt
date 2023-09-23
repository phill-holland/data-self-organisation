set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.hpp *.cpp)

set(SOURCES ${SOURCES})

set(CMAKE_CXX_STANDARD 20)
set(DPCPP_HOME "/home/phill/sycl_workspace")
set(CMAKE_CXX_COMPILER "${DPCPP_HOME}/llvm/build/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

add_executable(${BINARY} ${SOURCES})

add_library(${BINARY}_lib STATIC ${SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_compile_options(${PROJECT_NAME} PRIVATE -fsycl -fsycl-targets=nvptx64-nvidia-cuda,spir64_gen)
target_compile_options(${BINARY}_lib PRIVATE -fsycl -fsycl-targets=nvptx64-nvidia-cuda,spir64_gen)

target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
target_link_libraries(${PROJECT_NAME} sycl)

target_link_options(${PROJECT_NAME} PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)


