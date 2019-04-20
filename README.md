# CppResultType
This a header only for a type of Result<T,E>.

Its inspired by Result<T,E> in Rust, but much simpler.
No methods like map, and_then, ... .


With an overloaded operator (in this case operator|()) it should be more convenient to chain function calls in a readable way without omitting error handling.

This implementation assumes that all passed and returned types of Result<T,E> in the call chain use the same error type E.

