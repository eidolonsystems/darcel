file(GLOB source_files ./../../../source/operations_tests/*.cpp)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

add_executable(operations_tests ${source_files})
add_custom_command(TARGET operations_tests POST_BUILD COMMAND operations_tests)
install(TARGETS operations_tests CONFIGURATIONS Debug
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/debug)
install(TARGETS operations_tests CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/release)
