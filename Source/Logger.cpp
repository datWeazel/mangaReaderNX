#include <stdio.h>
#include <string>

void Log(std::string szString)
{
    FILE *pFile = fopen("/switch/manga-reader/logFile.txt", "a");
    fprintf(pFile, "%s\n", szString.c_str());
    fclose(pFile);
}