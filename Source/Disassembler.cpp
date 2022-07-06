#include "Assembler.h"

#define bitMask(num, start) (((1 << num) - 1) << start)

char** disassemble(uint32_t* instructionList, uint8_t length){
    uint8_t opcode, funct, fmt, ft, labelCount = 0;
    uint8_t opcodeIndex, extra = extraLines(instructionList, length);
    char** sourceCode = new char*[length + extra + 1];
    char* label = new char[tokenSize];
    char** line;
    struct label* head = nullptr, *ptr = nullptr;
    uint32_t location;

    for(int i = 0; i < length; i++){
        opcode = ((instructionList[i] & bitMask(6, 26)) >> 26);
        fmt = ((instructionList[i] & bitMask(5, 21)) >> 21);
        ft = ((instructionList[i] & bitMask(5, 16)) >> 16);
        funct = (instructionList[i] & bitMask(6, 0));

        opcodeIndex = instructionLookup(opcode, funct, fmt, ft);

        if(instructionList[i] == 0){
            sourceCode[i] = new char[lineSize];
            strcpy(sourceCode[i], "nop");
            continue;
        }

        switch(instructions[opcodeIndex].format){
            //R type
            case 0:
                sourceCode[i] = rTypeDis(instructionList[i], opcodeIndex, fmt, ft);
                break;

            //I type
            case 1:
                sourceCode[i] = iTypeDis(instructionList[i], opcodeIndex, fmt, ft);

                break;

            //J type
            case 2:
                sourceCode[i] = jTypeDis(instructionList[i], opcodeIndex);

                break;

            //FR type
            case 3:
                sourceCode[i] = frTypeDis(instructionList[i], opcodeIndex, fmt, ft, funct);

                break;

            //FI yype
            case 4:
                sourceCode[i] = fiTypeDis(instructionList[i], opcodeIndex, fmt, ft);

                break;

            //Error
            default:


                break;
        }

    }

    /*Putting in the labels
    1. For each of the instructions with labels add it to the linked list and calculate its absolute position
    2. Check if any of them are pointing to the same position and keep only one
    3. Based on the order assign the names in the format .Lx: where x is its position in the linked list
    4. Go back through each instruction and change the arguement based on the name generated and index calculated
    5. Sort the linked list based on the absolute index
    6. Go back through the instructions again and add the labels in while adding 1 position for each time you add a label
    */
    
    //Add labels into the instructions
    if(extra != 0){
        for(int i = length; i < length + extra; i++){
            sourceCode[i] = new char[lineSize];
            sourceCode[i][0] = '\0';
        }

        for(int i = 0; i < length; i++){
            line = parseLine(sourceCode[i]);
            strcpy(label, ".Lx");

            if(strcmp(line[0], "beq") == 0 || strcmp(line[0], "bne") == 0){
                label[2] = '0' + labelCount++;
                location = i + arrayToNum(line[3]) + 1;
                addToList(head, label, location);
                sprintf(sourceCode[i], "%s %s, %s, %s", line[0], line[1], line[2], findNode(head, location));
            }
            else if(strcmp(line[0], "j") == 0 || strcmp(line[0], "jal") == 0){
                label[2] = '0' + labelCount++;
                location = (arrayToNum(line[1]) - 0x400000) >> 2;
                addToList(head, label, location);
                sprintf(sourceCode[i], "%s %s", line[0], findNode(head, location));
            }

            delete2d(line, maxTokens);
        }

        sortList(head);

        labelCount = 0;
        ptr = head;
        while(ptr != nullptr){
            shiftArray(sourceCode, ptr->location + labelCount, length + extra - 1);
            strcat(ptr->name, ":");
            sprintf(sourceCode[ptr->location + labelCount], ptr->name);
            labelCount++;
            ptr = ptr->next;
        }

        sourceCode[length + extra] = nullptr;
    }
    else{
        sourceCode[length] = nullptr;
    }
    
    shiftArray(sourceCode, 0, length + extra - 1);
    strcpy(sourceCode[0], "main:");

    ptr = head;
    while(ptr != nullptr){
        printf("Name- %s Location- %u\n", ptr->name, ptr->location);
        ptr = ptr->next;
    }

    delete[] label;

    return(sourceCode);
}

//Sorts the list based on the location variable
void sortList(struct label* head){
    char *name;
    uint32_t location;
    struct label* ptr1 = head, *ptr2 = head;
    
    while(ptr1 != nullptr){
        while(ptr2 != nullptr){
            if(ptr1->location < ptr2->location){
                name = ptr1->name;
                location = ptr1->location;

                ptr1->name = ptr2->name;
                ptr1->location = ptr2->location;
                ptr2->name = name;
                ptr2->location = location;
            }

            ptr2 = ptr2->next;
        }

        ptr1 = ptr1->next;
        ptr2 = head;
    }
}

