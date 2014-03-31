ConfigFile
==========

This C++11 class reads simple configuration files, which can be used in all kinds of software. You can even modify and save configuration files, or you can simply use it to read user settings.

The main purpose of this project is to have a simple file format which can be used extremely easily in code, and so that users can easily modify these files without worrying about a lot of syntax. This is a very loose format, which ignores whitespace, and has dynamic data-types.

The file format is like this:
```
someOption = someValue
```
The left side is the name of the option, and the right is its value. The equals symbol obviously represents that the option is being set to that value.

Example file
------------

```
[ExampleSection]

// Numeric types:
someNumber = 500
eulersNumber = 2.718281828459045

// Booleans:
someBoolean = true
anotherBool = FALSE
// The values of booleans are not case sensitive.

// Booleans can also be in numeric form.
// Zero is false, any non-zero number is true.
bool1 = 0
bool2 = 100

// Strings:
name = "Qwerty"
color = "Blue"

// Option names are case sensitive:
number = 42
NUMBER = 250
// These are two different values.
NUMBER = 99999
// Options of the same name will rewrite the values of previous options.
// number is 42
// NUMBER is 99999

// Arrays:
myArray = {
    "Some text.",
    12345,
    {
        27.52,
        "Inside another array",
        true
    },
    "Back to outer array"
}
// Arrays are jagged and fully dynamic, please refer to the "Arrays" section for more information.

[Section2]

// Each section has a different "scope"
someNumber = 1000
```

Features (file format)
----------------------

* Options can be named almost anything, except starting with a comment symbol or section symbol.
* The options can be any of these types:
  * Integers (-1, 0, 1, 2)
  * Decimals (0.0123, 99.765, -346.2)
  * Booleans (true, false, TRUE, FALSE, 0, 1)
  * Strings ("Insert text here")
    * See Strings description for more details.
  * Arrays ({element1, element2, element3})
    * See Arrays description for more details.
* Comments and sections are also supported.

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

Strings
-------

NOTE: This is changing in the near future, all strings must have quotes and use proper escape codes for special characters such as newlines, whitespace, etc.

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

Arrays
------

More information will be here in the next few days.

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

Example usage of classes
------------------------

You can create cfg::File objects, which can load/save configuration files. Loading one will actually keep an in-memory std::map of all of the options, so accessing/changing/creating options in your program will be fast. Then, if you want to, you can write the changes back into the same file or into a new file.

### Loading/saving configuration files

#### Loading files
```
// First, you will need to include the header file:
#include "file.h"
// or
#include "configfile/file.h"
// depending on if you have set the include directory in your IDE/compiler.

// Load a configuration file:
cfg::File config("sample.cfg");
// Or you can load it after you construct the object:
cfg::File config;
config.loadFromFile("sample.cfg");
```

#### Saving files
```
// Save to a file:
cfg::File config;
/* ... */
config.writeToFile("saved.cfg");

// Another way is to just save the last loaded file:
cfg::File config("sample.cfg");
/* ... */
config.writeToFile();
```

#### Loading with default options
```
// You can also specify default options:
const cfg::File::ConfigMap defaultOptions = {
{"ExampleSection", {
    {"someOption", cfg::makeOption(false)},
    {"pi", cfg::makeOption(3.14159265358979)},
    {"name", cfg::makeOption("Test")},
    {"percent", cfg::makeOption(0, 0, 100)}
}}};
// Load a file using those default options:
cfg::File config("sample.cfg", defaultOptions);
// If any of those options do not exist in the file, the defaults will be used.
```

### Manipulating options

#### Option ranges

Notice in the previous example, there were three parameters used in cfg::makeOption(). The parameters are as follows:

* Default value (required)
* Minimum value (optional)
* Maximum value (optional)

Whenever a value in the configuration file is loaded or an option is being set in your program, it will only be set if the value is within that range. Assuming the above code is included, here is an example:

```
config.useSection("ExampleSection"); // Use the correct section
config("percent") = 50; // OK, value is in range
config("percent") = 9000; // Not set, because value is out of range
// "percent" ends up as 50
```

#### Reading values
```
// Read an option as an int:
int someNumber = config("someNumber").toInt();
// Notice the .toInt() above. This is because using operator() returns
// a reference to a cfg::Option object, which can be read as different types.
// Some other types that it can be read as:
std::string str = config("someString").toString();
double dec = config("someDouble").toDouble();
bool someBool = config("someBool").toBool();
// You can also get the option as almost any value that can cast from a double:
auto someValue = config("someOption").to<unsigned short>();
```

#### Setting values
```
// Modifying options:
config("someNumber") = 200;
// Options can be set to values of different types:
config("someNumber") = 3.14159;
config("someNumber") = "Some string";
config("someNumber") = true;
```

#### Accessing options with sections
```
// Sections:
// The 2nd parameter of operator() is the section to use:
config("test", "NewSection") = 5;
// You can alternatively set the current section to use by default:
config.useSection("NewSection");
// So that this section will be used when nothing is specified:
config("test") = 5;
// Both will set "test" in "NewSection" to 5.
```

For more ways of using the ConfigFile and Option classes, please refer to the header files.
