file(GLOB source_files ./../../../source/lexicon_tests/*.cpp)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

add_executable(lexicon_tests ${source_files})
add_custom_command(TARGET lexicon_tests POST_BUILD COMMAND lexicon_tests)
install(TARGETS lexicon_tests CONFIGURATIONS Debug
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/debug)
install(TARGETS lexicon_tests CONFIGURATIONS Release RelWithDebInfo
  DESTINATION $ENV{TEST_INSTALL_DIRECTORY}/release)