//Returns the name of the node given the location
char* findNode(struct label* head, uint32_t location){
    struct label* ptr = head;
    
    while(ptr != nullptr){
        if(ptr->location == location){
            return(ptr->name);
        }
        ptr = ptr->next;
    }

    return(nullptr);
}

//Adds node to list while checking if the node location already exists
void addToList(struct label* &head, char* name, uint32_t location){
    
    if(head == nullptr){
        head = new struct label;
        head->location = location;
        head->name = new char[tokenSize];
        head->next = nullptr;
        strcpy(head->name, name);
    }
    else{
        struct label* ptr = head;
        while(ptr->next != nullptr){
            if(ptr->location == location){
                return;
            }
            ptr = ptr->next;
        }

        ptr->next = new struct label;
        ptr = ptr->next;
        ptr->location = location;
        ptr->name = new char[tokenSize];
        ptr->next = nullptr;
        strcpy(ptr->name, name);
    }
}

//Returns the disassembled R type instruction
char* rTypeDis(uint32_t instruction, uint8_t opcodeIndex, uint8_t rs, uint8_t rt){
    char* line = new char[lineSize];
    uint8_t rd = ((instruction & bitMask(5, 11)) >> 11), shamt;

    //Special Cases
    //SLL, SRL, and SRA
    if(instructions[opcodeIndex].funct == 0x00 || instructions[opcodeIndex].funct == 0x02 || instructions[opcodeIndex].funct == 0x03){
        shamt = ((instruction & bitMask(5, 6)) >> 6);

        sprintf(line, "%s %s, %s, %d", instructions[opcodeIndex].name, registers[regLookup(rd, false)].name, registers[regLookup(rt, false)].name, shamt);
    }
    //jr
    else if(instructions[opcodeIndex].funct == 0x08){
        sprintf(line, "%s %s", instructions[opcodeIndex].name, registers[regLookup(rs, false)].name);
    }
    //mfhi and mflo
    else if(instructions[opcodeIndex].funct == 0x10 || instructions[opcodeIndex].funct == 0x12){
        sprintf(line, "%s %s", instructions[opcodeIndex].name, registers[regLookup(rd, false)].name);
    }
    //mul/multu, div/divu
    else if(instructions[opcodeIndex].funct == 0x18 || instructions[opcodeIndex].funct == 0x19 || instructions[opcodeIndex].funct == 0x1a || instructions[opcodeIndex].funct == 0x1b){
        sprintf(line, "%s %s, %s", instructions[opcodeIndex].name, registers[regLookup(rs, false)].name, registers[regLookup(rt, false)].name);
    }
    //mfco
    else if(instructions[opcodeIndex].opcode == 0x10){
        sprintf(line, "%s %s, %s", instructions[opcodeIndex].name, registers[regLookup(rd, false)].name, registers[regLookup(rs, false)].name);
    }
    //opcode rd, rs, rt
    else{
        sprintf(line, "%s %s, %s, %s", instructions[opcodeIndex].name, registers[regLookup(rd, false)].name, registers[regLookup(rs, false)].name, registers[regLookup(rt, false)].name);
    }

    return(line);
}


//Returns the disassembled I type instruction
char* iTypeDis(uint32_t instruction, uint8_t opcodeIndex, uint8_t rs, uint8_t rt){
    char* line = new char[lineSize];
    int16_t imm = (instruction & bitMask(16, 0));

    //lb, lbu, ldc1, lh, lhu, ll, lw, lwc1, lwl, lwr, swc1, swd1
    //0x23, 0x24, 0x25, 0x28, 0x29, 0x30, 0x31, 0x38, 0x2b, 0x35, 0x39, 0x3d
    if(instructions[opcodeIndex].opcode == 0x23 || instructions[opcodeIndex].opcode == 0x24 || instructions[opcodeIndex].opcode == 0x25 || 
        instructions[opcodeIndex].opcode == 0x28 || instructions[opcodeIndex].opcode == 0x29 || instructions[opcodeIndex].opcode == 0x30 ||
        instructions[opcodeIndex].opcode == 0x31 || instructions[opcodeIndex].opcode == 0x38 || instructions[opcodeIndex].opcode == 0x2b ||
        instructions[opcodeIndex].opcode == 0x35 || instructions[opcodeIndex].opcode == 0x39 || instructions[opcodeIndex].opcode == 0x3d
        ){
        // if(imm == 0){
        //     sprintf(line, "%s %s, (%s)", instructions[opcodeIndex].name, registers[regLookup(rt, false)].name, registers[regLookup(rs, false)].name);
        // }
        if(rs == 0){
            sprintf(line, "%s %s, %d", instructions[opcodeIndex].name, registers[regLookup(rt, false)].name, imm);
        }
        else{
            sprintf(line, "%s %s, %d(%s)", instructions[opcodeIndex].name, registers[regLookup(rt, false)].name, imm, registers[regLookup(rs, false)].name);
        }
    }
    else{
        //Bne and Beq
        if(instructions[opcodeIndex].opcode == 0x04 || instructions[opcodeIndex].opcode == 0x05){
            sprintf(line, "%s %s, %s, %d", instructions[opcodeIndex].name, registers[regLookup(rs, false)].name, registers[regLookup(rt, false)].name, imm);
        }
        //lui
        else if(instructions[opcodeIndex].opcode == 0x0f){
            sprintf(line, "%s %s, 0x%x", instructions[opcodeIndex].name, registers[regLookup(rt, false)].name, imm);
        }
        else{
            sprintf(line, "%s %s, %s, %d", instructions[opcodeIndex].name, registers[regLookup(rt, false)].name, registers[regLookup(rs, false)].name, imm);
        }
    }


    return(line);
}


