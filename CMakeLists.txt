cmake_minimum_required(VERSION 3.10)

# set the project name
project(Engine)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(Engine main.cpp)

#add Vulkan to the project
find_package(Vulkan REQUIRED)
target_include_directories(Engine PUBLIC ${Vulkan_INCLUDE_DIR})
target_link_libraries(Engine ${Vulkan_LIBRARY})

#add GLFW into the project
add_subdirectory(extern/glfw)
target_include_directories(Engine PUBLIC extern/glfw/include)
target_link_directories(Engine PRIVATE extern/glfw/src)
target_link_libraries(Engine glfw)

#add Vulkan Memory Allocator to the project
target_include_directories(Engine PUBLIC extern/VulkanMemoryAllocator/include)

#add OpenAL to the project
add_subdirectory(extern/openal-soft)
target_include_directories(Engine PUBLIC extern/openal-soft/include)
target_link_directories(Engine PRIVATE extern/openal-soft)
target_link_libraries(Engine OpenAL32)


