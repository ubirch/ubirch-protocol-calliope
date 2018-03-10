/**
 * @file
 *
 * A ubirch protocol implementation for the Calliope mini (or micro:bit).
 *
 * @copyright (c) ubirch GmbH.
 * @author Matthias L. Jugel.
 *
 * Licensed under the Apache License 2.0
 */

#include "ubirch-protocol-calliope.h"
#include <ubirch/ubirch_ed25519.h>

unsigned char prev_sig[UBIRCH_PROTOCOL_SIGN_SIZE] = {};

inline int msgpack_write_dal(void *data, const char *buf, size_t len) {
    msgpack_sbuffer *sbuf = (msgpack_sbuffer *) data;

    // if we have data in sbuf, use the stack to exchange and increase the buffer
    // this is neccessary to avoid heap fragmentation
    char tmp[sbuf->size] = {};
    if (sbuf->data) {
        memcpy(tmp, sbuf->data, sbuf->size);
        free(sbuf->data);
    }

    sbuf->data = (char *) malloc(sbuf->size + len);
    memcpy(sbuf->data, tmp, sbuf->size);
    memcpy(sbuf->data + sbuf->size, buf, len);
    sbuf->size += len;

    return 0;
}

PacketBuffer createSignedPacket(ManagedString message) {
    uint32_t _deviceId = __builtin_bswap32(microbit_serial_number());
    unsigned char hardwareSerial[UBIRCH_PROTOCOL_UUID_SIZE] = {};
    for (int i = 0; i < 4; i++) memcpy(hardwareSerial + 4 * i, &_deviceId, 4);

    // initialize ubirch protocol
    msgpack_sbuffer sbuf = {};
    ubirch_protocol proto = {};
    ubirch_protocol_init(&proto, proto_chained, 0x00,
                         &sbuf, msgpack_write_dal, ed25519_sign, hardwareSerial);
    memcpy(proto.signature, prev_sig, UBIRCH_PROTOCOL_SIGN_SIZE);

    // create a packer for ubirch protocol
    msgpack_packer pk = {};
    msgpack_packer_init(&pk, &proto, ubirch_protocol_write);

    ubirch_protocol_start(&proto, &pk);
    msgpack_pack_raw(&pk, static_cast<size_t>(message.length()));
    msgpack_pack_raw_body(&pk, message.toCharArray(), static_cast<size_t>(message.length()));
    ubirch_protocol_finish(&proto, &pk);

    // copy the last signature into our local buffer
    memcpy(prev_sig, proto.signature, UBIRCH_PROTOCOL_SIGN_SIZE);

    // hexdump("PKT", reinterpret_cast<const uint8_t *>(sbuf.data), sbuf.size);

    // create a packetbuffer from the buffer and free sbuf
    PacketBuffer buf(reinterpret_cast<uint8_t *>(sbuf.data), sbuf.size);
    msgpack_sbuffer_destroy(&sbuf);

    return buf;
}
