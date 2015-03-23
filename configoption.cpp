// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "configoption.h"

namespace cfg
{

Option::OptionVector Option::emptyVector;

Option::Option(const std::string& data)
{
    setString(data);
}

Option::Option(const Option& data)
{
    operator=(data);
}

void Option::reset()
{
    quotes = false;
    range = NoRange;
    options.reset();
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

Option& Option::operator=(const Option& data)
{
    str = data.str;
    number = data.number;
    decimal = data.decimal;
    logical = data.logical;
    quotes = data.quotes;
    range = data.range;
    rangeMin = data.rangeMin;
    rangeMax = data.rangeMax;
    if (data.options)
        options = std::make_unique<OptionVector>(*data.options);
    else
        options.reset();
    return *this;
}

bool Option::setString(const std::string& data)
{
    std::istringstream stream(data);
    double value{};

    // Try to parse a value from the string
    bool success = (stream >> value) && (static_cast<size_t>(stream.gcount()) == data.size());

    // Only set the value if it's in range
    if (isInRange(value))
    {
        decimal = value; // Set the decimal from the temporary one
        number = decimal; // Truncate to an int
        quotes = !(success || strlib::isBool(data)); // No quotes around a number or a boolean
        logical = (success ? (decimal != 0) : strlib::strToBool(data)); // Convert to a boolean
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

Option::operator const std::string&() const
{
    return str;
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

void Option::removeRange()
{
    range = NoRange;
}

Option& Option::push(const Option& opt)
{
    if (!options)
        options = std::make_unique<OptionVector>();
    options->push_back(opt);
    return options->back();
}

void Option::pop()
{
    if (options && !options->empty())
        options->pop_back();
}

Option& Option::operator[](unsigned pos)
{
    return ((*options)[pos]);
}

Option& Option::back()
{
    return options->back();
}

unsigned Option::size() const
{
    return (options ? options->size() : 0);
}

void Option::clear()
{
    options.reset();
}

Option::OptionVector::iterator Option::begin()
{
    if (options)
        return options->begin();
    return emptyVector.begin();
}

Option::OptionVector::iterator Option::end()
{
    if (options)
        return options->end();
    return emptyVector.end();
}

Option::OptionVector::const_iterator Option::cbegin() const
{
    if (options)
        return options->cbegin();
    return emptyVector.cbegin();
}

Option::OptionVector::const_iterator Option::cend() const
{
    if (options)
        return options->cend();
    return emptyVector.cend();
}

std::string Option::buildArrayString(const std::string& indentStr) const
{
    // Continue building array strings until the option is just a single element and not an array
    if (options)
    {
        std::string nextIndentStr(indentStr + '\t');
        // Build the array string
        std::string arrayStr("{\n");
        unsigned arraySize = options->size();
        for (unsigned i = 0; i < arraySize; ++i)
        {
            arrayStr += nextIndentStr;
            arrayStr += (*options)[i].buildArrayString(nextIndentStr);
            if (i < arraySize - 1)
                arrayStr += ",\n";
        }
        arrayStr += '\n' + indentStr + '}';
        return arrayStr;
    }
    else
        return toStringWithQuotes();
}

bool Option::isInRange(double num)
{
    return ((range == NoRange) || // If there is no range, then it will always be in range
            (range == MinRange && num >= rangeMin) || // If a minimum range is set, make sure the number is >= the minimum range
            (range == MinMaxRange && num >= rangeMin && num <= rangeMax)); // If a full range is set, make sure the number is between the minimum and maximum numbers
}

std::ostream& operator<<(std::ostream& stream, const Option& option)
{
    stream << option.toString();
    return stream;
}

}
