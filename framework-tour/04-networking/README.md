# 04 – Networking (QTcpServer / QTcpSocket)

A self-contained JSON echo demo: a single application starts both a
`QTcpServer` (listening on `127.0.0.1` on an OS-chosen ephemeral port) and a
`QTcpSocket` client that connects to it over loopback. Clicking "Send
request" sends a small JSON message and the round trip through real TCP
sockets — client write, kernel loopback, server read/parse/reply, client
read/parse — is measured and shown in the log view with its latency.

## Why QTcpServer/QTcpSocket, not QHttpServer

Qt has a newer, higher-level `QHttpServer` module for building HTTP
endpoints, but it is a separate module that is not part of the local Qt 6.4.2
installation used to build this tutorial (confirmed via the Qt Documentation
lookup: only Widgets, Quick, QuickControls2, Quick3D, Network, Sql,
StateMachine, SerialPort, Svg, Concurrent, DBus and PrintSupport are
available). `QTcpServer`/`QTcpSocket` live in Qt Network, are available in
every Qt 6 installation, and are the right level of abstraction anyway for
demonstrating what a network protocol actually looks like on the wire:
accepting connections, framing messages, and handling partial reads — details
a higher-level HTTP client/server would hide.

## The line protocol

Each message is one JSON object followed by `\n` ("newline-delimited JSON").
No length prefix is needed because JSON objects are self-delimiting once you
know where they end, and `\n` is a byte that never occurs inside compact JSON
text — so both sides just buffer bytes until they see one.

Request (client → server):

```json
{"id": 3, "clientTimestampMs": 1755000000123}
```

Response (server → client):

```json
{"id": 3, "clientTimestampMs": 1755000000123, "serverTimestampMs": 1755000000125, "echo": "echo of request 3"}
```

`id` is a monotonically increasing counter assigned by the client so
responses (which can arrive out of order in general, though this demo's
single-threaded echo server happens to answer in order) can be matched back
to the pending request that triggered them.

## Structure

- `src/Protocol.{h,cpp}` (in `networking_lib`) — pure encode/decode functions
  for `Request`/`Response`, no `QObject`, no sockets. Fully unit-testable.
- `src/EchoServer.{h,cpp}` (in `networking_lib`) — a `QTcpServer` subclass
  that accepts connections, buffers partial reads per socket, and replies to
  each decoded request line with a response line.
- `src/MainWindow.{h,cpp}`, `src/main.cpp` — the Qt Widgets app: a "Send
  request" button, a `QPlainTextEdit` log of every request/response with its
  latency, and a label tracking the last and average round-trip latency.

## Build & run

From the repository root:

```sh
cmake -S . -B build
cmake --build build --target networking
./build/framework-tour/04-networking/networking
```

## Tests

`tests/framework-tour/04-networking/test_networking_protocol.cpp` covers the
`Protocol` encode/decode round trip directly, plus an end-to-end test that
starts an `EchoServer` on an ephemeral loopback port, connects a real
`QTcpSocket` client, sends a framed request, and verifies the parsed
response.
