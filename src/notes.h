#ifndef NOTES_H
#define NOTES_H

#define A4_FREQ 440
#define HALFTONE_STEP pow(2, 1.0f/12)

double get_freq_by_name(const char * name, int transpose = 0);

#endif
