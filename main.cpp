#include <iostream> // cout, endl, hex, uppercase
#include <iomanip> // setfill, setw
#include <string.h> // strcmp
#include <vector> // std::vector
#include <assert.h> // assert()
#include <fstream> // Better file reading

// ASM cheat sheet: https://github.com/oowekyala/arm-cheatsheet/blob/master/arm-cheatsheet.pdf
// Memory location cheat sheet: https://www.smwcentral.net/?p=viewthread&t=31813
// String notes:
// - 253/0xFD is a null terminator, it signals the end of the string
// - 0xFE/254 is the start of a line. It can follow with two things:
//   - 0x10/16 0x00/0 for a new line
//   - 0x00/0  0x00/0 for not a new line, usually only at the start

// Constants
const int FILE_HEADER_OFFSET = 160; // 0x000000A0 = 160
const char FILE_HEADER[] = "SUPER MARIOCA3"; // Hex at location converts to ASCII
const unsigned int CHAR_MAX = 256;
const char TABLE_FILE[] = "sma3char.tbl";
const unsigned int LEVEL_TEXT_START = 0x2F90AC; // Welcome to Yoshis Island

// Globals
std::vector<unsigned char> dataChars;
unsigned char lTable[CHAR_MAX];

// Functions
void open(const char* fileName);
std::string toHexString(unsigned int address, unsigned int padding);
void printLines(unsigned int start, unsigned int lines);
void loadTable();
std::string getCharacter(unsigned int x);
std::string decodeTextAddrs(unsigned int start, unsigned int lines);
std::vector<unsigned char> generateChars(std::string textString);
unsigned char getTableValue(unsigned char c);

struct ylevel {
    const unsigned int address;
    char text[40];
    unsigned int max_chars;
};

// Main
int main(int argc, char const *argv[])
{
    using namespace std;

    if (argc < 2) {
        cout << "Not enough arguments, include file as first argument" << endl;
        return 1;
    }
    open(argv[1]);
    loadTable();
    // This covers every single level name
    //cout << decodeTextAddrs(LEVEL_TEXT_START,2010) << endl;
    //printLines(LEVEL_TEXT_START,2050);

    auto v = generateChars("Welcome To\nYoshi's Island");
    stringstream ss;
    for (auto it = begin(v); it != end(v); ++it) {
        cout << "0x" << hex << (unsigned int)(*it) << " ";
    }
    printLines(LEVEL_TEXT_START, 20);

    return 0;
}

// Debugging
void printLines(unsigned int start, unsigned int lines) {
    using namespace std;

    unsigned int end = start + lines;
    if (end > dataChars.size()) {
        end = dataChars.size();
    }
    for (unsigned int i = start; i < end; i++) {
        stringstream ss;
        unsigned char c = dataChars.at(i);
        ss << toHexString(i,8) << ": " << toHexString((unsigned int)c,2) << " (" << (unsigned int)c << ") <= " << getCharacter((unsigned int)c);
        cout << ss.str() << endl;
    }
}

std::string decodeTextAddrs(unsigned int start, unsigned int lines) {
    using namespace std;

    unsigned int end = start + lines;
    stringstream ss;
    if (end > dataChars.size()) {
        end = dataChars.size();
    }
    for (unsigned int i = start; i < end; i++) {
        unsigned char c = dataChars.at(i);
        ss << getCharacter((unsigned int)c);
    }
    return ss.str();
}

// Fills the char array dataChars with data from fileName
void open(const char* fileName) {
    //TODO: Use fstream instead
    std::cout << fileName << std::endl;
    FILE* f = fopen(fileName,"rb");
    int i = 0;
    // While it is not the end of the file...
    while (!feof(f)) {
        i++;
        unsigned char c;
        // Read data into char c
        // 1 element of 1 byte from file "f"
        if(fread(&c, 1, 1, f) == 0) {
            // Zero return means error, so break
            break;
        }
        dataChars.push_back(c);
    }
    fclose(f);
}

std::string toHexString(unsigned int address, unsigned int padding) {
    using namespace std;
    stringstream ss;
    ss << "0x" << uppercase << setfill('0') << setw(padding) << hex << address;
    return ss.str();
}

void loadTable() {
    using namespace std;
    fstream tableFile(TABLE_FILE);
    for (unsigned int i = 0; i < CHAR_MAX; i++) {
        string str;
        getline(tableFile, str);
        lTable[i] = str.c_str()[0];
    }
    assert(lTable[177] == 'H');
    tableFile.close();
}

std::string getCharacter(unsigned int x) {
    using namespace std;
    stringstream ss;
    ss << lTable[x];
    return ss.str();
}

unsigned char getTableValue(unsigned char c) {
    using namespace std;

    for (unsigned int i = 0; i < CHAR_MAX; i++) {
        unsigned char curChar = lTable[i];
        if (c == curChar) {
            return i;
        }
    }
    return 0;
}

std::vector<unsigned char> generateChars(std::string textString) {
    using namespace std;
    vector<unsigned char> v = {0xFE, 0x0, 0x0};
    const char* cstr = textString.c_str();
    for (unsigned int i = 0; i < textString.size(); i++) {
        char curStringChar = cstr[i];
        if (curStringChar == '\n') {
            v.push_back(0xFE);
            v.push_back(0x10);
            v.push_back(0x00);
        } else {
            v.push_back(getTableValue(curStringChar));
        }
    }
    v.push_back(0xFD);
    return v;
}