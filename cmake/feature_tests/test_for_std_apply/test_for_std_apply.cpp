#include <tuple>

int use_std_apply(){
    auto add = [](auto&&...item){
        return (0+...+item);
    };

    return std::apply(add, std::make_tuple(1,2,3))==6;
}
int main(){
    return use_std_apply();
}