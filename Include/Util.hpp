#pragma once

#include <string>
#include <nlohmann/json.hpp>

#define ROOT "/"
#define APP_PATH "/switch/manga-reader/"

#define YES 10
#define NO 20
#define ON 1
#define OFF 0

void copyFile(char *src, char *dest);                                // basic copy file. Use malloc if you need dynamic mem
int parseSearch(char *phare_string, char *filter, char *new_string); // hacky way to parse a file for a string
std::string get_file_contents(const char *filename);

struct mangaRating
{
    double value;
    int votes;
};

struct manga
{
    int id;
    std::string title;
    std::string image_url;
    std::string description;
    int views;
    int follows;
    mangaRating rating;
    std::string lang_name;
};
static void from_json(const nlohmann::json &j, mangaRating &rating);
void from_json(const nlohmann::json &j, manga &manga);