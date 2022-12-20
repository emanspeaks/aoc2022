add_executable(day10)  # WIN32)
# for VS, set the startup target (the one that starts in debugger by default)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT day10)

add_custom_command(TARGET day10 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/input.txt $<TARGET_FILE_DIR:day10>
)

add_subdirectory(include)
add_subdirectory(src)
