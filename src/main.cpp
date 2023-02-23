#include <stdio.h>
#include "tx.h"

#include <fstream>

#define SAMPLERATE  48e3
#define SYMBOLRATE  800.0
#define DEVIATION   800.0
#define TX_FREQ     19e3
//#define FRAME_SIZE  (259*8)
#define FRAME_SIZE  (48000/60)

int main() {
    TX tx(SAMPLERATE, SYMBOLRATE, DEVIATION, TX_FREQ);

    // Start transmit
    tx.start();

    // Open file
    std::ifstream file("/home/ryzerth/bingchilling.wav", std::ios::in | std::ios::binary);
    file.seekg(0x4E);
    int16_t buf[FRAME_SIZE];

    // 0x4E

    // Write data to input
    int n = 0;
    while (true) {
        printf("Swap %d\n", n++);
        file.read((char*)buf, FRAME_SIZE*sizeof(int16_t));
        volk_16i_s32f_convert_32f(tx.input.writeBuf, buf, 32768.0f, FRAME_SIZE);
        tx.input.swap(FRAME_SIZE);
    }

    // Stop transmit
    tx.stop();
    
    return 0;
}