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

#include <ManagedString.h>
#include <ubirch/ubirch_protocol.h>

/**
 * stores the last used signature
 */
extern unsigned char prev_sig[UBIRCH_PROTOCOL_SIGN_SIZE];

/**
 * A special msgpack write function that takes some peculiarities of the
 * microbit-dal into account to prevent memory leaks.
 * @param data the target data buffer
 * @param buf the new data buffer
 * @param len the length of the new data
 * @return
 */
inline int msgpack_write_dal(void *data, const char *buf, size_t len);

/**
 * Create a new signed packet using the ubirch-protocol.
 * @param message the message to sign
 * @return a packet buffer containing the bytes of the packet
 */
PacketBuffer createSignedPacket(ManagedString message);