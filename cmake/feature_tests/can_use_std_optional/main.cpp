#include <optional>

int main(int argc, char**){
    std::optional<int> tmp;
    if (argc){
        tmp = argc;
    }
    return tmp.has_value();
}