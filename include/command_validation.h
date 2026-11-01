#ifndef COMMAND_VALIDATION_H
#define COMMAND_VALIDATION_H

#include <string>

extern const std::string ALL_FLAGS[];
extern const std::string UTILITY_FLAGS[];
extern const std::string ENCODING_FLAGS[];
extern const std::string DECODING_FLAGS[];
extern const std::string HASHING_FLAGS[];

extern const int NUM_ALL_FLAGS;
extern const int NUM_ENCODING_FLAGS;
extern const int NUM_DECODING_FLAGS;
extern const int NUM_HASHING_FLAGS;

extern std::string operations[];
extern std::string used_operation_flags[]; 

extern int max_used_encoding_flags;
extern int max_used_decoding_flags;
extern int max_used_hashing_flags;
extern int num_operation_flags_used;

bool Command_Validation(int argc, char* argv[]);

#endif
