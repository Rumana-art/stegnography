#ifndef COMMON_H
#define COMMON_H

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"
void print_section(const char *title);

// Function to print progress bar
void print_progress(long current, long total);

#endif
