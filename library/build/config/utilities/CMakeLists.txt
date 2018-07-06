file(GLOB source_files ./../../../source/utilities_tests/*.cpp)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

add_executable(utilities_tests ${source_files})
add_custom_command(TARGET utilities_tests POST_BUILD COMMAND utilities_tests)
install(TARGETS utilities_tests CONFIGURATIONS Debug
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/debug)
install(TARGETS utilities_tests CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/release)
