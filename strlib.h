// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef STRLIB_H
#define STRLIB_H

#include <string>
#include <sstream>
#include <vector>

namespace strlib
{

// String manipulation
void trimWhitespace(std::string&); // Trims all whitespace on both sides of the string
bool trimQuotes(std::string&); // Trims quotes on ends of string, returns true if the string was modified
void stripNewLines(std::string&); // Removes all new lines/carriage returns from a string
unsigned replaceAll(std::string&, const std::string&, const std::string&); // Replaces all instances of a sub-string with another string, and returns the number of replaces
void split(const std::string&, const std::string&, std::vector<std::string>&, bool = true); // Splits a string into a vector of strings with a delimeter
std::string toLower(std::string); // Creates an all-lowercase version of the passed in string
bool areQuotes(char, char); // Returns true if both characters are either single or double quotes
bool mustEndWith(std::string&, const std::string&); // Appends the second string if the first doesn't end with it

// File operations
void getLinesFromString(std::string, std::vector<std::string>&, bool = true); // Splits a string into separate lines with CR and/or LF characters
bool readLinesFromFile(const std::string&, std::vector<std::string>&, bool = true); // Simply reads a file into memory in separate lines
bool writeStringToFile(const std::string&, const std::string&); // Writes a string to a file, will overwrite an existing file

// String converting
bool strToBool(const std::string&); // Parses a string to determine its boolean value
bool isBool(const std::string&); // Determines if a string is either "true" or "false"

// Converts most types to strings using a string stream
template <typename T>
std::string toString(T data, unsigned precision = 16)
{
    std::ostringstream tmp;
    tmp.precision(precision);
    tmp << data;
    return tmp.str();
}

// Converts a bool to a string
template <typename T>
std::string toString(bool data)
{
    return (data ? "true" : "false");
}

}

#endif
