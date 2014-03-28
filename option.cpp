// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "option.h"

Option::Option():
    str(""),
    number(0),
    decimal(0),
    logical(false),
    quotes(false),
    range(NoRange)
{
}

Option::Option(const std::string& data)
{
    quotes = false;
    range = NoRange;
    operator=(0);
    setString(data);
}

void Option::reset()
{
    range = NoRange;
    operator=(0);
}

bool Option::operator=(const char* data)
{
    return setString(data);
}

bool Option::operator=(const std::string& data)
{
    return setString(data);
}

bool Option::setString(const std::string& data)
{
    std::istringstream tmpStream(data);
    double tmpDec = 0;
    bool parsedNumber = (tmpStream >> tmpDec); // Try reading a number from the string
    if (isInRange(tmpDec)) // Check if the number is in range
    {
        decimal = tmpDec; // Set the decimal from the temporary one
        number = decimal; // Truncate to an int
        quotes = !parsedNumber; // No quotes around a number
        logical = (parsedNumber ? (decimal != 0) : strlib::strToBool(data)); // Convert to a boolean
        str = data; // Set the string
        return true;
    }
    return false;
}

const std::string& Option::toString() const
{
    return str;
}

std::string Option::toStringWithQuotes() const
{
    // Automatically append quotes to the string if it originally had them
    return (quotes ? ('"' + str + '"') : str);
}

int Option::toInt() const
{
    return number;
}

long Option::toLong() const
{
    return static_cast<long>(number);
}

float Option::toFloat() const
{
    return static_cast<float>(decimal);
}

double Option::toDouble() const
{
    return decimal;
}

bool Option::toBool() const
{
    return logical;
}

char Option::toChar() const
{
    return static_cast<char>(number);
}

void Option::setQuotes(bool setting)
{
    quotes = setting;
}

bool Option::hasQuotes()
{
    return quotes;
}

void Option::setRange(double num1)
{
    rangeMin = num1;
    range = MinRange;
}

void Option::setRange(double num1, double num2)
{
    rangeMin = num1;
    rangeMax = num2;
    range = MinMaxRange;
}

void Option::removeRange() { range = NoRange; }

bool Option::isInRange(double num)
{
    return ((range == NoRange) || // If there is no range, then it will always be in range
            (range == MinRange && num >= rangeMin) || // If a minimum range is set, make sure the number is >= the minimum range
            (range == MinMaxRange && num >= rangeMin && num <= rangeMax)); // If a full range is set, make sure the number is between the minimum and maximum numbers
}
