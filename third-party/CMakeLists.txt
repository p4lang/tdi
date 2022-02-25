if(STANDALONE)
  add_subdirectory(target-utils)
  add_subdirectory(target-syslibs)
endif()

if(TDI_GTEST)
  add_subdirectory(googletest)
endif()
