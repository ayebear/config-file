// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <memory>
#include "strlib.h"

// This class can store a value of different types based on a string
class Option
{
    public:
        Option(); // Default constructor
        Option(const std::string&); // Initialize with a string value

        void reset(); // Sets all values to 0 and removes the range

        // Setting will compute all possible types
        bool operator=(const char* data);
        bool operator=(const std::string& data);
        template <typename Type> bool operator=(Type data);
        bool setString(const std::string& data);

        // Getting will simply return the precomputed values
        const std::string& toString() const;
        std::string toStringWithQuotes() const;
        int toInt() const;
        long toLong() const;
        float toFloat() const;
        double toDouble() const;
        bool toBool() const;
        char toChar() const; // Based on int

        // This will try to cast the decimal to another type
        template <typename Type> Type to() const;

        // For determining if the option was originally read in as a string with quotes
        void setQuotes(bool setting);
        bool hasQuotes();

        // For setting the valid range
        void setRange(double num1);
        void setRange(double num1, double num2);
        void removeRange();

    private:
        bool isInRange(double num);

        enum RangeType
        {
            NoRange = 0,
            MinRange,
            MinMaxRange
        };

        // The "set" function will set all of these, no matter what the type is
        std::string str;
        long number;
        double decimal;
        bool logical;

        bool quotes;

        RangeType range;
        double rangeMin;
        double rangeMax;
};

template <typename Type>
bool Option::operator=(Type data)
{
    // Only set the value if it is in range
    if (isInRange((double)data))
    {
        number = data;
        decimal = data;
        logical = (data != 0);
        str = strlib::toString<Type>(data);
        quotes = false;
        return true;
    }
    return false;
}

template <typename Type>
Type Option::to() const
{
    return static_cast<Type>(decimal);
}

// Factory functions

template <typename Type>
Option makeOption(Type data)
{
    Option tmp;
    tmp = data;
    return tmp;
}

template <typename Type>
Option makeOption(Type data, double num1)
{
    Option tmp;
    tmp = data;
    tmp.setRange(num1);
    return tmp;
}

template <typename Type>
Option makeOption(Type data, double num1, double num2)
{
    Option tmp;
    tmp = data;
    tmp.setRange(num1, num2);
    return tmp;
}

#endif
