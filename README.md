master: [![Build Status](https://travis-ci.org/ferkulat/CppResultType.svg?branch=master)](https://travis-ci.org/ferkulat/CppResultType)
[![Build status](https://ci.appveyor.com/api/projects/status/github/ferkulat/CppResultType?brach=master&svg=true)](https://ci.appveyor.com/project/ferkulat/CppResultType)
# CppResultType
This a header only for a type of Result<T,E>.

Its inspired by Result<T,E> in Rust, but much simpler.
No methods like map, and_then, ... .


With an overloaded operator (in this case operator|()) it should be more convenient to chain function calls in a readable way without omitting error handling.

This implementation assumes that all passed and returned types of Result<T,E> in the call chain use the same error type E.

What is it good for?

- no output parameter anymore and still return errors to the caller
- chain function calls, checking for errors and return early in case of an error
- no explicit temporaries in function chain

### Before C++17
A common way to return values and errors from functions is using output parameters or use some special values to indicate an error.

Here are both cases shown in one snippet:
```c++
int someFunction(int index)
{
    int value;

    // some times a lot of other code here...

    if (!someOtherFunction(index, &value)){
        return -1;
    }
}
```
The user of ```someFunction``` needs to know this, by reading either documentation or having a look into its implementation, that ```-1``` indicates an error.

For ```someOtherFunction``` its more easy to see ```value``` as the desired result and the if-statement as a way to check if ```value``` is valid.
But its breaks the flow of reading when you see ```int value;``` and (if you are lucky) in the next line the pointer to ```value``` in the parameter list.
And its not possible to create a ```const int value```.

### Since C++17 (std::optional < T > )

C++17 introduced std::optional<T>. Which holds a boolean value to indicate the existence or absence of ```T```. With this we can improve it:
```c++
std::optional<int> someFunction(int index)
{
    auto const opt_result1 = someOtherFunction(index);
    if(!opt_result1.has_value())
        return std::nullopt;
    }
    return opt_result1.value();
}
```
Function parameters are always input and we can communicate the absence of a value more clearly.

### Explicit error checking

If we have to do more then one function call to create the result, it gets noisy:

```c++
std::optional<int> someFunction(SomeClass someClass)
{
    auto const opt_result1 = someOtherFunction(someClass);
    if(!opt_result1.has_value())
        return std::nullopt;
    }

    auto const opt_result2 = someReallyOtherFunction(opt_result1.value());
    if(!opt_result2.has_value())
        return std::nullopt;
    }
    return opt_result2.value();
}
```
### Implicit error checking
But the approach of error checking and early return is the same in both cases.
Lets put this in a special function by overloading ```operator|()```.

It enables us:
- make sure that nothing on the right side of ```|``` will be called when on its left side an optional without a value in there appears.
- make sure the result of this chain is a simple optional and not lots of nested optionals.
- chain of an optional value with a function, which returns a non optional, returns an optional 
```c++
template<typename Value, typename Function>
auto operator|(Value value, Function function){
        return function(value);
}

// lots of details are missing here
// its about the basic idea, not a working implementation

template<typename Value, typename Function>
auto operator|(std::optional<Value> value, Function function){
        if (value.has_value()){
            return function(value.value());
        }
        return std::nullopt
}

std::optional<int> someFunction(SomeClass someClass)
{
    return someClass | someOtherFunction | someReallyOtherFunction;
}

```
Overloading ```operator|()``` has one limitation: at least one of its parameters needs to be a non primitive type. And the function on the right side are pointers to functions. Which is already a primitive type.
We could use lambdas to solve this.

### Return other error values than true/false

The idea of ```std::optional<T>``` made more general, would be something like
```Result<Success, Error>``` where you are free to transport other error types than ```bool```.
It seems to make things easier to use the same type of Error in you whole project.
Which is not really a limitation, since you can use a scoped enum or something like ```std::variant<ErrorType1, ErrorType2, ...>```.
```c++
#include <result_type.h>

using ResultType::operator|();

enum class Error{Err1, Err2, Err2};

auto someComplexCalculation(int value)-> int{
    ...
}

auto someSimpleCalculation(int value)->int{
    ...
}
auto getInputFromUser()->std::string{
    ....
    return something;
}

auto convertToInt(std::string const& input) -> ResultType<int, Error>{
    std::stringstream s;
    int result;
    s << input;
    s >> result;
    if (s.fail()){
        return Error::Err1;
    }
    return result;
}

auto const result = getInputFromUser() | convertToInt | someComplexCalculation | someSimpleCalculation;

// to check for error
if (ResultType::IsError(result)){
    doSomethingWithTheError(result.CRefError());
    return std::move(result).Error();
}
// to check for success
if (ResultType::IsSuccess(result)){
    doSomething(result.CRefSuccess());
}
```

Sometimes when you do explicit checking on error and success, you realize you can put the action on success in a function and add it to the existing chain.

### Links to sources that inspired me

- https://doc.rust-lang.org/book/ch09-02-recoverable-errors-with-result.html?highlight=result#recoverable-errors-with-result
- Railway Oriented Programming by Scott Wlaschin https://fsharpforfunandprofit.com/rop/
- Functional C++ for Fun and Profit by Phil Nash https://www.youtube.com/watch?v=YgcUuYCCV14
- CppCon 2018: Andrei Alexandrescu “Expect the expected” https://www.youtube.com/watch?v=PH4WBuE1BHI
 