//Returns the disassembled J type instruction
char* jTypeDis(uint32_t instruction, uint8_t opcodeIndex){
    char* line = new char[lineSize];
    uint32_t imm = (instruction & bitMask(26, 0)) << 2;

    sprintf(line, "%s 0x%x", instructions[opcodeIndex].name, imm);

    return(line);
}


//Returns the disassembled FR type instruction
char* frTypeDis(uint32_t instruction, uint8_t opcodeIndex, uint8_t fmt, uint8_t ft, uint8_t funct){
    char* line = new char[lineSize];
    uint8_t fd = ((instruction & bitMask(5, 6)) >> 6);
    uint8_t fs = ((instruction & bitMask(5, 11)) >> 11);

    //c.x.s and c.x.d
    if(funct == 0x32 || funct == 0x3c || funct == 0x3e){
        if(fs == 0){
            sprintf(line, "%s %s, %s", instructions[opcodeIndex].name, registers[regLookup(fs, true)].name, registers[regLookup(ft, true)].name);
        }
        else{
            sprintf(line, "%s %d, %s, %s", instructions[opcodeIndex].name, (fd / 4),registers[regLookup(fs, true)].name, registers[regLookup(ft, true)].name);
        }
    }
    else{
        sprintf(line, "%s %s, %s, %s", instructions[opcodeIndex].name, registers[regLookup(fd, true)].name, registers[regLookup(fs, true)].name, registers[regLookup(ft, true)].name);
    }

    return(line);
}

//Returns the disassembled FI type instruction
char* fiTypeDis(uint32_t instruction, uint8_t opcodeIndex, uint8_t fmt, uint8_t ft){
    char* line = new char[lineSize];
    uint32_t imm = (instruction & bitMask(26, 0)) << 2;

    //works for both cases
    if(ft == 0){
        sprintf(line, "%s 0x%x", instructions[opcodeIndex].name, imm);
        return(line);
    }

    //bc1f special case
    if(ft % 4 == 0){
        sprintf(line, "%s %d, 0x%x", instructions[opcodeIndex].name, (ft / 4), imm);
    }
    //bc1t special case
    else{
        sprintf(line, "%s %d, 0x%x", instructions[opcodeIndex].name, ((ft - 1) / 4), imm);
    }

    return(line);
}

//Returns the index of the instruction
uint8_t instructionLookup(uint8_t opcode, uint8_t funct, uint8_t fmt, uint8_t ft){
    //FR and FI instructions
    if(opcode == 0x11){
        //FR type instructions
        if(fmt != 8){
            for(int i = 0; i < instructionNum; i++){
                if(instructions[i].opcode == opcode && instructions[i].FMT == fmt && instructions[i].funct == funct){
                    return(i);
                }
            }
        }
        //FI type instructions
        else{
            for(int i = 0; i < instructionNum; i++){
                if(instructions[i].opcode == opcode && instructions[i].FMT == fmt && instructions[i].FT == (ft % 4)){
                    return(i);
                }
            }
        }
    }
    //R type instructions
    else if(opcode == 0){
        for(int i = 0; i < instructionNum; i++){
            if(instructions[i].funct == funct && instructions[i].opcode == 0){
                return(i);
            }
        }
    }
    //I and J type instructions
    else{
        for(int i = 0; i < instructionNum; i++){
            if(instructions[i].opcode == opcode){
                return(i);
            }
        }
    }

    return(255);
}

//fType specifies whether its floating point or not
uint8_t regLookup(uint8_t num, bool fType){
    if(num == 29 || num == 30){//Get rid of after debugging
        return(num);
    }

    uint8_t offset = fType ? 64 : 32;

    for(int i = 32/*offset*/; i < regNum; i++){
        if(registers[i].val == num){
            return(i);
        }
    }

    return(255);
}

uint8_t extraLines(uint32_t* instruction, uint8_t length){
    uint8_t opcode, extra = 0;

    for(int i = 0; i < length; i++){
        opcode = ((instruction[i] & bitMask(6, 26)) >> 26);

        //Opcodes of all the jump and branch instructions that take a label as an input
        if(opcode >= 0x02 && opcode <= 0x05)
            extra++;
    }

    return(2 * extra);
}
