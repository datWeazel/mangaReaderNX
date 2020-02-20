#include <unistd.h>
#include <switch.h>
#include <string>
#include <fstream>

#include <Util.hpp>
#include <Logger.hpp>
#include <ui/Touch.hpp>
#include <ui/Menu.hpp>
#include <nlohmann/json.hpp>

#define APP_VERSION "Weazel Reader: 0.0.1"

// TextCheck callback, this can be removed when not using TextCheck.
SwkbdTextCheckResult validate_text(char *tmp_string, size_t tmp_string_size)
{
    if (strcmp(tmp_string, "bad") == 0)
    {
        strncpy(tmp_string, "Bad string.", tmp_string_size);
        return SwkbdTextCheckResult_Bad;
    }

    return SwkbdTextCheckResult_OK;
}

// The rest of these callbacks are for swkbd-inline.
void finishinit_cb(void)
{
    Log("reply: FinishedInitialize\n");
}

// String changed callback.
void strchange_cb(const char *string, SwkbdChangedStringArg *arg)
{
    //std::string str(string);
    //Log("reply: ChangedString. str = " + string + ", arg->stringLen = " + std::to_string(arg->stringLen) + ", arg->dicStartCursorPos = " + std::to_string(arg->dicStartCursorPos) + ", arg->dicEndCursorPos = " + std::to_string(arg->dicEndCursorPos) + ", arg->arg->cursorPos =" + std::to_string(arg->cursorPos));
}

// Moved cursor callback.
void movedcursor_cb(const char *string, SwkbdMovedCursorArg *arg)
{
    //Log("reply: MovedCursor. str = " + std::to_string(string) + ", arg->stringLen = " + std::to_string(arg->stringLen) + ", arg->cursorPos = " + std::to_string(arg->cursorPos));
}

// Text submitted callback, this is used to get the output text from submit.
void decidedenter_cb(const char *string, SwkbdDecidedEnterArg *arg)
{
    //Log("reply: DecidedEnter. str = " + std::to_string(string) + ", arg->stringLen = " + std::to_string(arg->stringLen));
}

void decidedcancel_cb(void)
{
    //Log("reply: DecidedCancel\n");
}

void refreshScreen(char loaded)
{
    clearRenderer();

    // app version.
    drawText(fntMedium, 40, 40, SDL_GetColour(white), APP_VERSION);

    // system version.
    //drawText(fntSmall, 25, 150, SDL_GetColour(white), getSysVersion());

    // atmosphere version.
    //drawText(fntSmall, 25, 230, SDL_GetColour(white), getAmsVersion());

    if (loaded)
    {
        // write the latest version number, if an update is available
        //drawText(fntSmall, 25, 260, SDL_GetColour(white), getLatestAtmosphereVersion());

        //drawText(fntMedium, 120, 225, SDL_GetColour(white), "Menu Here"); // menu options
        drawButton(fntButton, BUTTON_A, 970, 672, SDL_GetColour(white));
        drawText(fntSmall, 1010, 675, SDL_GetColour(white), "Select");

        drawButton(fntButton, BUTTON_PLUS, 1145, 672, SDL_GetColour(white));
        drawText(fntSmall, 1185, 675, SDL_GetColour(white), "Exit");
    }
}

void printMainMenu(int cursor)
{
    refreshScreen(/*loaded =*/1);

    char *option_list[] = {"Search Manga",
                           "Manga Info"};

    char *description_list[] = {"Searches for manga on mangadex.",
                                "Gets manga info by mangadex-id."};

    SDL_Texture *textureArray[] = {ams_icon, ams_plus_icon, hekate_icon, app_icon, reboot_icon};

    for (int i = 0, nl = 0; i < (CURSOR_LIST_MAX); i++, nl += NEWLINE)
    {
        if (cursor != i)
            drawText(fntSmall, 550, FIRST_LINE + nl, SDL_GetColour(white), option_list[i]);
        else
        {
            // icon for the option selected.
            drawImage(textureArray[i], 125, 350, 0.0);
            // highlight box.
            drawShape(SDL_GetColour(dark_blue), 530, (FIRST_LINE + nl - HIGHLIGHT_BOX_MIN), 700, HIGHLIGHT_BOX_MAX);
            // option text.
            drawText(fntSmall, 550, FIRST_LINE + nl, SDL_GetColour(jordy_blue), option_list[i]);
            // description.
            drawText(fntSmall, 25, 675, SDL_GetColour(white), description_list[i]);
        }
    }
}

