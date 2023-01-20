enable_testing()
find_package(GTest REQUIRED)

# Pile all of our unit tests into a single executable
add_executable(hls_unit_tests
  lexer_test.cpp ast_test.cpp parser_test.cpp ast_visitor_test.cpp
  graph_test.cpp graph_visitor_test.cpp
  )
target_link_libraries(hls_unit_tests PRIVATE
   hls GTest::gtest_main
   )

# Discover all the tests for CTest 
include(GoogleTest)
gtest_discover_tests(hls_unit_tests)
