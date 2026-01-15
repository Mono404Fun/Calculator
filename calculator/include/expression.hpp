#include "token.hpp"
#include "utils.hpp"

#include <deque>
#include <string_view>

namespace sya {
  class Expression : public utils::ItClasses<std::deque<Token>> {
    private:
    using token_list = std::deque<Token>;
    using it = token_list::iterator;
    using cit = token_list::const_iterator;

    std::string m_expr;
    token_list m_tokens;

    public:
    Expression() noexcept = default;
    explicit Expression(const std::string_view expr) noexcept;
    Expression(const Expression& expr);
    Expression(Expression&& expr) noexcept;

    Expression& operator=(const Expression&  expr);
    Expression& operator=(const Expression&& expr) noexcept;

    [[nodiscard]] constexpr inline const std::string& expresion() const noexcept;
    [[nodiscard]] constexpr inline const token_list& tokens() const noexcept;

    it begin() { return tokens.begin(); }
    it end() { return tokens.end(); }
    cit begin() const;
    cit end() const;
    cit cbegin() const;
    cit cend() const;

    void push(Token &token);
    void pop();
    [[nodiscard]] Token& first() const noexcept;
    [[nodiscard]] Token& last() const noexcept;

    [[nodiscard]] void clear() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] size_t size() const noexcept;
  }
}