void printMangaSearchResults(int cursor)
{
    refreshScreen(/*loaded =*/1);

    std::ifstream jsonFile("/switch/manga-reader/last_search.json");
    nlohmann::json mangas;
    jsonFile >> mangas;

    Log("Number of mangas: " + std::to_string(mangas.size()));

    //TODO:: Parse json, build option_list, display options
    std::string op1 = "Search Manga (# of last results: " + std::to_string(mangas.size()) + ")";
    const char *option_list[] = {op1.c_str(),
                                 "Manga Info"};

    char *description_list[] = {"Searches for manga on mangadex.",
                                "Gets manga info by mangadex-id."};

    SDL_Texture *textureArray[] = {ams_icon, ams_plus_icon, hekate_icon, app_icon, reboot_icon};

    for (int i = 0, nl = 0; i < (CURSOR_LIST_MAX); i++, nl += NEWLINE)
    {
        if (cursor != i)
            drawText(fntSmall, 550, FIRST_LINE + nl, SDL_GetColour(white), option_list[i]);
        else
        {
            // icon for the option selected.
            drawImage(textureArray[i], 125, 350, 0.0);
            // highlight box.
            drawShape(SDL_GetColour(dark_blue), 530, (FIRST_LINE + nl - HIGHLIGHT_BOX_MIN), 700, HIGHLIGHT_BOX_MAX);
            // option text.
            drawText(fntSmall, 550, FIRST_LINE + nl, SDL_GetColour(jordy_blue), option_list[i]);
            // description.
            drawText(fntSmall, 25, 675, SDL_GetColour(white), description_list[i]);
        }
    }
}

void popUpBox(TTF_Font *font, int x, int y, SDL_Colour colour, char *text)
{
    // outline. box
    drawShape(SDL_GetColour(black), (SCREEN_W / 4) - 5, (SCREEN_H / 4) - 5, (SCREEN_W / 2) + 10, (SCREEN_H / 2) + 10);
    // popup box.
    drawShape(SDL_GetColour(dark_blue), SCREEN_W / 4, SCREEN_H / 4, SCREEN_W / 2, SCREEN_H / 2);
    // text to draw.
    drawText(font, x, y, colour, text);
}

int yesNoBox(int mode, int x, int y, char *question)
{
    //printOptionList(mode);
    popUpBox(fntMedium, x, y, SDL_GetColour(white), question);
    // highlight box.
    drawShape(SDL_GetColour(faint_blue), 380, 410, 175, 65);
    drawShape(SDL_GetColour(faint_blue), 700, 410, 190, 65);
    // option text.
    drawButton(fntButtonBig, BUTTON_B, 410, 425, SDL_GetColour(white));
    drawText(fntMedium, 455, 425, SDL_GetColour(white), "No");
    drawButton(fntButtonBig, BUTTON_A, 725, 425, SDL_GetColour(white));
    drawText(fntMedium, 770, 425, SDL_GetColour(white), "Yes");

    updateRenderer();

    int res = 0;
    int touch_lock = OFF;
    touchPosition touch;
    u32 tch = 0;
    u32 touch_count = hidTouchCount();

    // check if the user is already touching the screen.
    if (touch_count > 0)
        touch_lock = ON;

    while (1)
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        hidTouchRead(&touch, tch);
        touch_count = hidTouchCount();

        if (touch_count == 0)
            touch_lock = OFF;

        if (touch_count > 0 && touch_lock == OFF)
            res = touch_yes_no_option(touch.px, touch.py);

        if (kDown & KEY_A || res == YES)
            return YES;

        if (kDown & KEY_B || res == NO)
            return NO;
    }
}

void errorBox(int x, int y, char *errorText)
{
    popUpBox(fntMedium, x, y, SDL_GetColour(white), errorText);
    drawImageScale(error_icon, 570, 340, 128, 128, 0.0);
    updateRenderer();

    sleep(3);
}

int touch_cursor(int x, int y)
{
    int cursor = 0;

    for (int nl = 0; cursor < (CURSOR_LIST_MAX + 1); cursor++, nl += NEWLINE)
        if (y > (FIRST_LINE + nl - HIGHLIGHT_BOX_MIN) && y < (FIRST_LINE + nl + NEWLINE - HIGHLIGHT_BOX_MIN))
            break;

    return cursor;
}

int touch_yes_no_option(int x, int y)
{
    if (x > 380 && x < 555 && y > 410 && y < 475)
        return NO;

    if (x > 700 && x < 890 && y > 410 && y < 475)
        return YES;

    return 1;
}