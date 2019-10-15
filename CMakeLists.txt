cmake_minimum_required (VERSION 2.8.9)

project(lyr_opengl_task1)

SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

add_executable(lyr_opengl_task1 
  main.cpp
  common/glad_shader.cpp
  common/glad_shader.hpp
  common/objloader.hpp
  common/objloader.cpp
  common/stb_image.h
  common/stb_image.c
  common/stb_image_write.h
  common/stb_image_write.c
)

find_package(OpenGL REQUIRED)
#find_package(GLEW REQUIRED)

# glad
set(GLAD_DIR "external/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")

add_subdirectory(external/glfw-3.3)
add_subdirectory(external/cnpy)

include_directories(
  ${OPENGL_INCLUDE_DIR}
  #${GLEW_INCLUDE_DIR}
  ${GLAD_INCLUDE_DIR}
  external/glm-0.9.6.3/
  external/glfw-3.3/include
  ${GLAD_DIR}/include
  external/cnpy
)

set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw
    cnpy
    #${GLEW_LIBRARY}
    ${GLAD_LIBRARY}
    "glad" "${CMAKE_DL_LIBS}"
)

target_link_libraries(lyr_opengl_task1 ${ALL_LIBS})
