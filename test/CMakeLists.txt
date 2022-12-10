find_package(GTest REQUIRED)

add_executable(lexer_test lexer_test.cpp)
target_include_directories(lexer_test PRIVATE
   $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)
target_link_libraries(lexer_test PRIVATE
   GTest::gtest_main
)

llvm_map_components_to_libnames(llvm_libs
  Core
)

add_executable(parser_test parser_test.cpp)
target_include_directories(parser_test PRIVATE
     $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)
target_link_libraries(parser_test PRIVATE
   GTest::gtest_main ${llvm_libs}
)

