/**
 * @file lexer.hpp
 * @author Salvatore Cardamone
 * @brief Lexical analysis of the Kaleidoscope language.
 */
#ifndef __HLS_LEXER_HPP
#define __HLS_LEXER_HPP

#include <iostream>
#include <optional>
#include <string>

namespace hls {

/**
 * @brief Tokens supported by the Kaleidoscope language. Lexical analysis must
 * return one of the following.
 */
enum class TokenType {
  tok_none,
  tok_eof,  //< End-of-file token
  // Keywords
  tok_def,     //< Function definition keyword
  tok_extern,  //< Extern function keyword
  // Primary
  tok_identifier,  //< Identifier (i.e. variable name)
  tok_number,      //< Numerical value
  tok_operator     //< Operator token
};

/**
 * @brief Wrapper around a token type and the associated string that was lexed
 * and identified as being of that type.
 *
 * Note that the associated string is stored as std::optional since certain
 * token types are fully descriptive of the token value (e.g. keywords).
 */
class Token {
 public:
  Token() : type_{TokenType::tok_none}, value_{std::nullopt} {}

  /**
   * @brief Class constructor.
   * @param type Type of token that was lexed.
   * @param value Value of the token that was lexed. Default initialised as
   * std::nullopt.
   */
  Token(TokenType&& type, std::optional<std::string> value = std::nullopt)
      : type_{std::move(type)}, value_{std::move(value)} {}

  /**
   * @brief Equality comparison operator.
   * @param rhs RHS Token to the equality condition.
   * @return True if Tokens are elementwise equal, otherwise False.
   */
  bool operator==(const Token& rhs) const {
    return type() == rhs.type() && value() == rhs.value();
  }

  /**
   * @brief Getter for the Token type.
   * @return Type of Token.
   */
  const TokenType& type() const { return type_; }

  /**
   * @brief Getter for the Token value.
   * @return Value of the Token.
   */
  const std::string value() const {
    return value_.value_or("");
  }

 private:
  TokenType type_;
  std::optional<std::string> value_;
};

/**
 * @brief Lexical analysis of the Kaleidoscope language.
 */
class Lexer {
 public:
  /**
   * @brief Class constructor.
   */
  Lexer() {}

  /**
   * @brief Retrieve a token from the input stream.
   * @param input The input stream to lex from.
   * @return The next Token parsed from the input stream.
   */
  Token get_token(std::istream& input) {
    // Eat any whitespace (including tabs, newlines and spaces)
    while (isspace(last_char_)) last_char_ = input.get();

    // If the character is alphabetical, it's either an identifier or
    // language keyword, so consume all subsequent alphanumerical characters
    if (isalpha(last_char_)) {
      std::string identifier_string(1, last_char_);
      while (isalnumuscore(last_char_ = input.get())) {
        identifier_string += last_char_;
      }

      // Handle any keywords
      if (identifier_string == "def") {
        return Token(TokenType::tok_def);
      } else if (identifier_string == "extern") {
        return Token(TokenType::tok_extern);
      }
      // If the token wasn't a keyword, it was an identifier
      return Token(TokenType::tok_identifier, identifier_string);
    }

    // If the character is numerical, it's a numerical constant of some
    // kind, so consume all subsequent numerical/ point characters
    if (isdigit(last_char_) || last_char_ == '.') {
      std::string numerical_string;
      do {
        numerical_string += last_char_;
        last_char_ = input.get();
      } while (isdigit(last_char_) || last_char_ == '.');
      // Return a numerical token
      return Token(TokenType::tok_number, numerical_string);
    }

    // If the character indicates a comment, consume the entire line
    if (last_char_ == '#') {
      do {
        last_char_ = input.get();
      } while (last_char_ != EOF && last_char_ != '\n' && last_char_ != '\r');
      // Recursively call this method so we can return a token if we didn't
      // reach the end-of-file
      if (last_char_ != EOF) return get_token(input);
    }

    // If the character is the end-of-file, then we're done, so return the
    // corresponding token
    if (last_char_ == EOF) return Token(TokenType::tok_eof);

    // Otherwise we've encountered some non-alphanumerical/ comment/ whitespace
    // character, so this must correspond to an operator of some kind that we
    // can return as a token comprising a single character
    std::string this_char(1, last_char_);
    last_char_ = input.get();
    return Token(TokenType::tok_operator, this_char);
  }

 private:
  int last_char_ = ' ';

  /**
   * @brief Check whether a character is alphanumerical or an underscore.
   * @param input The character to check.
   * @return True if the character is alphanumerical or an underscore, false
   * otherwise.
   */
  bool isalnumuscore(int& input) { return isalnum(input) || input == '_'; }
};

}  // namespace hls

#endif /* #ifndef __HLS_LEXER_HPP */
