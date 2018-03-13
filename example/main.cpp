
#include <armnacl.h>
#include <MicroBit.h>
#include <CryptoUbirchProtocol.h>

MicroBit uBit;
CryptoUbirchProtocol ubirch;

/* ==== ECC KEYS ================= */
unsigned char ed25519_public_key[crypto_sign_PUBLICKEYBYTES] = {
        0x7b, 0x60, 0x88, 0x2b, 0xee, 0x2e, 0x3f, 0x01, 0x79, 0x07, 0xce, 0x84, 0xe5, 0xe1, 0xc8, 0x7f,
        0x70, 0x57, 0x60, 0xfa, 0xc5, 0x87, 0x7f, 0xe0, 0xde, 0x7c, 0x58, 0x06, 0xc4, 0x69, 0x1f, 0x2f,
};
unsigned char ed25519_secret_key[crypto_sign_SECRETKEYBYTES] = {
        0x3b, 0xc4, 0xb2, 0x49, 0x9d, 0x97, 0x50, 0x1a, 0xba, 0x63, 0xb0, 0xf6, 0x30, 0x8f, 0x8d, 0x91,
        0x3b, 0xcb, 0xdc, 0xed, 0xa8, 0x53, 0x26, 0x9d, 0x68, 0x75, 0xa0, 0x6a, 0x64, 0x32, 0xfa, 0xc7,
        0x7b, 0x60, 0x88, 0x2b, 0xee, 0x2e, 0x3f, 0x01, 0x79, 0x07, 0xce, 0x84, 0xe5, 0xe1, 0xc8, 0x7f,
        0x70, 0x57, 0x60, 0xfa, 0xc5, 0x87, 0x7f, 0xe0, 0xde, 0x7c, 0x58, 0x06, 0xc4, 0x69, 0x1f, 0x2f,
};

// a little helper function to print the resulting byte arrays
void hexprint(const uint8_t *b, size_t size) {
    for (unsigned int i = 0; i < size; i++) uBit.serial.printf("%02x", b[i]);
    uBit.serial.printf("\r\n");
}

/**
 * Save the last generated signature to Calliope mini flash.
 */
void saveSignature() {
    uBit.serial.printf("saving last signature...\r\n");
    PacketBuffer signature = ubirch.getLastSignature();
    uBit.storage.put("s1", signature.getBytes(), 32);
    uBit.storage.put("s2", signature.getBytes() + 32, 32);
}

/**
 * Load the latest signature from flash (after reset).
 */
void loadSignature() {
    KeyValuePair *s1 = uBit.storage.get("s1");
    KeyValuePair *s2 = uBit.storage.get("s2");
    if (s1 && s2) {
        uBit.serial.printf("found last signature...\r\n");
        uint8_t s[64];
        memcpy(s, s1->value, 32);
        memcpy(s+32, s2->value, 32);
        PacketBuffer signature(s, sizeof(s));
        ubirch.setLastSignature(signature);
    }
    // make sure to free memory
    delete s1;
    delete s2;
}

int main() {
    uBit.init();
    uBit.serial.printf("HELLO WORLD!\r\n");
    uBit.display.readLightLevel();

    loadSignature();

    time_t ts;
    time(&ts);
    // create 3 consecutive messages and chain them, pressing reset will continue the chain
    for (int i = 0; i < 3; i++) {
        ubirch.startMessage();
        ubirch.addMap("data", 1);
        ubirch.addMap((int) time, 2);
        ubirch.addInt("t", uBit.thermometer.getTemperature());
        ubirch.addInt("l", uBit.display.readLightLevel());
        PacketBuffer packet = ubirch.finishMessage();
        uBit.serial.printf("message: %d\r\n", packet.length());
        hexprint(packet.getBytes(), static_cast<size_t>(packet.length()));
    }
    saveSignature();

    printf("THE END\r\n");
}

