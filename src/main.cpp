#include <stdio.h>
#include "tx.h"

#define SAMPLERATE  192e3
#define SYMBOLRATE  800.0
#define DEVIATION   800.0
#define TX_FREQ     19e3
//#define FRAME_SIZE  (259*8)
#define FRAME_SIZE  800

int main() {
    TX tx(SAMPLERATE, SYMBOLRATE, DEVIATION, TX_FREQ);

    // Start transmit
    tx.start();

    // Write data to input
    int n = 0;
    while (true) {
        for (int i = 0; i < FRAME_SIZE; i++) {
            tx.input.writeBuf[i] = ((rand() % 2) ? 1.0f : -1.0f) * 120.0;
        }
        tx.input.swap(FRAME_SIZE);
        printf("Swapped %d\n", n++);
    }

    // Stop transmit
    tx.stop();
    
    return 0;
}