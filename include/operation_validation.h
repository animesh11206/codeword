#ifndef OPERATION_VALIDATION_H
#define OPERATION_VALIDATION_H

#include <string>

extern std::string used_operation_flags[]; 
extern int num_operation_flags_used;

bool Operation_Validation(int argc, char* argv[]);

#endif