# SDL
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SDL)

# IMGUI
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(3rd_parties STATIC)

# STB
set(STB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stb)

# Boost DI
# STB
set(DI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/di/include)

target_sources(3rd_parties PRIVATE
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp)

target_include_directories(3rd_parties PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${SDL3_DIR}/include
    ${STB_DIR}
    ${DI_DIR})

target_include_directories(3rd_parties SYSTEM PUBLIC
    ${DI_DIR})

target_compile_features(3rd_parties PRIVATE cxx_std_17 c_std_17)

# Combine all together
find_package(OpenGL REQUIRED)
target_link_libraries(3rd_parties PUBLIC
    ${OPENGL_LIBRARIES}
    SDL3-shared
    ${CMAKE_DL_LIBS})