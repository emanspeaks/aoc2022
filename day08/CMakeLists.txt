add_executable(day08)  # WIN32)
# for VS, set the startup target (the one that starts in debugger by default)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT day08)
if (MINGW)
  target_compile_options(day08 PUBLIC -mwindows -std=c++20)
endif()

add_custom_command(TARGET day08 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/input.txt $<TARGET_FILE_DIR:day08>
)

add_subdirectory(include)
add_subdirectory(src)
