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
 * The ubirch-protocol. This class has state and needs to be reset if a new chain of messages
 * should be started.
 */
class CryptoUbirchProtocol {

public:
    /**
     * Initialize ubirch protocol with the device Id.
     * @param deviceId the hardware device id
     */
    explicit CryptoUbirchProtocol(uint32_t deviceId = microbit_serial_number());

    /**
     * Reset the ubirch-protocol, using the given device Id, resets the last signature.
     * @param deviceId
     */
    void reset(uint32_t deviceId);

    /**
     * Reset the ubirch-protocol using the given device Id and the last signature.
     * @param deviceId the hardware device id
     * @param prevSig the previous signature to start with
     */
    void reset(uint32_t deviceId, PacketBuffer &prevSig);

    /**
     * Set the last signature, possibly reading it from a non-volatile memory
     * @param prevSig the previous signature bytes
     * @return true if the length is correct and the signature was set
     */
    bool setLastSignature(PacketBuffer &prevSig);

    /**
     * Return the last generated signature into the prevSig parameter.
     * @return a packet buffer with the last signature generated
     */
    PacketBuffer getLastSignature();

    /**
     * Start a new chained packet using the ubirch-protocol.
     * Finish using #finishMessage
     */
    void startMessage();

    /**
     * Finish the message, add signature and return as a packet buffer.
     * @return the message in a packet buffer
     */
    PacketBuffer finishMessage();

    /**
     * Create a new signed message using the ubirch-protocol.
     * @param message the message to sign
     * @return a packet buffer containing the bytes of the packet
     */
    PacketBuffer createSignedMessage(ManagedString message);

    /**
     * Add a single integer.
     * @param value the value
     */
    void addInt(int value);

    /**
     * Add a single string.
     * @param value the value
     */
    void addString(ManagedString &value);

    /**
     * Add a key-value map with n entries.
     * @param n the number of entries
     */
    void addMap(int n);

    /**
     * Add an array with n entries
     * @param n the number of array elements
     */
    void addArray(int n);

    /**
     * Add a string map entry with name key and the value.
     * @param key the name of the entry
     * @param value the value
     */
    void addString(ManagedString key, ManagedString value);

    /**
     * Add an integer map entry with name key and the value.
     * @param key the name of the entry
     * @param value the value
     */
    void addInt(ManagedString key, int value);

    /**
     * Add a map inside of a map with the name and n entries.
     * @param key the name of the entry
     * @param n the number of entries
     */
    void addMap(ManagedString key, int n);

    /**
     * Add a map inside of a map with with the integer key and n entries (for time series).
     * @param key the integer that is the key
     * @param n number of entries
     */
    void addMap(int key, int n);

    /**
     * Add an array map entry with the key and n entries
     * @param key the name of the entry
     * @param n the number of array elements
     */
    void addArray(ManagedString key, int n);


protected:
    /**
     * A special write function that writes into an sbuffer. Override if you
     * want to directly stream data or write to another place.
     * @param data the sbuffer data
     * @param buf the data to write
     * @param len the amount of data to write
     * @return 0 if successful
     */
    static int msgpack_write_dal(void *data, const char *buf, size_t len);

private:
    msgpack_sbuffer sbuf;   //!< local stream buffer
    ubirch_protocol proto;  //!< ubirch-protocol structure
    msgpack_packer pk;      //!< the packer used to assemble the message

};