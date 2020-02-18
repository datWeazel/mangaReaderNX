#pragma once

#define ROOT "/"
#define APP_PATH "/switch/manga-reader/"

#define YES 10
#define NO 20
#define ON 1
#define OFF 0

void copyFile(char *src, char *dest);                                // basic copy file. Use malloc if you need dynamic mem
int parseSearch(char *phare_string, char *filter, char *new_string); // hacky way to parse a file for a string