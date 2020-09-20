#include <iostream> // cout, endl
#include <fstream>
#include <string>
#include <sstream> // string builder
#include <iomanip>
#include <string.h> // strcmp

// Constants
const int FILE_HEADER_LENGTH = 15;
const int FILE_HEADER_OFFSET = 160; // 0x000000A0 = 160
const char FILE_HEADER[] = "SUPER MARIOCA3"; // Hex at location converts to ASCII

// Globals
std::fstream fROM;
char* dataR;

// Functions
void open(const char fileName[]);
std::string toHexString(unsigned int address);

// Main
int main(int argc, char const *argv[])
{
    using namespace std;

    if (argc < 2) {
        cout << "Not enough arguments, include file as first argument" << endl;
        return 1;
    }
    open(argv[1]);
    cout << toHexString(160) << endl;
    
    return 0;
}

void open(const char fileName[]) {
    std::cout << fileName << std::endl;
    fROM.open(fileName, std::ios::ate | std::ios::binary | std::ios::in | std::ios::out);
    if (fROM.fail()) {
        std::cout << "Failed to open file" << std::endl;
        fROM.close(); // Close the stream
        fROM.clear(); // Reset the error state
        return;
    } else {
        char buffer[FILE_HEADER_LENGTH];
        buffer[FILE_HEADER_LENGTH-1] = 0; // Add terminator/NUL

        fROM.seekg(FILE_HEADER_OFFSET); // Move the read head to 0x000000A0
        fROM.read(buffer,FILE_HEADER_LENGTH-1);
        if (strcmp(buffer,FILE_HEADER) == 0) {
            // Header was found, get file length
            fROM.seekg(0,std::ios::end); // Read to end
            unsigned int fsz = fROM.tellg(); // Get current read position
            dataR = new char[fsz]; // Initiate the data variable with full length
            fROM.read(dataR, fsz); // Read the entire file into the data variable
            std::cout << "Opened successfully" << std::endl;
        } else {
            std::cout << "Header '" << FILE_HEADER << "' not present, found at location: " << buffer << std::endl;
            fROM.close(); // No need for clear since not an error
            return;
        }
    }
}

std::string toHexString(unsigned int address) {
    using namespace std;
    stringstream ss;
    ss << "0x" << uppercase << setfill('0') << setw(8) << hex << address;
    return ss.str();
}
