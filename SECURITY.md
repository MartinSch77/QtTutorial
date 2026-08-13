# Security Policy

## What this repository is

QtTutorial is example/teaching software: a Qt 6 / C++23 framework tour and a
set of small industry-flavored showcase apps. It does not handle real user
data, does not connect to production systems, and is not deployed as a
service anywhere. Please calibrate expectations accordingly — this is not a
product with a security team, an incident response process, or a bug bounty,
and it would be overclaiming to pretend otherwise.

That said, "teaching code" is not an excuse for careless code, and reports
about real issues (a memory-safety bug an example program has, a dependency
with a known CVE, a credential accidentally committed) are welcome and will
be looked at.

## What we do and don't ask you to trust

- Don't enter real secrets, API keys, or personal data into any
  framework-tour module or industry example — none of them are designed to
  handle sensitive data safely, and several (the networking and SQL
  examples in particular) are intentionally minimal/didactic rather than
  hardened.
- The sanitizer (`.github/workflows/sanitizers.yml`) and static analysis
  (`.github/workflows/static-analysis.yml`) CI jobs exist to catch memory
  safety and correctness issues as part of normal development, not as a
  formal security audit process.

## Reporting a vulnerability or security-relevant bug

If you find a security-relevant issue (memory corruption, an unsafe
deserialization pattern in one of the networking/SQL examples, a dependency
with a known vulnerability, a leaked credential in the repo history, etc.):

1. Open a GitHub issue on this repository describing the problem. Since this
   is example code with no production deployment and no user data at stake,
   there's no need for a private disclosure channel — a public issue is
   fine for essentially anything you're likely to find here.
2. If you believe the issue is sensitive enough to warrant not filing it
   publicly first (for example, if it would also affect other projects that
   copy code from here), you can instead contact the repository owner
   directly through their GitHub profile before filing a public issue.
3. Include enough detail to reproduce: which framework-tour module or
   industry example, what input or configuration triggers it, and — if
   applicable — which sanitizer (ASan/UBSan) or static analyzer flagged it.

There is no bug bounty program and no guaranteed response time — this is a
best-effort teaching project, not a maintained product.

## Dependencies

This repository depends on Qt 6 (see `docs/qa/licensing.md` for licensing
details) and standard C++23. It does not vendor any third-party libraries
beyond Qt itself. If a Qt release you're using has a known CVE, that's a Qt
issue to track upstream (https://www.qt.io/product/qt6/security) rather than
something this repository can independently patch.
