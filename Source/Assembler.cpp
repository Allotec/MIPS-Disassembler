#include "Assembler.h"

//Returns a list of 32 bit integer machine code
uint32_t* assemble(char** arr, uint8_t maxSize, uint8_t extraLines){
    //Copy orignal array to not mess up its contents
    char** arrayT = new char*[maxSize + extraLines];
    int k;
    
    for(int j = 0; j < maxSize + extraLines; j++){
        arrayT[j] = new char[lineSize];

        if(j < maxSize){
            for (k = 0; arr[j][k] != 0; k++){
                arrayT[j][k] = arr[j][k];
            }
        }
        else{
            continue;
        }
        
        arrayT[j][k] = 0;
    }

    uint8_t length = maxSize;
    //Opcode -> 0
    //Rd -> 1
    //Rs -> 2
    //Rt or imm or shamt -> 3 
    char** splitArray;
    int i;
    uint32_t programCounter = 0x400000;
    uint32_t* instructions = new uint32_t[length + extraLines];
    length += replacePseudo(arrayT, length);
    struct label* head = labelList(arrayT, length), *ptr = head;

    //Split up all the instructions
    for(int k = 0; k < length; k++){
        splitArray = parseLine(arrayT[k]);

        instructions[k] = mipsInstruction(splitArray[0], splitArray[1], splitArray[2], splitArray[3], head, programCounter);

        delete2d(splitArray, maxTokens);

        programCounter += 4;
    }

    delete2d(arrayT, maxSize);

    instructions[length] = 0xFFFFFFFF;

    //Clear the label linked list
    while(head != nullptr){
        ptr = head->next;
        delete [] head->name;
        delete head;
        head = ptr;
    }

    return(instructions);
}

//Replacing pseudo instruction with MIPS instructions
uint8_t replacePseudo(char** array, uint8_t length){
    char** splitLine;
    uint8_t index = 0, tokens;
    uint8_t addLines = 0;
    int num;
    
    for(int i = 0; i < length + addLines; i++){
        splitLine = parseLine(array[i]);

        //case move $t0, $s0
        if(strcmp(splitLine[0], "move") == 0){
            sprintf(array[i], "addu %s, $0, %s", splitLine[1], splitLine[2]);
        }
        //case li $t0, imm
        else if(strcmp(splitLine[0], "li") == 0){
            num = arrayToNum(splitLine[2]);

            //If the imm iss less than 16 bits then use one instruction else use two
            if(num <= 0xFFFF){
                sprintf(array[i], "addi %s, $0, %s", splitLine[1], splitLine[2]);
            }
            else{
                shiftArray(array, i, length + addLines++);

                sprintf(array[i], "lui $at, %d", (num & 0xFFFF0000) >> 16);
                sprintf(array[i + 1], "ori %s, $at, %d", splitLine[1] ,(num & 0x0000FFFF));
            }
        }
        else if(strcmp(splitLine[0], "bgt") == 0){
            shiftArray(array, i, length + addLines++);
            sprintf(array[i], "slt $at, %s, %s", splitLine[2], splitLine[1]);
            sprintf(array[i + 1], "bne $at, $0, %s", splitLine[3]);

        }
        else if(strcmp(splitLine[0], "bge") == 0){
            shiftArray(array, i, length + addLines++);
            sprintf(array[i], "slt $at, %s, %s", splitLine[1], splitLine[2]);
            sprintf(array[i + 1], "beq $at, $0, %s", splitLine[3]);

        }
        else if(strcmp(splitLine[0], "ble") == 0){
            shiftArray(array, i, length + addLines++);
            sprintf(array[i], "slt $at, %s, %s", splitLine[2], splitLine[1]);
            sprintf(array[i + 1], "beq $at, $0, %s", splitLine[3]);

        }
        else if(strcmp(splitLine[0], "blt") == 0){
            shiftArray(array, i, length + addLines++);
            sprintf(array[i], "slt $at, %s, %s", splitLine[1], splitLine[2]);
            sprintf(array[i + 1], "bne $at, $0, %s", splitLine[3]);

        }
        
        delete2d(splitLine, maxTokens);
    }

    return(addLines);
}

//Shifts 2d array up by the specified parameters
void shiftArray(char** array, uint8_t endIndex, uint8_t length){
    for(uint8_t i = length; i > endIndex; i--){
        strncpy(array[i], array[i - 1], lineSize);
    }

}

//Deletes 2d array
void delete2d(char** array, uint8_t length){
    //Delete temp array
    for(int i = 0; i < length; i++) {
        delete [] array[i];
    }

    delete [] array;
}

//Return a linked list of labels and values and removes thme from the array
struct label* labelList(char** array, uint8_t &length){
    int location;
    struct label* head = NULL, *tail = NULL;
    uint32_t programCounter = 0x400000;
    int i;

