# exercise.md

The following are a list of questions about the design of our math library and also further your C++/general programming knowledge.

1. In C++ what are the three instances where you would use the keyword [const](https://en.cppreference.com/w/cpp/keyword/const)?
2. What does [inline](https://en.cppreference.com/w/cpp/language/inline) do? Why might a programmer use it?
3. Why do you think libraries like GLM are implemented as a header only library? 
	- (Sometimes you will see this, and you may research [.inl files](https://stackoverflow.com/questions/1208028/significance-of-a-inl-file-in-c) in C++)
4. C++ allows for something known as 'operator overloading'. This means we can define (or redefine) how operators like +,-,/,etc. work--but this means additional symbols like `<<`. Would it be helpful to implement the '<<' operator for a mat4 type? (i.e. `operator<<`)? What might you use this for? Does the GLM library overload the '<<' operator? (i.e., `std::cout << some_mat4`)
5. For the floating-point data type, would it be dangerous to overload the operator for '=='?
	- How might you mitigate this? [reference](https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/)
	- The question I am really asking is, why is it a problem to compare two floating point numbers?
6. What is a template in C++? Is the GLM library using templates?
7. Do a little research on your own for this one. How closely named are the functions in GLM versus the GL Shading Language(GLSL) (i.e., Was the GLM library designed to closely match GLSL?)
8. Do you feel comfortable working with pointers? (Yes/No) -- If not -- here's a video: https://www.youtube.com/watch?v=2R5cjpi9Fzw

You may add your notes here:

1. Three instances where you use const are for constant variables [const int], constant pointers [const int*], and constant functions [void PPM::savePPM(std::string outputFileName) const {]
2. Inline suggests to the compiler to replace the call to a function with the code of the function. A programmer might choose to use this to eliminate the overhead associated with a function call like jumping to the address of the function or pushing to the stack.
3. The header only files in the libraries like GLM are added to the source files at compile time which eliminates the need for a precompiled binary file. This means dll's and other binary linking can be avoided, reducing build steps.
4. Mat4 is a floating point 4x4 matrix. Implementing << for this type would mean being able to print matrices to std::cout. This is helpful for visual purposes and can help with debugging. GLM doesn't overload the << operator. 
5. Floating point numbers that have the same mathematical value don't always have the same binary representation, which means comparing these values won't be accurate. One solution is to use Epsilon comparisons (bool isEqual = fabs(f1 – f2) <= epsilon;). This introduces error bounds within which we can determine two floating point numbers as equal. The epsilon is relative according to the difference between the numbers are their magnitudes.
6. Templates in C++ allow us to make generic classes/functions. This means that they can be instantiated with any data type. It also ensures type safety because the compiler will ensure that operations will be valid for the type at compile time. GLM uses templates to define types like matrices to work with many different data types (float, int, etc.).
7. There are many similarities between the GLM library and GLSL, and it was designed to closely match so that developers would be comfortable transitioning from GLSL to GLM. They follow the same naming conventions (both have types like mat4). 
8. Yes
