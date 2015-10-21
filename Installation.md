# How To Install #

First, you should download the latest version of Cistron from the Downloads section.

In addition, you also need the boost library. This library can be found on http://www.boost.org/. Installation instructions can be found on the site. You do not need to compile the boost library to use Cistron, because it only uses the header-only parts of boost. Make sure the headers are in the include path of the compiler you are using, so that Cistron can include them.

Cistron is platform-independent and only depends on boost for its implementation. This means that every project can use Cistron by just compiling the three source files (`ObjectManager.cpp`, `Object.cpp` and `Component.cpp`) and linking your project to them. To use the framework, just include `Cistron.h`. Note that the framework is made available in its own namespace `Cistron`.

In Visual Studio, it is the easiest to copy all the files (except for `Main.cpp`!)to your project directory and add them to your own project in Visual Studio.