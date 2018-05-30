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

#include "CryptoUbirchProtocol.h"
#include <ubirch/ubirch_ed25519.h>
#include <ubirch/ubirch_protocol_kex.h>

CryptoUbirchProtocol::CryptoUbirchProtocol(const uint32_t deviceId) : sbuf(), proto(), pk(), hardwareSerial() {
    reset(deviceId);
}

void CryptoUbirchProtocol::reset(const uint32_t deviceId) {
    memcpy(hardwareSerial + sizeof(hardwareSerial) - sizeof(deviceId), &deviceId, 4);

    // initialize ubirch protocol
    memset(&proto, 0, sizeof(proto));
    ubirch_protocol_init(&proto, proto_chained, 0x53,
                         &sbuf, msgpack_write_dal, ed25519_sign, hardwareSerial);
}

void CryptoUbirchProtocol::reset(uint32_t deviceId, PacketBuffer &prevSig) {
    reset(deviceId);
    setLastSignature(prevSig);
}

bool CryptoUbirchProtocol::setLastSignature(PacketBuffer &prevSig) {
    if (prevSig.length() != UBIRCH_PROTOCOL_SIGN_SIZE) return false;
    memcpy(proto.signature, prevSig.getBytes(), UBIRCH_PROTOCOL_SIGN_SIZE);
    return true;
}

PacketBuffer CryptoUbirchProtocol::getLastSignature() {
    return PacketBuffer(proto.signature, UBIRCH_PROTOCOL_SIGN_SIZE);
}


PacketBuffer CryptoUbirchProtocol::createSignedMessage(ManagedString message) {
    startMessage();

    msgpack_pack_raw(&pk, static_cast<size_t>(message.length()));
    msgpack_pack_raw_body(&pk, message.toCharArray(), static_cast<size_t>(message.length()));

    return finishMessage();
}

CryptoUbirchProtocol& CryptoUbirchProtocol::startMessage() {
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer_init(&pk, &proto, ubirch_protocol_write);
    ubirch_protocol_start(&proto, &pk);
    return *this;
}

PacketBuffer CryptoUbirchProtocol::finishMessage() {
    ubirch_protocol_finish(&proto, &pk);

    // create a packetbuffer from the buffer and free sbuf
    PacketBuffer buf(reinterpret_cast<uint8_t *>(sbuf.data), sbuf.size);
    msgpack_sbuffer_destroy(&sbuf);

    return buf;
}

CryptoUbirchProtocol& CryptoUbirchProtocol::addString(ManagedString &value) {
    msgpack_pack_raw(&pk, static_cast<size_t>(value.length()));
    msgpack_pack_raw_body(&pk, value.toCharArray(), static_cast<size_t>(value.length()));
    return *this;
}

CryptoUbirchProtocol& CryptoUbirchProtocol::addInt(const int value) {
    msgpack_pack_int(&pk, value);
    return *this;
}

CryptoUbirchProtocol& CryptoUbirchProtocol::addMap(int n) {
    msgpack_pack_map(&pk, static_cast<size_t>(n));
    return *this;
}

CryptoUbirchProtocol& CryptoUbirchProtocol::addArray(int n) {
    msgpack_pack_array(&pk, static_cast<size_t>(n));
    return *this;
}

CryptoUbirchProtocol& CryptoUbirchProtocol::addInt(ManagedString key, int value) {
    addString(key);
    msgpack_pack_int(&pk, value);
    return *this;
}

CryptoUbirchProtocol& CryptoUbirchProtocol::addString(ManagedString key, ManagedString value) {
    addString(key);
    msgpack_pack_raw(&pk, static_cast<size_t>(value.length()));
    msgpack_pack_raw_body(&pk, value.toCharArray(), static_cast<size_t>(value.length()));
    return *this;
}

CryptoUbirchProtocol& CryptoUbirchProtocol::addMap(ManagedString key, int n) {
    addString(key);
    addMap(n);
    return *this;
}

CryptoUbirchProtocol& CryptoUbirchProtocol::addMap(int key, int n) {
    addInt(key);
    addMap(n);
    return *this;
}

CryptoUbirchProtocol& CryptoUbirchProtocol::addArray(ManagedString key, int n) {
    addString(key);
    addArray(n);
    return *this;
}

CryptoUbirchProtocol &CryptoUbirchProtocol::addMsgPack(char *buf, size_t len) {
    msgpack_pack_raw_body(&pk, buf, len);
    return *this;
}

PacketBuffer  CryptoUbirchProtocol::createKeyRegistration(unsigned char *key, unsigned int notBefore, unsigned int notAfter) {
    // initialize ubirch protocol
    memset(&proto, 0, sizeof(proto));
    ubirch_protocol_init(&proto, proto_signed, UBIRCH_PROTOCOL_TYPE_REG,
                         &sbuf, msgpack_write_dal, ed25519_sign, hardwareSerial);
    startMessage();

    ubirch_key_info info = {};
    info.algorithm = const_cast<char *>(UBIRCH_KEX_ALG_ECC_ED25519);
    info.created = notBefore;
    memcpy(info.hwDeviceId, hardwareSerial, sizeof(info.hwDeviceId));
    memcpy(info.pubKey, key, crypto_sign_PUBLICKEYBYTES);
    info.validNotAfter = notAfter;
    info.validNotBefore = notBefore;
    msgpack_pack_key_register(&pk, &info);

    return finishMessage();
}


int CryptoUbirchProtocol::msgpack_write_dal(void *data, const char *buf, size_t len) {
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








