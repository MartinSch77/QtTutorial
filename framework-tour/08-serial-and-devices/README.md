# 08 - Serial & Devices

## What this module actually does (read this first)

This module does **not** talk to real hardware in the environment it was built
and tested in (a sandboxed container with no physical serial ports attached).
Being upfront about that:

- `QSerialPortInfo::availablePorts()` is called for real, in `MainWindow::rescanPorts()`.
  On a machine with no serial hardware — like the container this was built in — it
  legitimately returns an empty list, and the UI says so honestly
  ("No physical serial ports detected on this system") instead of hiding that fact
  or inventing fake ports.
- All of the framing, checksum, and timeout logic (`FrameCodec`, `FrameParser`,
  `DeviceLink`) is real, general-purpose code with no hardware dependency, and it is
  unit-tested directly (see `tests/framework-tour/08-serial-and-devices/`).
- The wire itself — the thing that actually moves bytes between a "controller" and a
  "device" — is **simulated**: `SimulatedSerialLink` sets up a real `QLocalServer`
  plus two connected `QLocalSocket` endpoints (genuine local IPC, asynchronous and
  byte-oriented, not a synchronous in-memory shortcut) and hands out both ends. Its
  class doc comment in `src/SimulatedSerialLink.h` says SIMULATED/loopback explicitly
  so nobody mistakes it for real hardware I/O.

## Why this design is honest and still useful

`QLocalSocket` is a `QIODevice`, exactly like `QSerialPort` is. `DeviceLink` and
`FrameParser` are written against `QIODevice*`/`QIODevice&`, not against
`QLocalSocket` or `QSerialPort` specifically. That means the framing/checksum/timeout
logic exercised in this module's tests over the simulated loopback would run
unmodified over a real serial cable.

## Swapping in real hardware

If you have actual serial hardware attached, you can replace the simulated wire with
a real one with a small, localized change:

```cpp
// Instead of:
auto* link = new SimulatedSerialLink(this);
auto* controllerLink = new DeviceLink(link->clientEnd(), 800, this);

// Use a real port:
auto* port = new QSerialPort(QSerialPortInfo::availablePorts().first().portName(), this);
port->setBaudRate(QSerialPort::Baud9600);
port->open(QIODevice::ReadWrite);
auto* controllerLink = new DeviceLink(port, 800, this);
```

`DeviceLink` and `FrameParser` do not need to change at all — they only ever see a
`QIODevice*`. Everything specific to "this is a simulation" lives in
`SimulatedSerialLink`, which you simply stop using.

## Frame format

```
[STX][LEN][PAYLOAD x LEN][CHECKSUM][ETX]
```

- `STX` (0x02) / `ETX` (0x03): fixed marker bytes.
- `LEN`: one byte, 0-255, payload length.
- `CHECKSUM`: XOR-fold of the payload bytes, computed by `computeChecksum()`.

`FrameParser::feed()` is incremental: it can be called with arbitrary byte chunks
(a single byte at a time, several frames at once, or anything in between) and only
emits `frameDecoded()` once a complete, checksum-valid frame has fully arrived. A
structurally complete frame with a wrong checksum, or a frame with a bad terminator,
emits `frameError()` instead of crashing or silently hanging.

## Timeout handling

`DeviceLink::sendFrame()` starts a single-shot timer alongside the write. If a
complete valid frame is not decoded before it fires, `timedOut()` is emitted. This is
demonstrated live in the UI: the "Send corrupted frame" button writes a frame with a
deliberately flipped checksum byte onto the simulated wire. The simulated device end
rejects it (visible via a logged `frameRejected` message) and, because it therefore
never echoes anything back, the controller end's own timer fires and `timedOut()` is
logged too.
