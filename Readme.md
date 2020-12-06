![imcalc](https://user-images.githubusercontent.com/20073598/101269513-90fd8a80-373d-11eb-9e91-d71f30afb137.gif)

**ImCalc**

ImCalc is an [ImGui](https://github.com/ocornut/imgui) graphing calculator with infinite precision. Be careful with the exponents!

**usage**
double multiplication (**) is evaluated to power/exponent x^n, x<sup>n</sup>

**integer math is assumed when no decimal is provided on either operand. some functions will automatically return real numbers**
input                 output 
10/3                     (3)
10/3.0         (3.333333333)

variable declarations available
input                 output 
myvar = 6                (6)
myvar / 3                (2)

boolean evaluation and comparison operators available

input                 output 
2 <= 6                (true)
true and false       (false)
true or false         (true)


ee_dll.dll is an external math library that I wrote for a project, unfortunately I cannot release the source for this since it is
the answer for the project..

This project was inspired by my friend WerWolv who made a very nice app called [ImHex](https://github.com/WerWolv/ImHex). He inspired to check out
[ImGui](https://github.com/ocornut/imgui). Shout out to him. PS. [ImHex](https://github.com/WerWolv/ImHex) is sweet go check it out! 

Building:

should be able to just open solution and build. default is a 64 bit version of the dll, however there is a 32 bit version included. just rename ee_dll##.dll to ee_dll.dll

for linux libraries. add rename libee_dll##.so to libee_dll.so and link it. Also ensure that you add the library to LD_LIBRARY_PATH for you to be able to run it.
