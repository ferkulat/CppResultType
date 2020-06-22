master: [![Build Status](https://travis-ci.org/ferkulat/CppResultType.svg?branch=master)](https://travis-ci.org/ferkulat/CppResultType)
[![Build status](https://ci.appveyor.com/api/projects/status/github/ferkulat/CppResultType?brach=master&svg=true)](https://ci.appveyor.com/project/ferkulat/CppResultType)
# CppResultType
#### Table of contents
 - [Motivation](#Motivation)
 - [Introduction](#introduction)
 - [Documentation](#Documentation)
 - [External resources](#ExternalLinks)
 
## Motivation <a name="Motivation"></a>
In a lot of applications the biggest part is busyness logic that is not performance critical. But the implementer needs to deal with untrusted input, that needs to be checked. Or there are cases when returning a non valid value is a recoverable error or even not an error. 
- Checking for errors or invalid values lets the implementer write a lot of boiler plate code, which lacks readability quickly.
- All to often: Checking results from function calls for errors is neglected and the "happy path" is the only path.
- Throwing exceptions is a side effect and the user of the library will only know by peeking into the implementation or has to rely on documentation. The compiler will not be able to help you catching bugs. This is a solid basis for hard to find bugs.

## Introduction <a name="introduction"></a>

This a header only library which enables the library user to 
 - chain function calls in a readable fashion, that looks like piping commands in a shell script.
 - Errors can be a visible part of the API by using/writing functions returning values which are able to hold either success or error.
 - the compiler will be able help you catch some bugs
 - checking for errors and leaving the call chain early with an error as result, is done implicitly by the library
 - the general idea is to return errors to the caller, which needs to deal with it.
 - this makes the functions easier to test

In general this only works if all functions take one parameter. To break such chains in case of an error, functions can return values of type ```Result<SuccessType, ErrorType>```.
 This type holds either a value of type ```SuccessType``` or of type ```ErrorType```. The function on the right side of the pipe will only called if the returned type on the left side holds a ```SucccessType```. In case it holds an ```ErrorType``` the call chain will be left early.

The same holds for dealing with ```std::optional<T>```, if the result on the left side holds a value, the function on the right side will be called. Other wise the chain returns early with ```std::nullopt```.

Scott Wlaschin gave a talk about this way of programming, where he calls it ["Railway oriented programming"](#ExternalLinks)

This implementation follows Scotts recommendation to have only one ErrorType to make it easier. Since an Error can be of type ```std::variant<ErrorType1, ..., ErrorTypeN>``` this not really a limitation.

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

### Since C++17 (```std::optional <T>``` )

C++17 introduced ```std::optional<T>```. Which holds a boolean value to indicate the existence or absence of ```T```. With this we can improve it:
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
namespace ResultType{
    template<typename Value, typename Function>
    auto operator|(Value value, Function function){
        // Simplification, incomplete, maybe even incorrect!!
        if constexpr (is_invocable_v<Function, Value>){
            return function(value);
        }
        else if constexpr (is_optional_v<Value>){
            using ReturnType = std::optional<decltype(function(value.value()))>;
            if (value){
                return ReturnType(function(value.value()));
            }
            return ReturnType();
        }
        ...
    }
}

auto someFunction(SomeClass someClass) -> std::optional<int> {
    using ResultType::operator|;
    return someClass | someOtherFunction | someReallyOtherFunction;
}

```
Overloading ```operator|()``` has one limitation: at least one of its parameters needs to be a non primitive type. And the function on the right side are pointers to functions. Which is already a primitive type.
We could use lambdas to solve this.

### Return other error values than true/false

The idea of ```std::optional<T>``` made more general, would be something like
```Result<Success, Error>``` where you are free to transport other error types than ```bool```.
It seems to make things easier to use the same type of Error in your whole project.
Which is not really a limitation, since you can use a scoped enum or something like ```std::variant<ErrorType1, ErrorType2, ...>```.
```c++
#include <result_type.h>


enum class Error{Err1, Err2, Err2};

struct Integer{value int}; // to make the overloaded operator|() work with normal functions

auto someComplexCalculation(Integer value)-> Integer{
    ...
}

auto someSimpleCalculation(Integer value)->Integer{
    ...
}
auto getInputFromUser()->std::string{
    ....
    return something;
}

auto convertToInt(std::string const& input) -> ResultType<Integer, Error>{
    std::stringstream s;
    int result;
    s << input;
    s >> result;
    if (s.fail()){
        return Error::Err1;
    }
    return Integer{result};
}

using ResultType::operator|;
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

## Documentation <a name="Documentation"></a>
This library makes use of an overloaded ``` operator|(Argument, Function)```.
To make this overload work, at least one of its arguments needs to be a non primitive type.
A normal function will be passed as function pointer, which is a primitive type.
Either at least ```Argument``` is a class/struct or you can wrap a lambda expression around the function.
 - [Chaining function calls](#NoErrorsNoOptionals)
 - [When functions take more than one parameter](#MoreParameters)
 - [When functions take/return ```std::optional<T>```](#WithOptional)
 - [When functions take/return ```Result<T, E>```](#WithResult)
 - [When functions take/return ```Result<std::optional<T>, E>```](#WithResultOptional)
 

### Chaining function calls <a name="NoErrorsNoOptionals"></a>
To mitigate the limitation of operator overload we don't use ```int``` as value type:
```c++

#include <result_type.h>

struct Integer{int value;}; // to make the overloaded operator|() work with normal functions
struct SomeType{...};

auto someComplexCalculation(Integer value)-> Integer{
    ...
}

auto someSimpleCalculation(Integer value)->Integer{
    ...
}

auto mostComplexCalculation(SomeType value)->Integer{
    ....
    return something;
}

auto calculationWithTwoParameters( double dependency, Integer integer)-> Integer{
    ...
} 
```

In this case nothing special is needed. You can do this without this library.
```c++
auto const result = someSimpleCalculation(someComplexCalculation(mostComplexCalculation(SomeType{2.0})))
```
But it is hard to read. When reading source code, you do this from left to right.
To know what is happening there, you look for the most inner call, and go back to the left.
In this case this library can improve the reading flow:
```c++
using ResultType::operator|;
auto const result = SomeType{2.0} | mostComplexCalculation | someComplexCalculation | someSimpleCalculation;
``` 

### When functions take more than one parameter <a name="MoreParameters"></a>
Chaining function calls only works when functions take one parameter. To make it work with functions which take more than one parameter, we need to write adaptors.
#### Option 1:
 
Write a function, which takes the dependency and returns a lambda expression, that takes the other parameter and calls the function with two parameters

```c++
auto calculationWith(double dependency){
    return  [dependency](Integer integer){
        return calculationWithTwoParameters(dependency, integer);
    };
}

auto insideSomeOtherFunction(){
    auto const dependency = 3.0;
    ...
   using ResultType::operator|;
   auto const result = SomeType{2.0} | calculationWith(dependency) | mostComplexCalculation | someComplexCalculation;
    ...
}
```
#### Option 2:
 
Use lambda exprsessions to bind the dependency. 

```c++
auto insideSomeOtherFunction(){
    auto const dependency = 3.0;
    ...
    using std::placeholders::_1;
    auto calculationWithDependency = [dependency](Integer integer){ return calculationWithTwoParameters(dependency, integer);};
    using ResultType::operator|;
    auto const result = SomeType{2.0} | calculationWithDependency | mostComplexCalculation | someComplexCalculation;
    ...
}
```

#### Option 3:
 
Use std::bind, that creates the function object for you. 

```c++
auto insideSomeOtherFunction(){
    auto const dependency = 3.0;
    ...
    using std::placeholders::_1;
    auto calculationWithDependency = std::bind(calculationWithTwoParameters, dependency, _1);
    auto const result = SomeType{2.0} | calculationWithDependency | mostComplexCalculation | someComplexCalculation;
    ...
}
```
Option 1 is the preferred way over Option 2 and 3, because Option 1 does not hide the dependency within the call chain.











### When functions take/return ```std::optional<T>``` <a name="WithOptional"></a>
Then using the ```operator|()``` will make sure the expression will be never return a nested ```std::optional<std::optional<...>>```. No need to unpack nested optionals to get the result.
 - [Chaining a function, which returns ```std::optional<T>``` with a function that takes ```std::optional<T>```](#OptionalOptional)
 - [Chaining a function, which returns ```std::optional<T>``` with a function that takes ```T``` and returns non optional ```U```](#OptionalTU)
 - [Chaining a function, which returns ```std::optional<T>``` with a function that takes ```T``` and returns ```void```](#OptionalTvoid)
 - [Chaining a function, which returns ```std::optional<T>``` with a function that takes ```T``` and returns ```std::optional<U>```](#OptionalTOptional)
 
#### Chaining a function, which returns ```std::optional<T>``` with a function that takes ```std::optional<T>``` <a name="OptionalOptional"></a>
In this case no special rule is applied. The function, which takes a ```std::optional<T>```, is called regardless, if the passed optional holds a value or not. The result of this chain will be of the same type as the return type of the second function. Which seems obvious, but in later cases it will be important to pay attention to this.
```c++
#include <result_type.h>



auto mayReturnNothing(SomeType value)-> std::optional<int>{
    ...
}

auto canDealWithGettingNothing(std::optional<int> value)->SomeOtherType{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    ...
    using ResultType::operator|;
    // result is of type 'SomeOtherType'
    auto const result = value | mayReturnNothing | canDealWithGettingNothing;
    ...
}
```
#### Chaining a function, which returns ```std::optional<T>``` with a function that takes ```T``` and returns non optional ```U``` <a name="OptionalTU"></a>
In this case the second function gets called only and if only the returned value from the first function holds a value. If the first function call returns ```std::nullopt```, ```std::nullopt``` will be returned for the whole expression. The result of this chain will be of type ```std::optional<U>```.
```c++
#include <result_type.h>



auto mayReturnNothing(SomeType value)-> std::optional<int>{
    ...
}

auto needsSomething(int value)->SomeOtherType{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    ...
    using ResultType::operator|;
    // result is of type 'std::optional<SomeOtherType>'
    auto const result = value | mayReturnNothing | needsSomething;
    ...
}
```

#### Chaining a function, which returns ```std::optional<T>``` with a function that takes ```T``` and returns ```void``` <a name="OptionalTvoid"></a>
In this case the second function gets called only and if only the returned value from the first function holds a value. Otherwise ```std::nullopt``` is returned. The result of this chain will be of type ```std::optional<ResultType::NothingType>```.
```c++
#include <result_type.h>



auto mayReturnNothing(SomeType value)-> std::optional<int>{
    ...
}

auto needsSomething(int value)->void{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    ...
    using ResultType::operator|;
    // result is of type 'std::optional<ResultType::NothingType>'
    auto const result = value | mayReturnNothing | needsSomething;
    ...
}
```

#### Chaining a function, which returns ```std::optional<T>``` with a function that takes ```T``` and returns ```std::optional<U>``` <a name="OptionalTOptional"></a>
In this case the second function gets called only and if only the returned value from the first function holds a value. If the first function call returns ```std::nullopt```, ```std::nullopt``` will be returned. The result of this chain will be of type ```std::optional<U>```. The ```operator|()``` flattens the return type. There will be no nested ~~```std::optional<std::optional<U>>```~~
```c++
#include <result_type.h>



auto mayReturnNothing(SomeType value)-> std::optional<int>{
    ...
}

auto needsSomething(int value)->std::optional<SomeOtherType>{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    ...
    using ResultType::operator|;
    // result is of type 'std::optional<SomeOtherType>'
    auto const result = value | mayReturnNothing | needsSomething;
    ...
}
```


### When functions take/return ```Result<T, E>``` <a name="WithResult"></a>
The type ```Result<T, E>``` is a sum type. It can only hold either ```T``` or ```E```. Where ```T``` represents success and ```E``` an error.
This library assumes that type ```E``` is the same type for all values. If errors of different types are needed, ```std::variant<ErrorType1,..., ErrorTypeN>``` can be used. 

Using the ```operator|()``` will make sure the expression will be never return a nested Result<Result<...>, Error>. No need to unpack nested results.
 - [Chaining a function, which returns ```Result<T, E>``` with a function that takes ```Result<T, E>```](#ResultResult)
 - [Chaining a function, which returns ```Result<T, E>``` with a function that takes ```T``` and returns ```U```](#ResultTU)
 - [Chaining a function, which returns ```Result<T, E>``` with a function that takes ```T``` and returns ```void```](#ResultTvoid)
 - [Chaining a function, which returns ```Result<T, E>``` with a function that takes ```T``` and returns ```Result<U, E>```](#ResultTResult)
 
#### Chaining a function, which returns ```Result<T, E>``` with a function that takes ```Result<T, E>``` <a name="ResultResult"></a>
In this case no special rule is applied. The function, which takes a ```Result<T, E>```, is called regardless, if the passed value holds a value or an error. The result of this chain will be of the same type as the return type of the second function. Which seems obvious, but in later cases it will be important to pay attention to this.
```c++
#include <result_type.h>


enum class Error{Fail1, Fail2};

auto mayReturnError(SomeType value)-> ResultType<int, Error>{
    ...
}

auto canDealWithErrors(ResultType<int, Error> value)->SomeOtherType{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    ...
    using ResultType::operator|;
    // result is of type 'SomeOtherType'
    auto const result = value | mayReturnError | canDealWithErrors;
    ...
}
```
#### Chaining a function, which returns ```Result<T, E>``` with a function that takes ```T``` and returns ```U``` <a name="ResultTU"></a>
In this case the second function gets called only and if only the returned value from the first function holds a ```T```. If the first function call returns a value holding ```E```, ```E``` will be returned for the whole expression. The result of this chain will be of type ```Result<U, E>```.
```c++
#include <result_type.h>


enum class Error{Fail1, Fail2};

auto mayReturnNothing(SomeType value)-> Result<int, Error>{
    ...
}

auto needsSomething(int value)->SomeOtherType{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    ...
    using ResultType::operator|;
    // result is of type 'Result<SomeOtherType, Error>'
    auto const result = value | mayReturnNothing | needsSomething;
    ...
}
```

#### Chaining a function, which returns ```Result<T, E>``` with a function that takes ```T``` and returns ```void``` <a name="ResultTvoid"></a>
In this case the second function gets called only and if only the returned value from the first function holds a value of ```T```. If it holds ```E```  the expression returns this error. The result of this chain will be of type ```ResultType::Result<ResultType::NothingType, E>```.
```c++
#include <result_type.h>


enum class Error{Fail1, Fail2};

auto mayReturnAnError(SomeType value)-> ResultType::Result<int, Error>{
    ...
}

auto needsSomething(int value)->void{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    ...
    using ResultType::operator|;
    // result is of type 'ResultType::Result<ResultType::NothingType, Error>'
    auto const result = value | mayReturnAnError | needsSomething;
    ...
}
```

#### Chaining a function, which returns ```Result<T, E>``` with a function that takes ```T``` and returns ```Result<U, E>``` <a name="ResultTResult"></a>
In this case the second function gets called only and if only the returned value from the first function holds a value of ```T```. If the first function call returns ```E```, the call chain will be left and ```E``` will be returned. The result of this chain will be of type ```Result<U, E>```. The ```operator|()``` flattens the return type. There will be no nested ~~```std::Result<Result<U, E>, E>```~~
```c++
#include <result_type.h>


enum class Error{Fail1, Fail2};

auto mayReturnNothing(SomeType value)-> ResultType::Result<int, Error>{
    ...
}

auto needsSomething(int value)->ResultType::Result<SomeOtherType, Error>{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    ...
    using ResultType::operator|;
    // result is of type 'ResultType::Result<SomeOtherType, Error>'
    auto const result = value | mayReturnNothing | needsSomething;
    ...
}
```

### When functions take/return ```Result<std::optional<T>, E>``` <a name="WithResultoptional"></a>
The type ```Result<T, E>``` is a sum type. It can only hold either ```T``` or ```E```. Where ```T``` represents success and ```E``` an error.
This library assumes that type ```E``` is the same type for all values. If errors of different types are needed, ```std::variant<ErrorType1,..., ErrorTypeN>``` can be used. 

Using the ```operator|()``` will make sure the expression will be never return a nested Result<Result<...>, Error>. No need to unpack nested results.
 - Chaining a function, which returns ```Result<std::optional<T>, E>``` with a function that takes
    - [```Result<std::optional<T>, E>```](#ResultResult)
    - [```Result<T, E>```](#ResultResult)
    - [```std::optional<T>``` and returns ```U```](#ResultTU)
    - [```std::optional<T>``` and returns ```void```](#ResultTvoid)
    - [```std::optional<T>``` and returns ```Result<U, E>```](#ResultTResult)
    - [```std::optional<T>``` and returns ```std::optional<U>```](#ResultTU)
    - [```std::optional<T>``` and returns ```Result<std::optional<U>, E>```](#ResultTResult)
    - [```T``` and returns ```U```](#ResultTU)
    - [```T``` and returns ```void```](#ResultTvoid)
    - [```T``` and returns ```Result<U, E>```](#ResultTResult)
    - [```T``` and returns ```std::optional<U>```](#ResultTU)
    - [```T``` and returns ```Result<std::optional<U>, E>```](#ResultTResult)
 
#### Chaining a function, which returns ```Result<T, E>``` with a function that takes ```Result<T, E>``` <a name="ResultResult"></a>
In this case no special rule is applied. The function, which takes a ```Result<T, E>```, is called regardless, if the passed value holds a value or an error. The result of this chain will be of the same type as the return type of the second function. Which seems obvious, but in later cases it will be important to pay attention to this.
```c++
#include <result_type.h>


enum class Error{Fail1, Fail2};

auto mayReturnError(SomeType value)-> ResultType<int, Error>{
    ...
}

auto canDealWithErrors(ResultType<int, Error> value)->SomeOtherType{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    ...
    using ResultType::operator|;
    // result is of type 'SomeOtherType'
    auto const result = value | mayReturnError | canDealWithErrors;
    ...
}
```
#### Chaining a function, which returns ```Result<T, E>``` with a function that takes ```T``` and returns ```U``` <a name="ResultTU"></a>
In this case the second function gets called only and if only the returned value from the first function holds a ```T```. If the first function call returns a value holding ```E```, ```E``` will be returned for the whole expression. The result of this chain will be of type ```Result<U, E>```.
```c++
#include <result_type.h>


enum class Error{Fail1, Fail2};

auto mayReturnNothing(SomeType value)-> Result<int, Error>{
    ...
}

auto needsSomething(int value)->SomeOtherType{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    ...
    using ResultType::operator|;
    // result is of type 'Result<SomeOtherType, Error>'
    auto const result = value | mayReturnNothing | needsSomething;
    ...
}
```

#### Chaining a function, which returns ```Result<T, E>``` with a function that takes ```T``` and returns ```void``` <a name="ResultTvoid"></a>
In this case the second function gets called only and if only the returned value from the first function holds a value of ```T```. If it holds ```E```  the expression returns this error. The result of this chain will be of type ```ResultType::Result<ResultType::NothingType, E>```.
```c++
#include <result_type.h>


enum class Error{Fail1, Fail2};

auto mayReturnAnError(SomeType value)-> ResultType::Result<int, Error>{
    ...
}

auto needsSomething(int value)->void{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    using ResultType::operator|;
    ...
    // result is of type 'ResultType::Result<ResultType::NothingType, Error>'
    auto const result = value | mayReturnAnError | needsSomething;
    ...
}
```

#### Chaining a function, which returns ```Result<T, E>``` with a function that takes ```T``` and returns ```Result<U, E>``` <a name="ResultTResult"></a>
In this case the second function gets called only and if only the returned value from the first function holds a value of ```T```. If the first function call returns ```E```, the call chain will be left and ```E``` will be returned. The result of this chain will be of type ```Result<U, E>```. The ```operator|()``` flattens the return type. There will be no nested ~~```std::Result<Result<U, E>, E>```~~
```c++
#include <result_type.h>


enum class Error{Fail1, Fail2};

auto mayReturnNothing(SomeType value)-> ResultType::Result<int, Error>{
    ...
}

auto needsSomething(int value)->ResultType::Result<SomeOtherType, Error>{
    ...
}

auto insideSomeOtherFunction(SomeType value){
    using ResultType::operator|;
    ...
    // result is of type 'ResultType::Result<SomeOtherType, Error>'
    auto const result = value | mayReturnNothing | needsSomething;
    ...
}
```

## External resources <a name="ExternalLinks"></a>

- https://doc.rust-lang.org/book/ch09-02-recoverable-errors-with-result.html?highlight=result#recoverable-errors-with-result
- Railway Oriented Programming by Scott Wlaschin https://fsharpforfunandprofit.com/rop/
- Functional C++ for Fun and Profit by Phil Nash https://www.youtube.com/watch?v=YgcUuYCCV14
- CppCon 2018: Andrei Alexandrescu “Expect the expected” https://www.youtube.com/watch?v=PH4WBuE1BHI
 |F78MrWv4cqjvh