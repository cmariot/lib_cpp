# libftpp

Further Exploration into Advanced C++

## Summary

This subject aims to introduce you to advanced C++ concepts through the development of complex tools and systems.

## Contents

- I Objectives — p. 2
- II General rules — p. 3
- III Mandatory part — p. 5
	- III.1 General structure of your library — p. 5
	- III.2 Data Structures — p. 6
	- III.3 Design Patterns — p. 8
	- III.4 IOStream — p. 11
	- III.5 Thread — p. 12
	- III.6 Network — p. 14
	- III.7 Mathematics — p. 16
- IV Bonuses — p. 19
- V Submission and peer-evaluation — p. 20

## Chapter I — Objectives

In this cutting-edge subject, you will embark on an extraordinary journey into the depths of C++ to craft a toolbox that will be your companion for the rest of your C++ projects! Gone are the days of relying solely on pre-built libraries and frameworks. Now it’s time to take control, build tools tailored to your unique challenges, and uphold the highest standards of code quality.

As you explore design patterns like Singleton and Observer, you’ll start to see how the architecture of effective software unfold before you.

You’ll dive into thread-safe data structures, pushing your multi-threading capabilities to new heights.

Imagine writing network code so robust that it could handle streaming the entire internet.

Have complex mathematical algorithms ever seemed daunting? Fear not! You’ll create your own customizable vector classes and random number generators, fine-tuned to your specific needs—proving to yourself that you’re more than capable.

This toolbox won’t just be a set of classes and methods; it will be a testament to your skill, creativity, and your mastery of one of the most powerful programming languages to date.

So gear up for an incredible adventure, because by the end, you won’t just have a toolbox — you’ll have an arsenal of skills that will empower you to take on any software project in the future.

## Chapter II — General rules

### Compiling

- Compile your code with `c++` and the flags `-Wall -Wextra -Werror`.
- Your code must compile with at least the flag `-std=c++11`.

Yes, you must use C++11 or later.

### Formatting and naming conventions

- Name your files, classes, functions, member functions, and attributes as outlined in the guidelines.
- Write class names in PascalCase format. Write method names in camelCase format. Files containing class code should always be named in snake_case according to the class name. For instance:

```
class_name.hpp / class_name.h
class_name.cpp
class_name.tpp
```

So, if you have a header file containing the definition of a class “BrickWall” representing a brick wall, the file name will be `brick_wall.hpp`.

- Unless specified otherwise, every output message must end with a new-line character and be displayed to the standard output.

- Goodbye Norminette! No coding style is enforced in the C++ modules. You can follow your preferred style. However, remember that if your code is difficult for your peer evaluators to understand, they won’t be able to grade it. So, do your best to write clean and readable code.

### Allowed / Forbidden

You are not coding in C anymore. It’s time to C++! Therefore:

You are allowed to use almost everything from the standard library, from at least C++11 up to the newest version of C++. So, instead of sticking to what you
already know, it would be wise to use the C++ versions of the C functions you are accustomed to, as much as possible.

However, you cannot use any external libraries beyond the standard library. This means libraries such as Boost are forbidden. The following functions are also prohibited: *printf(), *alloc(), and free(). Using any of these will be result in an automatic 0 for your grade, no exceptions.

A few design requirements:
- Memory leakage occurs in C++ as well. When you allocate memory using the new keyword, you must ensure you avoid memory leaks.
- Any function implementation placed in a header file (except for function templates) will result in a grade of 0 for that exercise.
- Each of your headers should be usable independently of the others. Thus, they must include all necessary dependencies. However, you must prevent the issue of double inclusion by adding include guards. Failure to do so will result in a grade of 0.

Read me
- You may add additional files if needed (e.g., to organize your code). Since these assignments are not automatically verified by a program, you are free to do so as long as you submit the mandatory files.
- You will need to implement many classes. This may seem tedious unless you can script your favorite text editor.

You have a certain amount of freedom in completing the exercises.
However, follow the mandatory rules and don’t be lazy—you would miss out on a lot of valuable information! Don’t hesitate to read up on theoretical concepts.


## Chapter III — Mandatory part

1. General structure of your library

In this project, your ultimate goal is to build a comprehensive C++ toolbox that will serve you well in your future projects. To ensure that your toolbox is both portable and easy to integrate, you are required to structure your project as follows:

- Makefile: Your project should include a Makefile that, when executed, produces a static library named libftpp.a. This library should bundle all the functionalities you implement throughout this project. Your Makefile must compile libftpp.a using the -Wall -Wextra -Werror flags.
- Header File: You must provide a unified header file named libftpp.hpp, which includes all the necessary headers from your toolbox. This will enable other developers (or your future self) to easily integrate your library into new projects by including just this single header file.
- Organization: Apart from these essential components, you are free to organize your codebase as you see fit. Creativity and good structure are encouraged but not strictly enforced. You’re free to organize sources and headers however you prefer, the choice is yours.
