#include <stdio.h>
#include <switch.h>
#include <fstream>
#include <string>
#include <cerrno>

#include <Util.hpp>
#include <ui/Menu.hpp>
#include <nlohmann/json.hpp>

#define TEMP_FILE "/switch/manga-reader/temp"
#define FILTER_STRING "browser_download_url\":\""
#define VERSION_FILTER_STRING "tag_name\":\""

void copyFile(char *src, char *dest)
{
    FILE *srcfile = fopen(src, "rb");
    FILE *newfile = fopen(dest, "wb");

    if (srcfile && newfile)
    {
        char buffer[10000]; // 10kb per write, which is fast
        size_t bytes;       // size of the file to write (10kb or filesize max)

        while (0 < (bytes = fread(buffer, 1, sizeof(buffer), srcfile)))
        {
            fwrite(buffer, 1, bytes, newfile);
        }
    }
    fclose(srcfile);
    fclose(newfile);
}

int parseSearch(char *parse_string, char *filter, char *new_string)
{
    FILE *fp = fopen(parse_string, "r");

    if (fp)
    {
        char c;
        while ((c = fgetc(fp)) != EOF)
        {
            if (c == *filter)
            {
                for (int i = 0, len = strlen(filter) - 1; c == filter[i]; i++)
                {
                    c = fgetc(fp);
                    if (i == len)
                    {
                        for (int j = 0; c != '\"'; j++)
                        {
                            new_string[j] = c;
                            new_string[j + 1] = '\0';
                            c = fgetc(fp);
                        }
                        fclose(fp);
                        remove(parse_string);
                        return 0;
                    }
                }
            }
        }
    }

    errorBox(350, 250, "Failed to find parse url!");
    fclose(fp);
    return 1;
}

std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return (contents);
    }
    return ("error");
    //throw(errno);
}

static void from_json(const nlohmann::json &j, mangaRating &rating)
{
    j.at("value").get_to(rating.value);
    j.at("votes").get_to(rating.votes);
}

void from_json(const nlohmann::json &j, manga &manga)
{
    j.at("id").get_to(manga.id);
    j.at("title").get_to(manga.title);
    j.at("image_url").get_to(manga.image_url);
    j.at("description").get_to(manga.description);
    j.at("views").get_to(manga.views);
    j.at("follows").get_to(manga.follows);
    j.at("rating").get_to(manga.rating);
    j.at("lang_name").get_to(manga.lang_name);
}