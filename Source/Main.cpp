#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <switch.h>
#include <string>

#include <Util.hpp>
#include <ui/Menu.hpp>
#include <ui/Touch.hpp>
#include <Network.hpp>
#include <Logger.hpp>

int appInit()
{
    Result rc;

    if (R_FAILED(rc = socketInitializeDefault())) // for curl / nxlink.
        printf("socketInitializeDefault() failed: 0x%x.\n\n", rc);

#ifdef DEBUG
    if (R_FAILED(rc = nxlinkStdio())) // redirect all printout to console window.
        printf("nxlinkStdio() failed: 0x%x.\n\n", rc);
#endif

    if (R_FAILED(rc = setsysInitialize())) // for system version
        printf("setsysInitialize() failed: 0x%x.\n\n", rc);

    if (R_FAILED(rc = splInitialize())) // for atmosphere version
        printf("splInitialize() failed: 0x%x.\n\n", rc);

    if (R_FAILED(rc = plInitialize())) // for shared fonts.
        printf("plInitialize() failed: 0x%x.\n\n", rc);

    if (R_FAILED(rc = romfsInit())) // load textures from app.
        printf("romfsInit() failed: 0x%x.\n\n", rc);

    sdlInit(); // int all of sdl and start loading textures.

    romfsExit(); // exit romfs after loading sdl as we no longer need it.

    return 0;
}

void appExit()
{
    sdlExit();
    socketExit();
    plExit();
    splExit();
    setsysExit();
}

// Main entrypoint
int main()
{
    // init stuff.
    appInit();
    mkdir(APP_PATH, 0777);
    chdir(ROOT);

    refreshScreen(/*loaded=*/0);
    updateRenderer();
    // set the cursor position to 0.
    short cursor = 0;

    // touch variables.
    int touch_lock = OFF;
    u32 tch = 0;
    touchPosition touch;

    // muh loooooop
    while (appletMainLoop())
    {
        // scan for button / touch input each frame.
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        hidTouchRead(&touch, tch);
        u32 touch_count = hidTouchCount();

        if (current_screen == MAIN_SCREEN)
            printMainMenu(cursor);
        else if (current_screen == SEARCH_RESULT_SCREEN)
            printMangaSearchResults(cursor);

        // move cursor down...
        if (kDown & KEY_DOWN)
        {
            if (cursor == current_max_cursor_pos - 1)
                cursor = 0;
            else
                cursor++;
        }

        // move cursor up...
        if (kDown & KEY_UP)
        {
            if (cursor == 0)
                cursor = current_max_cursor_pos;
            else
                cursor--;
        }

        // select option
        if (kDown & KEY_A || (touch_lock == OFF && touch.px > 530 && touch.px < 1200 && touch.py > FIRST_LINE - HIGHLIGHT_BOX_MIN && touch.py < (NEWLINE * current_max_cursor_pos) + FIRST_LINE + HIGHLIGHT_BOX_MAX))
        {
            // check if the user used touch to enter this option.
            if (touch_lock == OFF && touch_count > 0)
                cursor = touch_cursor(touch.px, touch.py);

            if (current_screen == MAIN_SCREEN)
            {
                Result rc = 0;
                SwkbdConfig kbd;

                switch (cursor)
                {
                case 0:
                {
                    //Search manga
                    char searchQuery[32] = {0};
                    rc = swkbdCreate(&kbd, 0);
                    if (R_SUCCEEDED(rc))
                    {
                        swkbdConfigMakePresetDefault(&kbd);
                        swkbdConfigSetTextCheckCallback(&kbd, validate_text); //Optional, can be removed if not using TextCheck.

                        rc = swkbdShow(&kbd, searchQuery, sizeof(searchQuery));

                        if (R_SUCCEEDED(rc))
                        {
                            Log(searchQuery);
                            std::string query(searchQuery);
                            std::string url = "http://192.168.178.68:3000/search/" + query;
                            if (!downloadFile(url.c_str(), "/switch/manga-reader/last_search.json", 0))
                            {
                                Log("Should have gotten search result.");
                                current_screen = SEARCH_RESULT_SCREEN;
                            }
                        }
                        swkbdClose(&kbd);
                    }
                }
                break;
                case 1:
                {
                    //Get manga info
                    char mangaId[32] = {0};
                    rc = swkbdCreate(&kbd, 0);
                    if (R_SUCCEEDED(rc))
                    {
                        swkbdConfigMakePresetDefault(&kbd);
                        swkbdConfigSetTextCheckCallback(&kbd, validate_text); //Optional, can be removed if not using TextCheck.

                        rc = swkbdShow(&kbd, mangaId, sizeof(mangaId));

                        if (R_SUCCEEDED(rc))
                        {
                            Log(mangaId);
                            std::string id(mangaId);
                            std::string url = "http://192.168.178.68:3000/manga/" + id;
                            if (!downloadFile(url.c_str(), "/switch/manga-reader/last_manga_info.json", 0))
                            {
                                Log("Should have gotten info.");
                            }
                        }
                        swkbdClose(&kbd);
                    }
                    break;
                }
                }
            }
        }

        // exit...
        if (kDown & KEY_PLUS || (touch.px > 1145 && touch.px < SCREEN_W && touch.py > 675 && touch.py < SCREEN_H))
            break;

        // lock touch if the user has already touched the screen (touch tap).
        if (touch_count > 0)
            touch_lock = ON;
        else
            touch_lock = OFF;

        // display render buffer
        updateRenderer();
    }

    // cleanup then exit
    appExit();

    return 0;
}