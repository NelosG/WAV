#include <vector>
#include <string>

class WAV {
private:
    char chunkId[4];
    unsigned int chunkSize;
    char format[4];
    char subchunk1Id[4];
    unsigned int subchunk1Size;
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned int sampleRate;
    unsigned int byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    unsigned short bytesPerSample;

    unsigned short extraParamSize;
    std::vector<char> extraParam;
    bool listId;
    unsigned int listSize;
    std::vector<char> listParam;

    char subchunk2Id[4];
    unsigned int subchunk2Size;
    std::vector<std::vector<char>> data;

    bool readed = false;
    bool saved = true;

public:
    void read();
    void doUWantSave();
    void read(const char* filename);
    void write(const char* filename);
    void write();
    void cut();
    std::string outDuration();
    ~WAV();

    WAV(const char *filename) {
        read(filename);
    }

    WAV() {}
};

