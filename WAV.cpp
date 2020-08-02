#include "WAV.h"
#include <iostream>
#include <sstream>




void WAV::read() {
    doUWantSave();
    if(std::cin.rdbuf()->in_avail() < 2) std::cout << "Enter input filename: ";
    std::string s;
    std::cin >> s;
    std::cout << '\n';
    if (s[0] == '\"') s = s.substr(1, s.length() - 2);
    read(s.data());
}

void WAV::doUWantSave() {
    if (readed && !saved) {
        char command = '\0';
        while (command != 'Y' && command != 'N' && command != 'y' && command != 'n') {
            std::cout << "Do you want save changes(Y/N): ";
            std::cin >> command;
            std::cout << "\n";
        }
        if (command == 'Y' || command == 'y') write();
    }
}

void WAV::read(const char* filename) {

    doUWantSave();

    FILE* f;
    fopen_s(&f, filename, "rb");
    if (f == nullptr) {
        printf("Cannot open file.\nTry again\n");
        read();
    }

    auto check = [](char* a, const char* b, int kol) {
        for (int i = 0; i < kol; i++) {
            if (a[i] != b[i]) return true;
        }
        return false;
    };


    fread(chunkId, 1, 4, f);
    if (check(chunkId, "RIFF", 4)) exit(2);

    fread(&chunkSize, 4, 1, f);

    fread(format, 1, 4, f);
    if (check(format, "WAVE", 4)) exit(3);

    fread(subchunk1Id, 1, 4, f);
    if (check(subchunk1Id, "fmt ", 4)) exit(4);

    fread(&subchunk1Size, 4, 1, f);

    fread(&audioFormat, 2, 1, f);

    fread(&numChannels, 2, 1, f);

    fread(&sampleRate, 4, 1, f);

    fread(&byteRate, 4, 1, f);

    fread(&blockAlign, 2, 1, f);

    fread(&bitsPerSample, 2, 1, f);
    bytesPerSample = bitsPerSample / 8;

    char temp;

    if (subchunk1Size != 16 && audioFormat != 1) { // not PCM
        fread(&extraParamSize, 2, 1, f);
        for (int i = 0; i < extraParamSize; i++) {
            fread(&temp, 1, 1, f);
            extraParam.push_back(temp);
        }

    }
    fread(subchunk2Id, 1, 4, f);

    if (!check(subchunk2Id, "LIST", 4)) {
        listId = true;
        fread(&listSize, 4, 1, f);
        for (unsigned int i = 0; i < listSize; i++) {
            fread(&temp, 1, 1, f);
            listParam.push_back(temp);
        }
        fread(subchunk2Id, 1, 4, f);
    }

    if (!check(subchunk2Id, "data", 4)) {
        fread(&subchunk2Size, 4, 1, f);
    } else exit(5);

    for (int i = 0; i < numChannels; i++)
        data.push_back(std::vector<char>());
    for (unsigned long readed = 0; readed < subchunk2Size; readed += bytesPerSample * numChannels)
        for (int j = 0; j < numChannels; j++) 
            for (int i = 0; i < bytesPerSample; i++) {
                fread(&temp, 1, 1, f);
                data[j].push_back(temp);
            }

    fclose(f);
    readed = true;
    std::cout << "Succsesfull read\n\n";
}

void WAV::write(const char* filename) {

    FILE* f;
    fopen_s(&f, filename, "wb");
    if (f == nullptr) {
        printf("Cannot open file.\nTry again\n");
        write();
    }

    fwrite(chunkId, 1, 4, f);

    fwrite(&chunkSize, 4, 1, f);

    fwrite(format, 1, 4, f);

    fwrite(subchunk1Id, 1, 4, f);

    fwrite(&subchunk1Size, 4, 1, f);

    fwrite(&audioFormat, 2, 1, f);

    fwrite(&numChannels, 2, 1, f);

    fwrite(&sampleRate, 4, 1, f);

    fwrite(&byteRate, 4, 1, f);

    fwrite(&blockAlign, 2, 1, f);

    fwrite(&bitsPerSample, 2, 1, f);


    if (subchunk1Size != 16 && audioFormat != 1) {
        fwrite(&extraParamSize, 2, 1, f);
        for (int i = 0; i < extraParamSize; i++) 
            fwrite(&extraParam[i], 1, 1, f);
    }

    if (listId) {
        fwrite("LIST", 1, 4, f);

        fwrite(&listSize, 4, 1, f);

        for (unsigned int i = 0; i < listParam.size(); i++)
            fwrite(&listParam[i], 1, 1, f);
    }

    fwrite(subchunk2Id, 1, 4, f);

    fwrite(&subchunk2Size, 4, 1, f);

    for (unsigned long writed = 0; writed < data[0].size(); writed += bytesPerSample)
        for (int j = 0; j < numChannels; j++)
            for (short i = 0; i < bytesPerSample; i++) {
                fwrite(&data[j][writed + i], 1, 1, f);
            }

    fclose(f);
    std::cout << "Succsesfull write\n\n";
    saved = true;
}

void WAV::write(){
    if (std::cin.peek() == '\n') std::cout << "Enter output filename: ";
    std::string s;
    std::cin >> s;
    std::cout << '\n';
    if (s[0] == '\"') s = s.substr(1, s.length() - 2);
    write(s.data());
}

void WAV::cut() {

    if (!readed) {
        std::cout << "Warning:first read the WAV file.\n\n";
        read();
    }

    if (std::cin.rdbuf()->in_avail() < 3) std::cout << "Duration of the audio file is " << outDuration() << "\n\n";

    unsigned short start, end;
    std::string st, en;

    auto parseInt = [](std::string s) {
        int i = s.length() - 1, res = 0, k = 1;
        while (i >= 0) {
            res += (s[i--] - '0') * k;
            k *= 10;
        }
        return res;
    };

    auto parse = [parseInt](std::string s) {
        unsigned int i = 0;
        while (i < s.length() && s[i] != '.') i++;
        if (i < s.length() && s[i] == '.') return parseInt(s.substr(0, i)) * 60 + 
            parseInt(s.substr(i + 1, 2));
        return parseInt(s);
    };

    auto f = [&start, &end, &st, &en, parse](std::string s) {
        if(std::cin.peek() == '\n') std::cout << s << "Enter start point: ";
        std::cin >> st;
        start = parse(st);
        if (std::cin.peek() == '\n') std::cout << "\nEnter end point: ";
        std::cin >> en;
        end = parse(en);
        std::cout << '\n';
    };


    f("");

    while(start >= end || start < 0 || end > subchunk2Size / byteRate)
        f("Please enter 2 numbers in the range from 0 to file length (the first number must be greater than the second)\n\n");
    for (int i = 0; i < numChannels; i++) {
        data[i].erase(data[i].begin(), data[i].begin() + start * byteRate / numChannels);
        data[i].erase(data[i].begin() + (end - start) * byteRate / numChannels, data[i].end());
        data[i].shrink_to_fit();
    }
    chunkSize -= subchunk2Size - (end - start) * byteRate;
    subchunk2Size = (end - start) * byteRate;

    std::cout << "Succsesfull cut\n\nNew Duration is " << outDuration() << "\n\n";
    saved = false;
}

std::string WAV::outDuration()
{
   return (std::stringstream() << subchunk2Size / byteRate / 60 << "." << subchunk2Size / byteRate - subchunk2Size / byteRate / 60 * 60).str();
}

WAV::~WAV(){
    doUWantSave();
}
