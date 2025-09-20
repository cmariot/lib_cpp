# libftpp — a small C++ toolbox

Light-weight, self-contained C++ toolbox used for learning and testing advanced C++ concepts.

This repository builds a static library `libftpp.a` and exposes a single umbrella header `libftpp.hpp` for consumers.

## Purpose

The goal of this project is to provide a curated set of reusable C++ utilities and data structures (pools, buffers, design patterns, etc.) implemented without external dependencies. It's organized to be easy to extend and to serve as a teaching / lab project.

## Implementations

- `documentation/data_structures/pool.md` — describes `Pool<T>`

  Overview
  - Purpose: a lightweight object pool for manual allocation/reuse of objects of type T.
  - When to use: reduce allocation churn for frequently created/destroyed objects, implement object recycling in tests or performance-sensitive code.

  Key concepts
  - acquire(): returns a reference to a pooled object (caller must initialize if needed).
  - release(obj): returns the object to the pool for reuse; does not call destructor unless configured.
  - Lifetime: pool manages raw storage; callers are responsible for object construction/destruction semantics as documented.

  Usage example

  ```cpp
  Pool<MyType> pool(32);            // reserve capacity
  auto &obj = pool.acquire();       // obtain an object reference
  new (&obj) MyType(args...);       // placement-new initialize if required
  // use obj...
  obj.~MyType();                    // optional manual cleanup
  pool.release(obj);                // return to pool
  ```

  Implementation notes
  - The implementation uses contiguous storage and an internal free-list to track available slots.
  - Templates follow header/.tpp pattern: see includes/data_structures/pool.hpp and pool.tpp.
  - Be careful when changing ownership semantics — tests rely on manual construct/destruct behavior.

- `documentation/data_structures/data_buffer.md` — describes DataBuffer

  Overview
  - Purpose: a simple binary buffer for serialization and deserialization of POD and trivially-copyable data.
  - When to use: efficient packing/unpacking of binary messages for tests, demos, or small IPC utilities.

  Key features
  - `append<T>(const T&)`: append raw bytes of T (T should be trivially copyable).
  - `read<T>(offset)`: read a value at a given offset (returns T by value).
  - growable contiguous storage with explicit size/position management.

  Usage example

  ```cpp
  DataBuffer buf;
  uint32_t x = 42;
  buf.append(x);
  buf.append(std::string_view("hi")); // uses length+bytes or raw bytes per implementation
  auto y = buf.read<uint32_t>(0);
  ```

  Implementation notes
  - DataBuffer stores a contiguous std::vector<uint8_t> and exposes append/read helpers.
  - Endianness and non-trivially-copyable types are not handled automatically — convert or serialize manually.
  - Check tests under tests/data_structures/ for expected semantics and examples.

For implementation details and examples, consult the corresponding headers and .tpp files in includes/data_structures and the tests that demonstrate correct usage.

Refer to those files for implementation details and examples when working on the corresponding modules.

## Repository layout

- `includes/` — public headers, grouped by feature (e.g. `data_structures/`, `design_patterns/`).
- `sources/` — implementation `.cpp` files compiled into the static archive.
- `objects/` — generated object files (build output).
- `tests/` — unit tests and test utilities:
  - `tests/framework/` — small custom libunit test harness used to register/run tests and produce per-test logs.
  - `tests/<suite>/` — test suites, each file should expose `extern "C" int <name>(void)` (see below).
- `Makefile` — top-level build, test and cleaning rules.
- `.github/` — repository prompts/instructions for contributors and tools (copilot instructions are present here).

## Build & test (exact)

All commands run from the project root.

- Build the library: `make all` (produces `libftpp.a`).
- Build and run tests (framework auto-built if needed): `make tests`.
- Run tests without rebuilding: `make test-only`.
- Clean build artifacts: `make clean`.
- Remove library, test binaries and generated logs: `make fclean`.

Notes:

- `make tests` runs `tests/generate_launcher.py` to auto-generate `tests/tests_launcher.cpp`. Do not edit the generated launcher manually; add or edit test files instead.
- Tests use `tests/framework/libunit.a`; the top-level `Makefile` will build that framework if missing.

## How to add a test

1. Create a new folder under `tests/` for the suite, for example `tests/my_feature/`.
2. Add a `.cpp` file that exposes an `extern "C"` test function. Example:

    ```cpp
    extern "C" int my_feature_basic_test(void) {
        // Use helpers from tests/test_utils.hpp (ASSERT_TRUE/ASSERT_EQ return non-zero on failure)
        return 0; // return 0 on success
    }
    ```

3. Run `make tests` — the generator will detect the `extern "C"` symbol or any function ending with `_test`, generate the launcher and run the test runner.

## Conventions & patterns to follow

- Umbrella header: `libftpp.hpp` should include the public headers required to use the library.
- Templates follow `header.hpp` + `header.tpp` pattern (implementations in `.tpp` and included at end of header). Example: `includes/data_structures/pool.hpp` + `pool.tpp`.
- Memory and lifetime: several utilities (Pool, DataBuffer) do manual construction/destruction using `operator new`/`delete` and placement new. Be careful about ownership and lifetimes when modifying these modules (see `pool.tpp` for the intended semantics of `acquire()`/`release()`).
- No external libraries: projects must not depend on third-party packages; only the C++ standard library is allowed.

## Test framework specifics

- Per-test logs are created by the framework as `tests/<function>_<test>.log` (framework writes logs via `create_log_file.cpp`).
- The top-level `fclean` removes logs under `tests/` to keep the repository clean.

## Compiler & portability

- The repository compiles with `-std=c++23` by default (see `Makefile`), but `CXX` and `CXXFLAGS` can be overridden in the environment.
- The test framework Makefile (`tests/framework/Makefile`) uses `CXX ?= g++` and `CXXFLAGS ?=` so you can inject different compilers/flags from the top-level environment.
