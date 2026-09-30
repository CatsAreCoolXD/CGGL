#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "CGGL::CGGL" for configuration ""
set_property(TARGET CGGL::CGGL APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CGGL::CGGL PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib64/libCGGL.a"
  )

list(APPEND _cmake_import_check_targets CGGL::CGGL )
list(APPEND _cmake_import_check_files_for_CGGL::CGGL "${_IMPORT_PREFIX}/lib64/libCGGL.a" )

# Import target "CGGL::glfw" for configuration ""
set_property(TARGET CGGL::glfw APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CGGL::glfw PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib64/libglfw3.a"
  )

list(APPEND _cmake_import_check_targets CGGL::glfw )
list(APPEND _cmake_import_check_files_for_CGGL::glfw "${_IMPORT_PREFIX}/lib64/libglfw3.a" )

# Import target "CGGL::glad" for configuration ""
set_property(TARGET CGGL::glad APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CGGL::glad PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib64/libglad.a"
  )

list(APPEND _cmake_import_check_targets CGGL::glad )
list(APPEND _cmake_import_check_files_for_CGGL::glad "${_IMPORT_PREFIX}/lib64/libglad.a" )

# Import target "CGGL::glm" for configuration ""
set_property(TARGET CGGL::glm APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CGGL::glm PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib64/libglm.a"
  )

list(APPEND _cmake_import_check_targets CGGL::glm )
list(APPEND _cmake_import_check_files_for_CGGL::glm "${_IMPORT_PREFIX}/lib64/libglm.a" )

# Import target "CGGL::freetype" for configuration ""
set_property(TARGET CGGL::freetype APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CGGL::freetype PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib64/libfreetype.a"
  )

list(APPEND _cmake_import_check_targets CGGL::freetype )
list(APPEND _cmake_import_check_files_for_CGGL::freetype "${_IMPORT_PREFIX}/lib64/libfreetype.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
