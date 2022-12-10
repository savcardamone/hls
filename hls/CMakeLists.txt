include_directories(${LLVM_INCLUDE_DIRS})
add_library(hls INTERFACE lexer.hpp parser.hpp ast.hpp ast_visitor.hpp)
