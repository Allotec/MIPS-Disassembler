#include <iostream>

using namespace std;

int main(){
    uint16_t address = 23654;

    printf("%x\n", address);

    for(int i = 0; i < 2; i++){
        printf("%d- %x ", i , (uint8_t)((address & (0xFF << (8 * i))) >> (8 * i)) ); 
    }

    return(0);
}

