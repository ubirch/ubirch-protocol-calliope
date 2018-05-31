# ubirch-protocol for Calliope mini and micro:bit

This wraps the [ubirch-protocol](https://github.com/ubirch/ubirch-protocol) to be used with the 
[microbit-dal](https://github.com/calliope-mini/microbit-dal).

## Example

See [example/main.cpp](example/main.cpp) for a working example:

```cpp
ubirch.startMessage();
ubirch.addMap("data", 1);
ubirch.addMap((int) time, 2);
ubirch.addInt("t", uBit.thermometer.getTemperature());
ubirch.addInt("l", uBit.display.readLightLevel());
PacketBuffer packet = ubirch.finishMessage();
```