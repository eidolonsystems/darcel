file(GLOB darcel_header_files ./../../../include/darcel/darcel/*.hpp)
source_group("darcel" FILES ${darcel_header_files})
file(GLOB data_types_header_files ./../../../include/darcel/data_types/*.hpp)
source_group("data_types" FILES ${data_types_header_files})
file(GLOB lexicon_header_files ./../../../include/darcel/lexicon/*.hpp)
source_group("lexicon" FILES ${lexicon_header_files})
file(GLOB operations_header_files ./../../../include/darcel/operations/*.hpp)
source_group("operations" FILES ${operations_header_files})
file(GLOB reactors_header_files ./../../../include/darcel/reactors/*.hpp)
source_group("reactors" FILES ${reactors_header_files})
file(GLOB semantics_header_files ./../../../include/darcel/semantics/*.hpp)
source_group("semantics" FILES ${semantics_header_files})
file(GLOB syntax_header_files ./../../../include/darcel/syntax/*.hpp)
source_group("syntax" FILES ${syntax_header_files})
file(GLOB type_checks_header_files
  ./../../../include/darcel/type_checks/*.hpp)
source_group("type_checks" FILES ${type_checks_header_files})
file(GLOB utilities_header_files ./../../../include/darcel/utilities/*.hpp)
source_group("utilities" FILES ${utilities_header_files})

file(GLOB header_files ${darcel_header_files} ${data_types_header_files}
  ${lexicon_header_files} ${operations_header_files} ${reactors_header_files}
  ${semantics_header_files} ${syntax_header_files} ${type_checks_header_files}
  ${utilities_header_files})

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

add_library(darcel ${header_files} ${source_files})
set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
set_target_properties(darcel PROPERTIES STATIC_LIBRARY_FLAGS_RELEASE
"${CMAKE_LIBRARY_FLAGS}" LINKER_LANGUAGE CXX OUTPUT_NAME darcel)
