add_library(base
    run_loop.cpp
    run_loop_ui.cpp
    priority_task_queue.cpp
    steady_time_provider.cpp
    task_pump_std.cpp)

target_include_directories(base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(base PRIVATE project_options)