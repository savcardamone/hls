find_package(GTest REQUIRED)

add_executable(lexer_test lexer_test.cpp)
target_link_libraries(lexer_test PRIVATE
   hls GTest::gtest_main
)

add_executable(parser_test parser_test.cpp)
target_link_libraries(parser_test PRIVATE
   hls GTest::gtest_main
)

