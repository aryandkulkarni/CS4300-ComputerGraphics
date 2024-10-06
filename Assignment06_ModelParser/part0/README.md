## Part 0 - The C++ std::vector data structure (C++ refresh)

Before we jump into coding, I want to start with a programming refresh on a useful data structure in C++, the [std::vector](https://en.cppreference.com/w/cpp/container/vector).

### Task 1

Provided in this section is a dynamic data structure that will be useful for storing geometric data. The Standard Template Library(STL) provides [std::vector](http://www.cplusplus.com/reference/vector/vector/). It is an expanding data structure that we can push (i.e. add data) data into, and access each element in constant time (i.e., O(1)). Provided below is an example for working with a [std::vector](http://www.cplusplus.com/reference/vector/vector/) and a custom data type.

**Historically I have noted that a portion of folks (10-15%) completely skip reading this part and never find out why vector.data() is useful**: For those who have taken CS 3520 (Programming in C++), that is probably okay if you are comfortable with std::vector--but if you don't know what 'std::vector.data()' does, or why 'sizeof(std::vector) is not the same as std::vector.length()' then you really need to read this example. 

```cpp
// This is a 'toy' example of using the std::vector
// Compile with:
// g++ -std=c++20 vector.cpp -o prog
// Run with:
// ./prog

#include <iostream>
#include <vector>	// Our new library

// Here is a struct that holds
// the indices that make up a triangle.
// Note that we are using a 'struct' as a 
// 'Plain-old datatype' or POD, to hold
// some information. No fancy constructors or
// destructors needed since we are only 
// storing primitive data (i.e., ints, floats, char, etc.)
// types in each field.
struct Triangle{
    unsigned int i1,i2,i3;
};

// Here is a std::vector that holds all of the indices.
// Since we are creating it outside of our main,
// it is a global variable (i.e., it is availablle everywhere).
// NOTE: This is a toy example--but I want you to pay attention
// 	 that this is allocated in some globally accessible form of memory.
// 	 Generally we do not like global variables.
std::vector<unsigned int> indiciesList;

// Entry function into our C++ program.
int main(){

    // This local variable triangleList holds all of the triangles
    // for our model. We are allocating it on the stack here.
    // Remember, things allocated on the stack are 'deleted' 
    // when we exit the function from which they were allocated
    // (in this case, the main() function).
    std::vector<Triangle> triangleList;

    // We create a first triangle. It will be made up of
    // whatever 'vertex' 0, 1, and 2 are. 
    Triangle t;
    t.i1=0;
    t.i2=1;
    t.i3=2;
	
    // Here is a second triangle. This time it is made up
    // of wherever triangle 2, 1, and 3 are.
    Triangle t1;
    t1.i1=2;
    t1.i2=1;
    t1.i3=3;
    
    // And a third triangle...
    Triangle t2;
    t2.i1=2;
    t2.i2=1;
    t2.i3=3;

    // We now 'append' each of our triangles to our triangle list.
    triangleList.push_back(t);
    triangleList.push_back(t1);
    triangleList.push_back(t2);

    // We can print out some information about each of our vectors.
    // This tells us how many triangles we have.
    std::cout << "triangleList.size():" << triangleList.size() << "\n";
    
    // Here is another example.
    // Careful, this tells us the size of a data type.
    // Do you understand what triangList.data() is giving us access to? Look it up!
    std::cout << "sizeof(triangleList.data()):" << sizeof(triangleList.data()) << "\n";
    // Here we print out the size of each 'data type' NOT how much data we have
    // stored within them.
    std::cout << "sizeof(triangleList):" << sizeof(triangleList) << "\n";
    std::cout << "sizeof(Triangle):" << sizeof(Triangle) << "\n";

    // Here is an array with 12 elements, each element that is 4 bytes.
    // I am also allocating it on the stack, which costs me 48 bytes.
    unsigned int indices[] = {0,1,2, 0,3,4, 7,8,9,10,11,12};

    // Let's also add to our 'global indiciesList' some things.
    // Remember, indiciesList is on the heap.
    indiciesList.push_back(0);
    indiciesList.push_back(1);
    indiciesList.push_back(2);
    indiciesList.push_back(0);
    indiciesList.push_back(3);

    // Arrays allocated on the stack will tell me exactly how big the structure is.
    // That is because the sizeof operator has the exact information needed available
    // to compute the size when a data structure is in local scope (i.e., stack allocated)
    std::cout << "sizeof(indices) on stack:" << sizeof(indices) << "\n";
    
    // sizeof here only tells me the size of a 'std::vector' type. Be careful--so this means
    // that anything you declare global, or otherwise declare on the *heap* (i.e., using 'new')
    // will not work properly with the sizeof operator.
    std::cout << "sizeof(indiciesList)     :" << sizeof(indiciesList) << "\n";

    return 0;
}
```

**Why is the above example important?** 

For two reasons:

1. [std::vector](http://www.cplusplus.com/reference/vector/vector/) is a nice data structure to assist in implementing your model loader (see next task)
2. **I often have seen students make a mistake** when figuring out how much memory to allocate on the GPU end (i.e. when you start using [glBufferData](http://docs.gl/gl3/glBufferData)) and using the **sizeof** operator incorrectly. Please pay special attention to this. I hope the above example at least provides the insight that it is worth printing out how much memory you have allocated.

### Task 2 - Program arguments

In order to speed up iteration time, it can be useful to read in data from program arguments. For our .obj loader (see next part), we will load a file from the command line arguments. Here is an example in C++ for reading in program arguments in C++.

```cpp
// Compile with: g++ -std=c++20 args.cpp -o args
// Run with    : ./prog ./../../common/objects/bunny_centered.obj
//
// Debug with  : g++ -std=c++17 -g -Wall -Werror args.cpp -o args
// Debug with  : gdb ./prog --tui

#include <iostream>
#include <string>

int main(int argc, char** argv){

    // Warn the user they need to specify at least one argument
    // We detect this by checking the size of the argument count (argc)
    if(argc < 2){
        std::cout << "Please specify a .obj model" << std::endl;
        return 0;
    }

    // The first or 0th argument is the program name
    std::cout << "Program name is argv[0: " << argv[0] << "\n";
    // This is the next argument, thus the filepath
    std::cout << "My next argument is: " << argv[1] << "\n";
    // Now I'll convert the filepath to a string, and you can use
    // as needed.
    std::string modelFilePath(argv[1]);
    // Then load some file by
    // passing in modelFilePath or modelFilePath.c_str() as an
    // argument.

    return 0;
}
```

### Task 3 - Parsing Strings

One tricky part of C++ can be parsing data. That is, taking in some formatted (or unformatted) data and trying to store the results in a data structure. For this assignment, you are going to have to parse some string data. Below is one example of how you may do so using a std::stringstream.

```cpp
// Compile with: g++ -g -std=c++20 stringstream.cpp -o prog
// Run with    : ./prog
#include <iostream> // input/output functions
#include <string>   // std::string data type
#include <sstream>  // std::stringstream
#include <vector>   // std::vector


struct Vertex{
    float x,y,z;
};

struct Triangle{
    unsigned int i1,i2,i3;
};

int main(){
    // Here is an example of some data you may
    // want to read in.
    // Note: The backslashes at the end just 
    //       continue the line.
    // Note: \n creates a newline
    // Note: The 'weird' indentation is 
    //       intentional for this example.
    std::string somedata = "\
v 1.00 2.00 3.00 \n\
v 4.00 5.00 6.00 \n\
v -7.0 -8.0 -9.0 \n\
f 1//1 2//2 3//3 \n\
                            ";

    // Let's just print out our data to see
    // what our data looks like.
    std::cout << "Print our data:\n" 
              << somedata
              << std::endl;

    // So let's try to make sense of how to
    // read those values into a data structure.
    // We're going to need to store vertex and
    // triangle data somewhere, and we do not
    // know how many of each of these elements
    // we'll have--thus a dynamic data structure
    // like std::vector will be useful.
    std::vector<Vertex> vertices; 
    std::vector<Triangle> triangles; 

    // We're going to read in one line at
    // a time from our 'somedata' string
    // So below we'll create a stringstream
    // and populatet it with the 'somedata'
    // of the string.
    // Then we're going to read in one 
    // 'token' at a time from our stream
    // and print it out.
    std::stringstream stream(somedata);    
    std::string token;
    while(stream >> token){
        std::cout << "token: " << token << std::endl;
        if(token[0] == 'v'){
            std::cout << "\tvertex data incoming\n";
        }else if(token[0] == 'f'){
            std::cout << "\tface/triangle data incoming\n";
        }
    }

    // Now, pragmatically, you are more likely
    // to be reading in a 'line' of text from
    // a file one line at a time.
    // For that, you can use std::getline to
    // read from a specific file.
    // Then you can use stringstream to break
    // that line into tokens.
    //
    // Hopefully this gives you a few ideas
    // on parsing.
}
```
