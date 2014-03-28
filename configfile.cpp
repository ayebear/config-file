// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "configfile.h"
#include <fstream>
#include <iostream>
#include "strlib.h"

ConfigFile::ConfigFile()
{
    resetSettings();
}

ConfigFile::ConfigFile(const string& filename, bool warnings)
{
    resetSettings();
    showWarnings = warnings;
    loadFromFile(filename);
}

ConfigFile::ConfigFile(const ConfigMap& defaultOptions, bool warnings)
{
    resetSettings();
    showWarnings = warnings;
    setDefaultOptions(defaultOptions);
}

ConfigFile::ConfigFile(const string& filename, const ConfigMap& defaultOptions, bool warnings)
{
    resetSettings();
    showWarnings = warnings;
    setDefaultOptions(defaultOptions);
    loadFromFile(filename);
}

ConfigFile::~ConfigFile()
{
    if (autosave)
        writeToFile();
}

bool ConfigFile::loadFromFile(const string& filename)
{
    bool status = false;
    configFilename = filename;
    std::vector<string> lines;
    if (strlib::readLinesFromFile(configFilename, lines, false))
    {
        parseLines(lines);
        status = true;
    }
    else if (showWarnings)
        std::cout << "Error loading \"" << configFilename << "\"\n";
    return status;
}

void ConfigFile::loadFromString(const string& str)
{
    std::vector<string> lines;
    strlib::getLinesFromString(str, lines, false);
    parseLines(lines);
}

bool ConfigFile::writeToFile(string filename) const
{
    if (filename.empty())
        filename = configFilename;
    // Write the string to the output file
    return strlib::writeStringToFile(filename, buildString());
}

void ConfigFile::writeToString(string& str) const
{
    for (const auto& section: options) // Go through all of the sections
    {
        if (!section.first.empty())
            str += '[' + section.first + "]\n"; // Add the section line if it is not blank
        for (const auto& o: section.second) // Go through all of the options in this section
            str += o.first + " = " + o.second.toStringWithQuotes() + '\n'; // Include the original quotes if any
        str += '\n';
    }
    if (!str.empty() && str.back() == '\n')
        str.pop_back(); // Strip the extra new line at the end
}

string ConfigFile::buildString() const
{
    string configStr;
    writeToString(configStr);
    return configStr;
}

void ConfigFile::setShowWarnings(bool setting)
{
    showWarnings = setting;
}

void ConfigFile::setAutosave(bool setting)
{
    autosave = setting;
}

void ConfigFile::resetSettings()
{
    showWarnings = false;
    autosave = false;
}

Option& ConfigFile::operator()(const string& name, const string& section)
{
    return options[section][name];
}

Option& ConfigFile::operator()(const string& name)
{
    return options[currentSection][name];
}

bool ConfigFile::optionExists(const string& name, const string& section) const
{
    auto sectionFound = options.find(section);
    return (sectionFound != options.end() && sectionFound->second.find(name) != sectionFound->second.end());
}

bool ConfigFile::optionExists(const string& name) const
{
    return optionExists(name, currentSection);
}

void ConfigFile::setDefaultOptions(const ConfigMap& defaultOptions)
{
    options.insert(defaultOptions.begin(), defaultOptions.end());
}

void ConfigFile::useSection(const string& section)
{
    currentSection = section;
}

ConfigFile::ConfigMap::iterator ConfigFile::begin()
{
    return options.begin();
}

ConfigFile::ConfigMap::iterator ConfigFile::end()
{
    return options.end();
}

ConfigFile::Section& ConfigFile::getSection(const string& section)
{
    return options[section];
}

ConfigFile::Section& ConfigFile::getSection()
{
    return options[currentSection];
}

bool ConfigFile::sectionExists(const string& section) const
{
    return (options.find(section) != options.end());
}

bool ConfigFile::sectionExists() const
{
    return sectionExists(currentSection);
}

bool ConfigFile::eraseOption(const string& name, const string& section)
{
    bool status = false;
    auto sectionFound = options.find(section);
    if (sectionFound != options.end()) // If the section exists
        status = (sectionFound->second.erase(name) > 0); // Erase the option
    return status;
}

bool ConfigFile::eraseOption(const string& name)
{
    return eraseOption(name, currentSection);
}

bool ConfigFile::eraseSection(const string& section)
{
    return (options.erase(section) > 0);
}

