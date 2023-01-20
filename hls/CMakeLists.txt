# Retrieve the LLVM dependency and get the libraries that we need
find_package(LLVM REQUIRED)
llvm_map_components_to_libnames(llvm_libs
  Core ScalarOpts
  )

add_library(hls STATIC ast_visitor.cpp graph_visitor.cpp)
# Project headers are included publically while LLVM is private
target_include_directories(hls
  PUBLIC ${PROJECT_SOURCE_DIR}
  PRIVATE ${LLVM_INCLUDE_DIRS}
  )
# May need this to be public for unit testing in the future
target_link_libraries(hls
  PRIVATE ${llvm_libs}
  )
