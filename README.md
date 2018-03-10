# ubirch-protocol for Calliope mini and micro:bit

This wraps the [ubirch-protocol](https://github.com/ubirch/ubirch-protocol) to be used with the 
[microbit-dal](https://github.com/calliope-mini/microbit-dal).

> Right now the lib will package all data as raw bytes into the payload.

## Example

```cpp
#include <MicroBit.h>
#include <armnacl.h>
#include "ubirch-protocol-calliope.h"

MicroBit uBit;

void hexprint(const uint8_t *b, size_t size) {
    for (unsigned int i = 0; i < size; i++) printf("%02x", b[i]);
    printf("\r\n");
}

unsigned char ed25519_secret_key[crypto_sign_SECRETKEYBYTES] = {
        // ADD secret key bytes
};

void sendMessage() {
    printf("==================================================================\r\n");
    ManagedString message = "{\"temperature\":" + ManagedString(uBit.thermometer.getTemperature()) + "}";
    ManagedString signedPacket = createSignedPacket(message);
    
    // instead of printing here, send it via UART/Wifi/BLE
    hexprint(reinterpret_cast<const uint8_t *>(signedPacket.toCharArray()), static_cast<size_t>(signedPacket.length()));
    printf("==================================================================\r\n");
}

int main() {
    // necessary to initialize the Calliope mini
    uBit.init();
    uBit.serial.baud(115200);
    printf("STARTING\r\n");

    for (int i = 0; i < 5; i++) {
        sendMessage();
    }
    printf("= DONE ===========================================================\r\n");
}
```