add_executable(day15)  # WIN32)
# for VS, set the startup target (the one that starts in debugger by default)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT day15)
if (MINGW)
  target_compile_options(day15 PUBLIC -mwindows -std=c++20)
endif()

add_custom_command(TARGET day15 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/input.txt $<TARGET_FILE_DIR:day15>
)

add_subdirectory(include)
add_subdirectory(src)
