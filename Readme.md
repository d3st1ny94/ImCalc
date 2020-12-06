![imcalc](https://user-images.githubusercontent.com/20073598/101269513-90fd8a80-373d-11eb-9e91-d71f30afb137.gif)

**ImCalc**

ImCalc is an [ImGui](https://github.com/ocornut/imgui) graphing calculator with infinite precision. Be careful with the exponents!

double multiplication (**) is evaluated to power/exponent x^n, x<sup>n</sup>

ee_dll.dll is an external math library that I wrote for a project, unfortunately I cannot release the source for this since it is
the answer for the project..

This project was inspired by my friend WerWolv who made a very nice app called [ImHex](https://github.com/WerWolv/ImHex). He inspired to check out
[ImGui](https://github.com/ocornut/imgui). Shout out to him. PS. [ImHex](https://github.com/WerWolv/ImHex) is sweet go check it out! 

Building:

should be able to just open solution and build.

for linux libraries. add rename libee_dll##.so to libee_dll.so and link it. Also ensure that you add the library to LD_LIBRARY_PATH for you to be able to run it.
