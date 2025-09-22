## Quick orientation — libftpp (for AI agents)

Short, targeted guidance to get productive in this repository.

- Purpose: a small, self-contained C++ toolbox that builds a static archive `libftpp.a` and exposes a single umbrella header `libftpp.hpp`.
- Where to look: `includes/` (public headers), `sources/` (impl), `tests/` (unit suites + `tests/framework/` libunit harness), `Makefile` (top-level build/test rules).

Build & test (exact)
- Full library: `make all`
- Build and run tests: `make tests` (runs `tests/generate_launcher.py` first)
- Verbose build/tests: `make tests V=1`
- Rebuild from scratch: `make fclean && make tests`
- Run tests without rebuilding: `make test-only`

Test framework & logs
- `tests/generate_launcher.py` auto-generates `tests/tests_launcher.cpp`. Do NOT edit that generated file.
- Test discovery: any `extern "C" int name(void)` or functions named `*_test` are registered.
- Global log: `tests/logs/libftpp.log`. Per-test temporary files live in `tests/logs/tmp/`. Use `KEEP_TEST_TMP=1 make tests` to keep them.

Key code patterns (do not change lightly)
- Template pattern: headers with implementations in `.tpp` (e.g. `includes/data_structures/pool.hpp` + `pool.tpp`). Include the `.tpp` at the end of the header.
- Umbrella header: `libftpp.hpp` should re-export public headers under `includes/`.
- Manual lifetime: Pool/DataBuffer use manual placement-new/destruction semantics; changing ownership requires scanning tests and callers.

Quick troubleshooting checklist
- If a build error appears after header edits: run `make fclean && make tests V=1` to force a full rebuild and see linker errors.
- If tests show unexpected stdout/stderr: inspect `tests/logs/libftpp.log` and per-test files under `tests/logs/tmp/`.

Files to inspect first
- `Makefile` (root) — build flags, `V` verbosity, RUN helper.
- `includes/` — public API and `.tpp` template files.
- `tests/framework/` — the harness sources (`create_log_file.cpp`, `execute_test.cpp`, `print_test_output.cpp`) to understand logging, timeouts and output capture.

If you need to implement a feature change
- Add public headers under `includes/` and export them in `libftpp.hpp`.
- Add unit tests under `tests/<suite>/` as `extern "C" int name(void)` and run `make tests` to validate.

If anything is unclear or you want a deeper automation (unsubscribe tokens, thread-safe Observer, CI job), tell me which feature and I will implement headers, tests and docs.
## Quick orientation — libftpp (AI agent notes)

Purpose: small, self-contained C++ toolbox that builds a static archive `libftpp.a` and exposes a single umbrella header `libftpp.hpp`.

Where to look
- `includes/` — public headers grouped by feature (`data_structures/`, `design_patterns/`).
- `sources/` — implementation files compiled into `libftpp.a`.
- `tests/` — unit tests + custom test harness under `tests/framework/` (small libunit).
- `Makefile` (root) — orchestrates builds, tests and cleaning.

Fast build & test commands (exact)
- Full build: `make all`  # produces `libftpp.a`
- Build+run tests (generator runs automatically): `make tests` or `make tests V=1` for verbose
- Full rebuild (when headers change widely): `make fclean && make tests`
- Run existing binary only: `make test-only`

Important testing details
- `tests/generate_launcher.py` auto-generates `tests/tests_launcher.cpp`. Do NOT edit the generated file.
- Test discovery: any `extern "C" int name(void)` or functions named `*_test` are picked up.
- Test logging: global log `tests/logs/libftpp.log`; per-test tmp files live under `tests/logs/tmp/`. Use `KEEP_TEST_TMP=1 make tests` to keep temporary files for inspection.

Key code conventions
- Templates: header + `.tpp` implementation pattern (example: `pool.hpp` + `pool.tpp`). Include `.tpp` at end of header.
- Public API: add new public headers under `includes/` and export them via `libftpp.hpp`.
- Ownership: many utilities (Pool, DataBuffer) use manual lifetime management; changing this requires scanning usages and updating tests.

Common tasks & quick pointers
- Fix a linker/compile issue: find the symbol in `sources/` and its header in `includes/`, run `make tests V=1` to reproduce.
- Add a test: create `tests/<suite>/<your_test>.cpp` with `extern "C" int your_test(void)` and re-run `make tests`.
- Ensure a template compiles: edit both `.hpp` and matching `.tpp` and run a full rebuild.

Project-specific gotchas
- C++ standard: top-level Makefile uses `-std=c++23`.
- No external dependencies: everything links against `libftpp.a` and `tests/framework/libunit.a` only.
- Generated files: `tests/tests_launcher.cpp` is recreated by the generator; don't modify it manually.

Files to inspect first when onboarding
- `Makefile` (root) — build flags, V=1 verbosity, RUN helper.
- `includes/` — API headers and `.tpp` template files.
- `tests/framework/` — small test harness sources (create_log_file.cpp, execute_test.cpp, print_test_output.cpp) to understand logging/timeouts.

When you're unsure
- Run a full clean build (`make fclean && make tests V=1`) to reproduce compile/link failures.
- Read `tests/logs/libftpp.log` after `make tests` to inspect test stdout/stderr capture.

If you want a deeper change (unsubscribe tokens, thread-safe Observer, additional CI), say which feature to implement and I will create the headers, tests and small docs.

