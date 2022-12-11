include_directories(${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(llvm_libs
  Core
)
add_library(hls STATIC ast_visitor.cpp)
target_include_directories(hls PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(hls PRIVATE ${llvm_libs})
