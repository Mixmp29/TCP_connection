function(set_compile_options target_name)
  if(MSVS)
    target_compile_options(${target_name} PRIVATE /W4 /WX)
  else()
    target_compile_options(${target_name} PRIVATE -std=c++20 -Wall -lpthread)
  endif()

  set_target_properties(
    ${target_name}
    PROPERTIES
      CXX_STANDARD 20
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF
  )
  
endfunction()