Doxygen documentation
---------------------
- Goal: produce discoverable API docs for each public header under `includes/` so contributors and students can read usage and design notes.
- Add Doxygen comments to public headers only (files under `includes/`); for templates document both the `.hpp` (API) and `.tpp` (implementation notes) where helpful.
- Recommended tags: `@file`, `@brief`, `@tparam`, `@param`, `@return`, `@code`/`@endcode`, `@note`, `@see`.
- Keep block comments simple (avoid nested `/* ... /* ... */` inside `@code` blocks). Prefer `@code{.cpp}` and small, self-contained examples.
- Examples to update first: `includes/data_structures/pool.hpp` + `pool.tpp`, `includes/data_structures/data_buffer.hpp`, `includes/design_patterns/observer.hpp`, `includes/design_patterns/memento.hpp`.
- Generation: add a `Doxyfile` at repo root (or a minimal script) and run:

```sh
# generate docs into docs/doxygen/
doxygen Doxyfile
```

- If no Doxyfile exists, use `doxygen -g Doxyfile` to create one and set `INPUT = includes` and `OUTPUT_DIRECTORY = docs/doxygen`.
- Keep examples short and focused; refer to real tests under `tests/` for usage snippets.
- After generating, commit only the Doxyfile and any small doc helpers; do NOT commit the generated HTML (prefer CI artifact publishing).

Quick follow-ups I can do
- Add `Doxyfile` template tuned for this repo (INPUT=includes, EXCLUDE=tests, RECURSIVE=YES) and a small `scripts/generate_docs.sh` wrapper.
- Run `doxygen` locally and verify documentation pages for `Pool`, `DataBuffer` and `Observer`.
## Quick orientation — libftpp

This file helps an AI coding agent become productive quickly in this repository. Keep it short and concrete: where to look, how to build/test, and project-specific conventions.

### Big picture
- Purpose: a small C++ toolbox library that builds a static archive `libftpp.a` and exposes a single umbrella header `libftpp.hpp`.
- Structure:
  - `includes/` — public headers (organized by feature, e.g. `data_structures/`, `design_patterns/`).
  - `sources/` — implementation files compiled into `libftpp.a`.
  - `tests/` — unit tests + a small custom `libunit` framework under `tests/framework/`.
  - `Makefile` (top-level) drives build, tests and cleaning.

### Build & test workflow (exact commands)
- Build library: `make all` (produces `libftpp.a`).
- Build and run tests (auto-builds the small framework if needed): `make tests`.
- Run only existing test binary: `make test-only`.
- Cleanup all build artifacts and test logs: `make fclean`.

Notes: `make tests` will run `tests/generate_launcher.py` to auto-generate `tests/tests_launcher.cpp` — do not edit that generated file manually.

### Key implementation patterns & conventions
- Public entry: `libftpp.hpp` should include all needed public headers. Prefer adding headers to `includes/` and referencing them from the umbrella header.
- Templates: header+`.tpp` pattern is used (e.g. `pool.hpp` + `pool.tpp`). Implementation of templates lives in `.tpp` and is included at the end of the header.
- Pool pattern: `includes/data_structures/pool.hpp` implements an object pool type. Note `acquire()` returns a reference to `Pool<T>::Object` and lifetime semantics are manual (caller must call `release`). See `pool.tpp` for correct placement/new usage.
- Test discovery: `tests/generate_launcher.py` scans `tests/` for `extern "C" int <name>(void)` or functions ending with `_test` and generates `tests/tests_launcher.cpp` which registers tests with `load_test(...)`.
- Test logs: `tests/framework` creates per-test logs named `<function>_<test>.log`. `Makefile`'s `fclean` removes `*.log` files.

### Files to inspect first for common tasks
- Fix a build/compile issue: inspect `Makefile` + `includes/` + `sources/` where the failing symbol is defined.
- Add a new test: create `tests/<suite>/<your_test>.cpp` exposing `extern "C" int your_test(void)` and re-run `make tests` (the generator will pick it up).
- Change a template class: edit `<header>.hpp` and `<header>.tpp` together; `.tpp` must be included at the end of the header.

### Project-specific rules & gotchas
- C++ standard: the project uses `-std=c++23` in the top-level `Makefile`. Use modern C++ features accordingly.
- No external libs: the project is self-contained; tests link `libftpp.a` + `tests/framework/libunit.a` and do not rely on third-party packages.
- Lifetime / ownership: many lower-level utilities (Pool, DataBuffer) use manual construction/destruction and `operator new`/`delete`; be careful when changing ownership semantics.
- Generated files: `tests/tests_launcher.cpp` is generated by `tests/generate_launcher.py`. Don't edit it — change tests instead.

### Examples (copyable snippets)
- Add a test function signature (save in `tests/my_suite/my_test.cpp`):
  ```cpp
  extern "C" int my_feature_test(void) {
      // use ASSERT-like helpers in tests/test_utils.hpp
      return 0; // 0 == success for libunit launcher
  }
  ```
- Generate launcher (manual): `python3 tests/generate_launcher.py` (Makefile runs this for you before building tests)

### Integration points
- Top-level Makefile: compiles `sources/` -> `objects/` -> `libftpp.a` and orchestrates test framework build.
- `tests/framework`: contains the small test harness and log generation (see `create_log_file.cpp`).

If anything here is unclear or you want a shorter/longer version, tell me which area to expand (build, tests, templates, or the Pool/Buffer lifetimes) and I will iterate.
