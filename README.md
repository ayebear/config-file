ConfigFile
==========

This C++11 class reads simple configuration files, which can be used in all kinds of software. You can even modify and save configuration files, or you can simply use it to read user settings.

The main purpose of this project is to have a simple file format which can be used extremely easily in code, and so that users can easily modify these files without worrying about a lot of syntax. This is a very loose format, which ignores whitespace, and has dynamic data-types.

The file format is like this:
```
someOption = someValue
```
The left side is the name of the option, and the right is its value. The equals symbol obviously represents that the option is being set to that value.

Features (file format)
----------------------

* Options can be named almost anything, except starting with a comment symbol or section symbol.
* The options can be any of these types:
  * Integer
  * Float
  * Boolean
  * String
    * See Strings description for more details.
* Comments and sections are also supported.
* Simple arrays are supported if stored in strings. Better support will be available in the future.

Features (class)
----------------

* Can load/save from/to either a file or string
  * Can automatically save the file on object destruction
* Contains an in-memory std::map of the options
* Can use operator() to easily access/create/modify options
* Supports using a std::map to load default options from
  * This means the user isn't required to make a configuration file, and can just add the options they wish to change.
* You can check if an option exists before trying to access it (which would just create a new one)
* Has begin() and end() iterators to allow iterating through the sections and options with a range based for loop
* Can erase options and sections
* Option class:
  * Supports using a default value and valid range
    * If the option is being set to a value out of range, it won't be set
  * Supports setting/getting as all of the types listed in file format features
  * All type conversion is done on set, so that accessing is always fast

Example usage of class
----------------------

You can create ConfigFile objects, which can load/save configuration files. Loading one will actually keep an in-memory std::map of all of the options, so accessing/changing/creating options in your program will be fast. Then, if you want to, you can write the changes back into the same file or into a new file.

```
ConfigFile config("sample.cfg"); // Loads a configuration file
int someNumber = config("someNumber").toInt(); // Read an option as an int
// Notice the .toInt() above. This is because using operator[] returns
// a reference to an Option object, which can be read as different types.
auto someValue = config("someOption").to<unsigned short>();
// You can get the option as almost any value that can cast from a double
config("someNumber") = 200; // You can modify values like this
config("someNumber") = "Some string"; // You can even set the value to a different type!
// Sections:
config("test", "NewSection") = 5; // You can specify the section to use as the 2nd parameter
config.useSection("NewSection"); // You can alternatively set the current section
config("test") = 5; // So that this section will be used when nothing is specified
```

For more ways of using the ConfigFile and Option classes, please refer to the header files.

Strings
-------

How strings are handled:
```
someString =    this is a test
```
This string is interpreted as "this is a test". It ignores all whitespace "around" the data.
It is recommended to use quotes for strings, so that it is interpreted as you expect:
```
someString = "    this is a test"
```
Symbols in strings work fine too, including quotes:
```
str = "!@#$%^&*()"""""""_+-="
```
The first and last quote are used for determining what is contained in the string, so there is no need to escape characters.

Sections
--------

* You can also have "sections", which are optional to use with code that uses this class.
* If you use sections in your code, then they must be specified correctly, in order for options
    to be distinguished from other options in other sections.
* If no sections are used, a default section is used which is just an empty string.
  * This also applies to options in the beginning of the file before any section headers.
  * Specifying an empty string section with "[]" would have the same behavior as using no section.
  * You can even use "" as a section in your code which refers to the default section.
    * Notice that this is used as the default value for the section string parameters.

Sections example:
```
[SomeSection]
option = value
anotherOption = 123

[AnotherSection]
option = 5000
```
So "option" will be read as "value" in SomeSection, but 5000 in AnotherSection.
Also, "anotherOption" only exists in SomeSection, so you will not get its value with AnotherSection.

Comments
--------

* Comments can be made with most symbols (//, #, ::, and ;)
* Comments MUST be on their own lines
  * The symbols are checked only in the beginning of each line
  * The whole line is ignored if it is a comment
* Multi-line comments are also supported with /* and */

Other notes
-----------

* Whitespace is ignored around setting names.
* Semicolons CANNOT be used to separate lines, only new lines can.

Example file
------------

```
[ExampleSection]

someNumber = 500

// Decimals are stored as double precision floats:
someDecimal = 3.1415926535

// Booleans are not case sensitive:
someBoolean = True
anotherBool = false
bool3 = FALSE

// Zero is false, any non-zero value is true:
bool4 = 0
bool5 = 100

[Section2]

// Remember that each section has its own scope
someNumber = 1000
```