    //Look for labels and store them with there values
    for(int k = 0; k < length; k++){
        location = labelFind(array[k], ':');

        if(location != 255){
            //Store in linked list
            if(head == NULL){
                head = new struct label;
                tail = head;
                head->location = programCounter;
                head->name = new char[lineSize];
                tail->next = nullptr;
                
                for(uint8_t j = 0; j < location; j++){
                    head->name[j] = array[k][j];
                }

                head->name[location] = 0;
            }
            else{
                tail->next = new struct label;
                tail = tail->next;
                tail->location = programCounter;
                tail->name = new char[lineSize];
                tail->next = nullptr;
                
                for(uint8_t j = 0; j < location; j++){
                    tail->name[j] = array[k][j];
                }

                tail->name[location] = 0;
            }

            //Shift and delete
            for(i = k + 1; i < length; i++){
                for(uint8_t j = 0; j < lineSize; j++){
                    array[i - 1][j] = array[i][j];
                }
            }

            length--;

        }
        programCounter += 4;
    }

    return(head);
}


//Passes back a parsed line of assembly
char** parseLine(char* line){
    char** splitArray = new char*[maxTokens];
    int arrayIndex = 0, arrayRow = 0;
    int i;

    for(int j = 0; j < maxTokens; j++){
        splitArray[j] = new char[tokenSize];
    }

    //Tokenize each expression
    while(arrayIndex < lineSize && arrayRow < maxTokens){
        //If we are at the start of a word
        if(*(line + arrayIndex) != ' ' && *(line + arrayIndex) != ',' && *(line + arrayIndex) != '('){
            for(i = 0; *(line + arrayIndex) != ' ' && *(line + arrayIndex) != ',' && *(line + arrayIndex) != ')' && *(line + arrayIndex) != '(' && *(line + arrayIndex) != '\0' && arrayIndex < lineSize; i++){
                splitArray[arrayRow][i] = *(line + arrayIndex);
                arrayIndex++;
            }

            
            for(int j = i; j < tokenSize; j++){
                splitArray[arrayRow][j] = 0;
            }

            arrayRow++;
        }

        arrayIndex++;
    }

    return(splitArray);    
}

//Returns the program counter value corresponding to the label
uint32_t labelFinder(struct label* head, char* array){
    struct label *ptr = head;

    while(ptr != NULL){
        if(strcmp(ptr->name, array) == 0){
            return(ptr->location);
        }
        else{
            ptr = ptr->next;
        }
    }

    return(0);
}