bool ConfigFile::eraseSection()
{
    return eraseSection(currentSection);
}

void ConfigFile::clear()
{
    options.clear();
}

void ConfigFile::parseLines(std::vector<string>& lines)
{
    string section = "";
    bool multiLineComment = false;
    Comment commentType = Comment::None;
    for (string& line: lines) // Iterate through the std::vector of strings
    {
        strlib::trimWhitespace(line);
        commentType = stripComments(line, multiLineComment);

        if (commentType == Comment::Start)
            multiLineComment = true;

        if (!multiLineComment && !line.empty()) // Don't process a comment or an empty line
        {
            if (isSection(line))
                parseSectionLine(line, section); // Example: "[Section]"
            else
                parseOptionLine(line, section); // Example: "Option = Value"
        }

        if (commentType == Comment::End)
            multiLineComment = false;
    }
}

bool ConfigFile::isSection(const string& section) const
{
    return (section.size() >= 2 && section.front() == '[' && section.back() == ']');
}

void ConfigFile::parseSectionLine(const string& line, string& section)
{
    section = line.substr(1, line.size() - 2); // Set the current section
    options[section]; // Add that section to the map
}

void ConfigFile::parseOptionLine(const string& line, const string& section)
{
    size_t equalPos = line.find("="); // Find the position of the "=" symbol
    if (equalPos != string::npos && equalPos >= 1) // Ignore the line if there is no "=" symbol
    {
        string name, value;
        // Extract the name and value
        name = line.substr(0, equalPos);
        value = line.substr(equalPos + 1);
        // Trim any whitespace around the name and value
        strlib::trimWhitespace(name);
        strlib::trimWhitespace(value);
        // Remove outer quotes if any
        bool trimmedQuotes = strlib::trimQuotes(value);
        // Add/update the option in memory
        Option& option = options[section][name];
        bool optionSet = (option = value);
        if (trimmedQuotes) // If quotes were removed
            option.setQuotes(true); // Add quotes to the option
        if (showWarnings && !optionSet)
            std::cout << "Warning: Option \"" << name << "\" was out of range. Using default value: " << option.toStringWithQuotes() << std::endl;
    }
}

bool ConfigFile::isEndComment(const string& str) const
{
    return (str.find("*/") != string::npos);
}

ConfigFile::Comment ConfigFile::getCommentType(const string& str, bool checkEnd) const
{
    // Comment symbols and types
    static const std::vector<string> commentSymbols = {"/*", "//", "#", "::", ";"};
    static const std::vector<Comment> commentTypes = {Comment::Start, Comment::Single, Comment::Single, Comment::Single, Comment::Single};

    Comment commentType = Comment::None;

    // This is optional so the function returns the correct result
    if (checkEnd && isEndComment(str))
        commentType = Comment::End;

    // Find out which comment type the string is, if any
    for (unsigned int i = 0; (commentType == Comment::None && i < commentSymbols.size()); i++)
    {
        if (str.compare(0, commentSymbols[i].size(), commentSymbols[i]) == 0)
            commentType = commentTypes[i];
    }

    // This check is required for comments using the multi-line symbols on a single line
    if (!checkEnd && commentType == Comment::Start && isEndComment(str))
        commentType = Comment::Single;

    return commentType;
}

ConfigFile::Comment ConfigFile::stripComments(string& str, bool checkEnd)
{
    Comment commentType = getCommentType(str, checkEnd);
    if (commentType == Comment::Single)
        str.clear();
    return commentType;
}

std::vector<string> ConfigFile::splitArrayString(string str) const
{
    // There must be quotes in the array to determine what the elements are
    // Example format: {"1", "2", "test"}
    std::vector<string> vec;
    if (str.size() >= 2 && str.front() == '{' && str.back() == '}')
    {
        str.pop_back();
        str.erase(str.begin());
    }
    strlib::split(str, ",", vec);
    for (auto& elem: vec)
    {
        strlib::trimWhitespace(elem);
        strlib::trimQuotes(elem);
    }
    return vec;
}

string ConfigFile::joinArrayString(const std::vector<string>& vec) const
{
    // Join a vector back together into a properly formatted string
    string str = "{";
    for (auto& elem: vec)
        str += elem + ",";
    if (str.back() == ',')
        str.pop_back();
    str += "}";
    return str;
}
