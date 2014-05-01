// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef CFG_FILE_H
#define CFG_FILE_H

#include <map>
#include <vector>
#include <string>
#include "configoption.h"

namespace cfg
{

/*
A class for reading/writing configuration files.
See README.md for more information.

TODO:
    Handle escape codes inside of strings.
    Handle multiple array elements on the same line.
    Support sub-sections by using a tree, similar to Option arrays.
    Writing should optionally preserve formatting and comments.
*/
class File
{
    public:
        enum Flags
        {
            NoFlags = 0,
            Warnings = 1, // Print messages when options are out of range
            Errors = 2,   // Print errors when loading/saving files
            Autosave = 4, // Automatically save the last file loaded on destruction
            AllFlags = 7
        };

        // Types used to store the options
        using Section = std::map<std::string, Option>;
        using ConfigMap = std::map<std::string, Section>;

        // Constructors
        File();
        File(const std::string& filename, int newFlags = NoFlags);
        File(const ConfigMap& defaultOptions, int newFlags = NoFlags);
        File(const std::string& filename, const ConfigMap& defaultOptions, int newFlags = NoFlags);
        ~File();

        // Loading/saving
        bool loadFromFile(const std::string& filename); // Loads options from a file
        void loadFromString(const std::string& str); // Loads options from a string
        bool writeToFile(std::string filename = "") const; // Saves current options to a file (default is last loaded)
        void writeToString(std::string& str) const; // Saves current options to a string (same format as writeToFile)
        std::string buildString() const; // Returns a string of the current options (same format as writeToFile)
        explicit operator bool() const; // Returns true if the last file loaded/saved successfully

        // Settings
        void setFlag(int flag, bool state = true);
        void setFlags(int newFlags = NoFlags); // Set warnings, autosave, etc.

        // Accessing/modifying options
        Option& operator()(const std::string& name, const std::string& section); // Returns a reference to an option with the specified name (and section). If it does not exist, it will be automatically created
        Option& operator()(const std::string& name); // Same as above but uses the current section
        bool optionExists(const std::string& name, const std::string& section) const; // Returns true if an option exists
        bool optionExists(const std::string& name) const; // Returns true if an option exists
        void setDefaultOptions(const ConfigMap& defaultOptions); // Sets initial values in the map from another map in memory
        ConfigMap::iterator begin(); // Returns an iterator to the beginning of the map
        ConfigMap::iterator end(); // Returns an iterator to the end of the map

        // Accessing/modifying sections
        void useSection(const std::string& section = ""); // Sets the default current section to be used
        Section& getSection(const std::string& section); // Returns a reference to a section
        Section& getSection(); // Returns a reference to the default section
        bool sectionExists(const std::string& section) const; // Returns true if a section exists
        bool sectionExists() const; // Returns true if a section exists

        // Erasing options/sections
        bool eraseOption(const std::string& name, const std::string& section); // Erases an option, returns true if the option was successfully erased
        bool eraseOption(const std::string& name); // Erases an option from the default section
        bool eraseSection(const std::string& section); // Erases a section, returns true if the section was successfully erased
        bool eraseSection(); // Erases the default section
        void clear(); // Clears all of the sections and options in memory, but keeps the filename

    private:
        enum class Comment
        {
            None,   // No comment
            Single, // Single line comment
            Start,  // Start of multiple line comment
            End     // End of multiple line comment
        };

        // File parsing
        void parseLines(std::vector<std::string>& lines); // Processes the lines in memory and adds them to the options map
        bool isSection(const std::string& section) const; // Returns true if the line is a section header
        void parseSectionLine(const std::string& line, std::string& section); // Processes a section header line and adds a section to the map
        void parseOptionLine(const std::string& line, const std::string& section); // Processes an option line and adds an option to the map
        //void setOption(const std::string& section, const std::string& name, const std::string& value); // Adds or sets an option
        bool setOption(Option& option, const std::string& value); // Sets an existing option
        Option& getArrayOption(const std::string& section, const std::string& name); // Returns an option at the current array level
        void startArray(Option& option); // Starts another array when "{" is found

        // Comment handling
        bool isEndComment(const std::string& str) const; // Returns true if it contains a multiple-line end comment symbol
        Comment getCommentType(const std::string& str, bool checkEnd = false) const; // Returns an enum value of the comment type
        Comment stripComments(std::string& str, bool checkEnd = false); // Removes all comments from a string

        // Objects/variables
        ConfigMap options; // The data structure for storing all of the options in memory
        std::string configFilename; // The filename of the config file to read/write to
        std::string currentSection; // The default current section
        int flags; // Flag bits are stored in here
        mutable bool fileIoSuccessful;

        // Array related objects
        std::vector<unsigned> currentArrayStack; // Stack of array indices for current option
        std::string arrayOptionName; // Name of option whose array is currently being handled
};

}

#endif
