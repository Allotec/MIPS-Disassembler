#include "Assembler.h"

#include <fstream>
#include <string>
#include <iomanip>
#include <vector>

int main(int argc, char** argv){
    std::string line;
    std::ifstream myfile("C:\\Users\\Matthew Champagne\\OneDrive\\Personal Projects\\MIPS-Disassembler\\TestProgram\\instructions.txt");
    std::ofstream output("Output.asm");
    std::vector<uint32_t> arrayLines;

    if (myfile.is_open()){
        while(std::getline(myfile, line)){
            arrayLines.push_back(std::stoul(line, nullptr, 16));
        }

        myfile.close();
    }
    else{
        std::cout << "File could not open\n";
        return(0);
    }

    uint32_t* instructionList = new uint32_t[arrayLines.size()];
    uint8_t length = arrayLines.size();

    for(int i = 0; i < arrayLines.size(); i++){
        instructionList[i] = arrayLines.at(i);
    }

    char** source = disassemble(instructionList, length);
    
    for(int i = 0; i < length; i++){
        output << source[i] << std::endl;
    }

    return(0);
}