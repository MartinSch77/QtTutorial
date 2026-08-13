# Tutorial: Networking with QTcpServer / QTcpSocket

This walks through `framework-tour/04-networking/`: a self-contained JSON
echo demo that exercises real TCP sockets end to end, and the Qt Network
concepts it demonstrates.

## 1. Why raw sockets instead of QHttpServer?

Qt does have a higher-level `QHttpServer` module for building HTTP
endpoints. It is not part of every Qt 6 installation, though (it ships
separately and was not available in the Qt 6.4.2 installation this tutorial
targets), whereas `QTcpServer`/`QTcpSocket` (Qt Network) are available
wherever Qt Network is. More importantly for a tutorial: working directly
with sockets forces you to confront the things a protocol actually needs to
handle — framing messages inside a byte stream, coping with partial reads,
matching asynchronous responses back to requests — instead of having an HTTP
library hide them.

To keep the demo a single, runnable executable (no need to launch two
processes and coordinate ports), the app starts both ends: a `QTcpServer`
listening on `127.0.0.1` on an ephemeral, OS-assigned port, and a
`QTcpSocket` client that immediately connects to it over loopback.

## 2. A newline-delimited JSON protocol

`src/Protocol.h`/`.cpp` defines two plain structs, `Request` and `Response`,
and pure functions to convert them to/from `QByteArray`:

```cpp
QByteArray encodeRequest(const Request& request);
std::optional<Request> decodeRequest(const QByteArray& line);
```

Each encoded message is a single `QJsonDocument::toJson(QJsonDocument::Compact)`
object followed by `'\n'`. This is a common, simple wire format: JSON objects
are self-delimiting, and `'\n'` never appears inside compact JSON text, so a
reader only needs to buffer bytes until it sees a newline, then hand that
slice to `QJsonDocument::fromJson()`. There's no explicit length prefix to
compute or validate, which keeps both the encoder and decoder a few lines
long — deliberately, this is the simplest protocol that is still "real":
self-framing, versionable (add fields, old decoders ignore them), and
readable in a log.

Crucially, none of this code touches `QObject`, `QTcpSocket`, or an event
loop, which is what makes it directly unit-testable (see step 5).

## 3. `QTcpServer`: accept, buffer, and reply

`src/EchoServer.h`/`.cpp` subclasses `QTcpServer`. The constructor connects
its `newConnection()` signal to `handleNewConnection()`, which drains
`hasPendingConnections()`/`nextPendingConnection()` and, for each new
`QTcpSocket`, wires up `readyRead()` and `disconnected()`.

The important subtlety is that `readyRead()` gives you *whatever bytes
happened to arrive*, not necessarily one whole message and not necessarily
only one message. `EchoServer` keeps a per-socket `QByteArray` buffer
(`QHash<QTcpSocket*, QByteArray> m_buffers`) and, on every `readyRead()`,
appends the newly available bytes, then peels off and decodes complete lines
in a loop:

```cpp
int newlineIndex = 0;
while ((newlineIndex = buffer.indexOf('\n')) != -1) {
    const QByteArray line = buffer.left(newlineIndex);
    buffer.remove(0, newlineIndex + 1);
    // decode line, build and write a response
}
```

For each decoded request the server builds a `Response` carrying the same
`id`, its own `QDateTime::currentMSecsSinceEpoch()` timestamp, and an `echo`
string, encodes it, and writes it straight back on the same socket.

## 4. The client side and latency measurement

`MainWindow` owns both the `EchoServer` and a client `QTcpSocket`. On
construction it calls `m_server->listen(QHostAddress::LocalHost)` (port `0`
implicitly — Qt assigns an ephemeral one) and then
`m_clientSocket->connectToHost(QHostAddress::LocalHost, m_server->serverPort())`.

Every click of "Send request":

1. Assigns the next monotonically increasing `id`.
2. Starts a `QElapsedTimer` and stores it in `QHash<int, QElapsedTimer>
   m_pendingRequests` keyed by `id` — this is what measures the round trip,
   independent of wall-clock timestamps that could be affected by clock
   adjustments.
3. Writes the encoded request to the socket.

The client's `readyRead()` handler mirrors the server's buffering loop
exactly (peel off `'\n'`-terminated lines, decode each), but on a successful
decode it looks up `m_pendingRequests[response.id]`, reads `.elapsed()` for
the latency in milliseconds, removes the entry, appends a log line, and
updates a running total/count used to display both the latest and the
average latency.

## 5. Testing sockets without flaky sleeps

`tests/framework-tour/04-networking/test_networking_protocol.cpp` has two
kinds of tests:

- Pure round-trip tests that call `encodeRequest`/`decodeRequest` and
  `encodeResponse`/`decodeResponse` directly — no sockets, no event loop, so
  they're as fast and deterministic as any other unit test.
- An end-to-end test that starts a real `EchoServer` on
  `QHostAddress::LocalHost` with port `0`, reads back the assigned port via
  `server.serverPort()`, connects a `QTcpSocket` client with
  `waitForConnected()`, writes a request, and then waits for the response
  with `QTRY_VERIFY(client.canReadLine())` rather than a fixed `QTest::qWait`
  or a manual sleep loop. `QTRY_VERIFY` re-checks its condition on a short
  timer until it's true or a timeout elapses, which is what makes the test
  robust against normal scheduling jitter without being slow in the common
  case.

## 6. Try it yourself

- Add a `type` field to the request/response JSON and branch on it in
  `EchoServer::handleReadyRead()` to support more than one kind of message
  (e.g. an "add" request that sums two numbers server-side).
- Make the server intentionally delay its reply (e.g. `QTimer::singleShot`)
  to see the latency label and log respond to a slower, more realistic
  round trip.
- Have the client send several requests back-to-back before any response
  arrives, and confirm they're still matched up correctly by `id` — this
  exercises exactly the buffering/framing logic described in step 3, now
  from the client's perspective.
