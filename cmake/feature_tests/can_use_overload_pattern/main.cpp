#include <variant>

//
// Created by marcel on 10/27/20.
//
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename V, typename... Os>
constexpr auto MatchType(V&& variant, Os... overloads)
{
    return std::visit(overloaded{overloads...}, std::forward<V>(variant));
}

int main(){return 0;}