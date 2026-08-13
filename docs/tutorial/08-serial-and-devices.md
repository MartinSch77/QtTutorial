# 08 — Serial & Devices

This step covers talking to byte-oriented devices with Qt: discovering real serial
ports, defining a small framing protocol on top of a byte stream, parsing that
protocol incrementally, and handling timeouts when a device doesn't answer.

Module source: `framework-tour/08-serial-and-devices/`
Tests: `tests/framework-tour/08-serial-and-devices/`

## An upfront honesty note

The build/test environment for this tutorial has no physical serial hardware
attached. Rather than fake that away, this module is explicit about the boundary:

- Port discovery (`QSerialPortInfo::availablePorts()`) is real. It will legitimately
  report zero ports if you run this tutorial on a machine without serial hardware.
- The protocol logic (framing, checksums, incremental parsing, timeouts) is real,
  hardware-independent, and unit-tested on its own.
- The actual "wire" used to exercise that protocol logic end-to-end is a
  **simulation**: two connected `QLocalSocket` endpoints standing in for a serial
  cable. It is clearly named and documented as a simulation (`SimulatedSerialLink`),
  and the protocol code is written so it would run unchanged against a real
  `QSerialPort`.

If you have real hardware, see the README in the module directory for how to swap
`SimulatedSerialLink` for a real `QSerialPort`.

## 1. Discovering real ports

`QSerialPortInfo::availablePorts()` returns a `QList<QSerialPortInfo>` describing
whatever serial devices the OS currently sees. `MainWindow::rescanPorts()` calls it
directly:

```cpp
const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
if (ports.isEmpty()) {
    m_portList->addItem(tr("No physical serial ports detected on this system"));
    return;
}
for (const QSerialPortInfo& info : ports) {
    m_portList->addItem(tr("%1 (%2)").arg(info.portName(), info.description()));
}
```

There is no fallback to fake ports here — an empty result is reported as exactly
that.

## 2. Defining a frame format

Serial links and sockets alike only guarantee you a stream of bytes; you need your
own framing to know where one message ends and the next begins, and whether a
message arrived intact. `FrameCodec.h` defines:

```
[STX][LEN][PAYLOAD x LEN][CHECKSUM][ETX]
```

`STX`/`ETX` are marker bytes, `LEN` is the payload size, and `CHECKSUM` is an
XOR-fold over the payload, computed by the pure function `computeChecksum()`.
`encodeFrame()` builds a complete frame from a payload.

Because `computeChecksum()` and `encodeFrame()` are pure functions with no Qt event
loop or I/O involved, they're trivial to unit test directly.

## 3. Parsing frames incrementally

Real byte streams don't hand you one whole frame at a time — a single `readyRead()`
might contain half a frame, several frames, or a frame plus the start of the next
one. `FrameParser::feed()` is written to cope with that: it appends whatever bytes
it's given to an internal buffer, then repeatedly tries to extract complete frames
from the front of that buffer.

- If the buffer doesn't yet contain a full frame, `feed()` simply returns and waits
  for more bytes on the next call — nothing is emitted yet.
- If a full frame is present but its checksum doesn't match, `frameError()` is
  emitted with a diagnostic message, and the parser moves on to look for the next
  frame — it does not crash or get stuck.
- If a full frame is present and its checksum matches, `frameDecoded()` is emitted
  with the decoded payload.

The unit tests feed a single encoded frame into `FrameParser` one byte at a time to
prove this incremental behavior actually works, not just the all-at-once case.

## 4. Timeouts

Sending a message and getting no reply is a normal failure mode for any device link
— a serial cable can be unplugged, a device can be busy or crashed. `DeviceLink`
wraps a `QIODevice*` (any `QIODevice`, so `QLocalSocket` and `QSerialPort` both
qualify) together with a `FrameParser` and a single-shot `QTimer`:

```cpp
void DeviceLink::sendFrame(const QByteArray& payload)
{
    m_transport->write(encodeFrame(payload));
    m_timer->start();
}
```

If a valid frame is decoded before the timer fires, the timer is stopped and
`frameReceived()` is emitted. If not, `timedOut()` fires instead. This is real,
independently testable logic — the unit tests exercise it by intentionally not
answering a sent frame and asserting `timedOut()` fires within the expected window,
using `QTRY_COMPARE` rather than a sleep.

## 5. Simulating the wire without pretending it's real

`SimulatedSerialLink` creates a `QLocalServer` and connects two `QLocalSocket`
endpoints to each other — a real, asynchronous, byte-oriented local IPC channel,
standing in for a serial cable. Its header comment says, in plain terms, that this is
a simulation of the transport layer only. Both endpoints are exposed as
`QLocalSocket*`, and since `QLocalSocket` is a `QIODevice`, the exact same
`DeviceLink`/`FrameParser` code that runs against the simulation would run against a
real `QSerialPort` without modification.

## 6. Seeing it work in the UI

The app window has two panels:

- **Real hardware scan**: the "Rescan ports" button calls
  `QSerialPortInfo::availablePorts()` and shows the honest result.
- **Simulated link**: "Send framed message" sends a message through the simulated
  wire and shows the device end decoding it and echoing it back. "Send corrupted
  frame" deliberately flips a checksum byte before sending — the log shows the
  device end rejecting it via `frameRejected()`, and, since no echo follows, the
  controller end's own `timedOut()` firing shortly after.

## 7. Running the tests

```sh
ctest -R test_serial_and_devices
```

The test binary covers: the checksum function directly; the incremental parser fed
one byte at a time and fed multiple frames at once; checksum-mismatch handling;
`SimulatedSerialLink` moving bytes end-to-end; a full `DeviceLink` echo round trip;
and the timeout path when nothing answers.