uint32_t mipsInstruction(char* opcode, char* rd, char* rs, char* rt, struct label* head, uint32_t programCounter){
    uint32_t instruction = 0;
    uint8_t opcodeIndex = opcodeLookup(opcode);

    //All Combinations
    uint32_t opcodeV = instructions[opcodeIndex].opcode;
    uint32_t functV = instructions[opcodeIndex].funct;
    uint32_t rsV = regLookup(rs);
    uint32_t rtV = regLookup(rt);
    uint32_t rdV = regLookup(rd);
    uint32_t shamt = 0;
    int32_t tempNum;

    //Tests the instruction formaat
    switch (instructions[opcodeIndex].format){
        //R type
        case 0:            
            //Special Cases
            // SLL and SRL
            if(functV == 0x00 || functV == 0x02 || functV == 0x03){
                rtV = rsV;
                rsV = 0;
                shamt = arrayToNum(rt);
                shamt <<= 6;
                instruction = shamt;
            }
            // jr
            else if(functV == 0x08){
                rsV = rdV;
                rdV = 0;
                rtV = 0;
            }
            // mfhi and mflo
            else if(functV == 0x10 || functV == 0x12){
                rsV = 0;
                rtV = 0;
            }
            // mul/multu, div/divu, and mfco
            else if(functV == 0x18 || functV == 0x19 || functV == 0x1a || functV == 0x1b || opcodeV == 0x10){
                rtV = rsV;
                rsV = rdV;
                rdV = 0;
                instruction = opcodeV << 26;
            }

            //General Case
            rdV <<= 11;
            rtV <<= 16;
            rsV <<= 21;

            instruction |= rdV | rtV | rsV| functV;

            break;
        
        //I type
        case 1:
            //Register swaps and immdiate calculation
            //case opcode rt, rs, imm
            if(rtV == 0xFF){
                //For beq and bne
                if(opcodeV == 0x04 || opcodeV == 0x05){//Change to relative addressing instead of absolute
                    rtV = rsV;
                    rsV = rdV;
                    rdV = ((labelFinder(head, rt) - programCounter) - 4) >> 2 & 0xFFFF;
                }
                //For lui
                else if(opcodeV == 0x0F){
                    rtV = rdV;
                    rsV = 0;
                    rdV = arrayToNum(rs);
                }
                else{
                    //case op rt, imm
                    if(rsV == 0xFF){
                        rtV = rdV;
                        rdV = arrayToNum(rs) & 0x0000FFFF;
                        rsV = 0;
                    }
                    //case opcode rd, rs, imm
                    else{
                        rtV = rdV;
                        rdV = arrayToNum(rt) & 0x0000FFFF;
                    }
                }
            }
            //case opcode rt, imm(rs)
            else if(rsV == 0xFF){
                rsV = rtV;
                rtV = rdV;
                rdV = arrayToNum(rs) & 0x0000FFFF;
            }

            rtV <<= 16;
            rsV <<= 21;
            opcodeV <<= 26;

            instruction = rdV | rtV | rsV | opcodeV;

            break;

        //J type
        case 2://put num in rd
            //For j and jal
            rdV = (labelFinder(head, rd) & 0x03FFFFFF) >> 2;
            opcodeV <<= 26;

            instruction = rdV | opcodeV;

            break;

        //FR type
        case 3:
            //rt -> ft, rs -> fmt, and shamt -> fmt
            //case for c.x.s or c.x.d
            if(functV == 0x32 || functV == 0x3c || functV == 0x3e){
                if(rdV == 0xFF){
                    rdV = rsV;
                    rsV = arrayToNum(rd) * 4;
                }
                else{
                    rtV = rsV;
                    rsV = 0;
                }
            }

            //case op fd, fs, ft
            rdV <<= 6;
            rsV <<= 11;
            rtV <<= 16;
            shamt = instructions[opcodeIndex].FMT << 21;
            opcodeV <<= 26;

            instruction = opcodeV | shamt | rtV | rsV | rdV | functV;

            break;

        //FI type
        case 4:
            //rt -> ft, rs -> fmt, and rdV -> imm
            rdV = labelFinder(head, rd);

            //case op label 
            //assumes imm is zero
            if(rdV != 0){
                rtV = instructions[opcodeIndex].FT;
                rsV = instructions[opcodeIndex].FMT;
                rdV = ((labelFinder(head, rd) - programCounter) - 4) >> 2 & 0xFFFF;
            }
            //case op imm, label
            else{
                rtV = instructions[opcodeIndex].FT + (arrayToNum(rd) * 4); //No idea why but the specified flag is multiplied by 4
                rsV = instructions[opcodeIndex].FMT;
                rdV = ((labelFinder(head, rs) - programCounter) - 4) >> 2 & 0xFFFF;
            }

            rtV <<= 16;
            rsV <<= 21;
            opcodeV <<= 26;

            instruction = rdV | rtV | rsV | opcodeV;

            break;

        //Error
        default:
            //For nop
            return(0);
            break;
    }

    return(instruction);
}

//Finds the position of the first character given in the array, if its not then -1 is returned
uint8_t labelFind(char* array, char c){
    for(int i = 0; *(array + i) != 0 && i < lineSize; i++){
        if(*(array + i) == c){
            return(i);
        }
    }

    return(255);
}

//Returns the index into the table for the given opcode
uint32_t opcodeLookup(char* opcode){
    for(uint32_t i = 0; i < instructionNum; i++){
        if(strcmp(instructions[i].name, opcode) == 0)
            return(i);
    }

    return(255);
}

//Returns the value of the register given the name
uint32_t regLookup(char* reg){
    for(int i = 0; i < regNum; i++){
        if(strcmp(registers[i].name, reg) == 0)
            return(registers[i].val);
    }

    return(255);
}

//Turns a char array into an integer
int arrayToNum(char* num){
    int8_t sign = 1;
    int base = 10;
    int val = 0;
    uint8_t power = 0;
    
    //Determines if the number is negative and shifts everything down if it is
    if(num[0] == '-'){
        sign = -1;
        int i = 1;

        for(i = 1; i < tokenSize; i++){
            num[i - 1] = num[i];
        }

        num[i - 1] = 0;
    }
    if(num[0] == '0' && (num[1] == 'x' || num[1] == 'X')){
        int i = 1;
        base = 16;

        for(int j = 0; j < 2; j++){
            for(i = 1; i < tokenSize; i++){
                num[i - 1] = num[i];
            }

            num[i - 1] = 0;
        }
    }

    for(int i = tokenSize - 1; i >= 0; i--){
        if(num[i] == '\0')
            continue;

        if(base == 16){
            if((num[i] >= 'a' && num[i] <= 'f'))
                num[i] = '9' + 1 + (num[i] - 'a');
            else if((num[i] >= 'A' && num[i] <= 'F'))
                num[i] = '9' + 1 + (num[i] - 'A');
        }

        val += (num[i] - '0') * mathPow(base, power++);
    }

    return(val * sign);
}

int mathPow(int x, uint8_t y){
    int pow = 1;

    for(uint8_t i = 0; i < y; i++){
        pow *= x;
    }

    return(pow);
}
