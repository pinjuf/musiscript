#ifndef CONFIG_H
#define CONFIG_H

// Files
#define OUTFILE "out.wav"
#define VOICE_SUFFIX ".msc"
#define PREPROCESSED_VOICE_SUFFIX ".mscpp"
#define DATA_DIR "data"

// Logging stuff
#define DEF_LOG_LEVEL LOG_INFO
#define DO_LOGLINENUMBER false // Settings to true will STRONGLY slow down the program
#define DO_COLOR_LOG true

// RNG stuff, set to true to always have the same seed
#define CONSTANT_SRAND false

// Number to string precision
#define DIGIT_PREC 32


#endif
