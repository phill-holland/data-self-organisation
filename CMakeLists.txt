cmake_minimum_required(VERSION 3.20)

project(data-self-organisation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

set(DPCPP_HOME "/home/phill/sycl_workspace")
set(CMAKE_CXX_COMPILER "${DPCPP_HOME}/llvm/build/bin/clang++")

set(CMAKE_BUILD_TYPE "Debug")

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(LLVM_ENABLE_THREADS TRUE)
set(LLVM_PARALLEL_COMPILE_JOBS, 8)
set(LLVM_PARALLEL_LINK_JOBS, 8)

find_package(oneDPL REQUIRED)
find_package(Threads REQUIRED)

#include(FetchContent)
#FetchContent_Declare(
  #googletest
  #URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
#)

#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)

include_directories("${DPCPP_HOME}/llvm/build/include")
include_directories("${DPCPP_HOME}/llvm/build/include/sycl")
include_directories("/usr/include")
include_directories("src/include")

#add_subdirectory(src)
#add_subdirectory(test)


file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS 
"${PROJECT_SOURCE_DIR}/src/source/*.cpp" 
#"${PROJECT_SOURCE_DIR}/source/parallel/source/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_compile_options(${PROJECT_NAME} PRIVATE -fsycl -fsycl-targets=nvptx64-nvidia-cuda,spir64_gen)

target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
target_link_libraries(${PROJECT_NAME} sycl)

target_link_options(${PROJECT_NAME} PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda)
