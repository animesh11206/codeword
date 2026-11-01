#ifndef OUTPUT_FILE_VALIDATION_H
#define OUTPUT_FILE_VALIDATION_H

#include <string>

extern bool append_output;
extern bool output_file_creation;
extern std::string output_file;

bool Output_File_Validation(const std::string& output_file);

#endif