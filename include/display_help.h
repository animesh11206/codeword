#ifndef DISPLAY_HELP_H
#define DISPLAY_HELP_H

#include <string>

extern const std::string AUTHOR;
extern const std::string VERSION;

void display_help(const std::string& prog_name = "codeword");
void display_version();

inline void Display_Help(const std::string& prog_name = "codeword") {
    display_help(prog_name);
}

inline void Display_Version() {
    display_version();
}

#endif // DISPLAY_HELP_H
