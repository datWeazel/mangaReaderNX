#pragma once

#include <ui/SDL.hpp>

#define HIGHLIGHT_BOX_MIN 20
#define HIGHLIGHT_BOX_MAX 70
#define NEWLINE 110
#define FIRST_LINE 130
#define MAX_STRLEN 512

#define MAIN_SCREEN 0
#define SEARCH_RESULT_SCREEN 1
#define MANGA_INFO_SCREEN 2

extern int current_screen;

SwkbdTextCheckResult validate_text(char *tmp_string, size_t tmp_string_size);

// The rest of these callbacks are for swkbd-inline.
void finishinit_cb(void);

void strchange_cb(const char *str, SwkbdChangedStringArg *arg);   // String changed callback.
void movedcursor_cb(const char *str, SwkbdMovedCursorArg *arg);   // Moved cursor callback.
void decidedenter_cb(const char *str, SwkbdDecidedEnterArg *arg); // Text submitted callback, this is used to get the output text from submit.
void decidedcancel_cb(void);

void refreshScreen(char loaded); // clear screen, display background with title
void printMainMenu(int cursor);  // print list of options
void printMangaSearchResults(int cursor);
void popUpBox(TTF_Font *font, int x, int y, SDL_Colour colour, char *text); // pop-up box in center of screen
int yesNoBox(int mode, int x, int y, char *question);                       // box with yes or no option
void errorBox(int x, int y, char *errorText);                               // used for displaying error messages
