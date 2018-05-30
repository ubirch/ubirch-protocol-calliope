#! /usr/bin/env python3

import msgpack
import ed25519
import hashlib

# this is the key registration message printed on stdout after starting up
messageHex = "95cd0012b000000000000000000000000039b29abc0186a9616c676f726974686dab4543435f45443235353139a763726561746564ce5b0ef871aa68774465766963654964b000000000000000000000000039b29abca67075624b6579da002067bf98685f6d9cbc93e18372521cbdaa2b4b18b9fdb6d0dd8f7d1df43d6bacc3ad76616c69644e6f744166746572ce5cf02bf0ae76616c69644e6f744265666f7265ce5b0ef871da004091d9bb8180e28eba450c81ca6d51eab43716ca2f752d89eacfea3cadf3b8d4e3f3b6d270b758d058f44f4c19f3edc2741f1a99fcfa5a4bf780c0d4c671eae109"
message = bytes.fromhex(messageHex)

unpacked = msgpack.unpackb(message)
signature = unpacked[4]
publicKey = unpacked[3][b'pubKey']
vk = ed25519.VerifyingKey(publicKey)

try:
    tohash = message[0:-67]
    hash = hashlib.sha512(tohash).digest()
    print("message hash: {}".format(hash.hex()))
    vk.verify(signature, hash)
    print("key registration is valid")
except Exception as e:
    print("key registration is invalid")
