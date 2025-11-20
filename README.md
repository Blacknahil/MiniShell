# MiniShell (C++23)

MiniShell is a personal exploration of how UNIX shells work under the hood. The goal is to build a small, understandable, POSIX‑leaning shell in modern C++ while iteratively adding features (parsing, builtins, job control, redirection, pipelines, scripting, etc.).

Current focus: a clean, minimal core that executes external programs and a handful of builtin commands reliably.

## Implemented Features (so far)

- Prompt loop (`$`) with line input
- Tokenizer with basic single & double quote handling
- Builtins: `echo`, `exit <status>`, `type`, `pwd`, `cd` (including `cd ~`)
- External command execution via `fork + execvp`
- Path lookup using `$PATH`
- Basic error messages for unknown commands & invalid `cd` targets

## 🔜 Near-Term Roadmap

- Improve quote/state machine & escaped characters
- Environment variable expansion (`$VAR`)
- I/O redirection (`>`, `>>`, `<`)
- Pipelines (`cmd1 | cmd2`)
- Command history & optional readline integration
- Signals & basic job control (foreground/background)
- Configuration & startup rc file
- Builtin improvements: `export`, `unset`, `which` alternative, `help`

## 🧱 Project Structure

```text
src/            # C++ source (entry point: main.cpp)
CMakeLists.txt  # Build definition (target: shell)
your_program.sh # Convenience wrapper to build & run locally
```

## 🔧 Build & Run

Requires: CMake (>=3.13), a C++23 capable compiler, and (optionally) `readline` (linked, may be leveraged later for history/editing).

### Standard build

```bash
mkdir -p build
cmake -S . -B build
cmake --build build
./build/shell
```

### Using the helper script

```bash
./your_program.sh
```

The script currently configures via CMake and then runs the produced `shell` binary.

## 🕹 Usage Examples

```bash
$ echo "Hello world"
Hello world

$ pwd
/current/working/directory

$ cd ~
$ pwd
/Users/yourname

$ type echo
echo is a shell builtin

$ type ls
ls is /bin/ls

$ exit 0
```

##  Design Notes

- Parsing currently uses a single pass tokenizer with quote state tracking; future iterations will separate lexing and parsing for extensibility.
- Builtins are detected via a simple vector lookup (`type` reports whether a name is builtin or resolved via `$PATH`).
- External execution uses `fork` then `execvp` with a temporary `char*` argv construction; later versions will manage memory more robustly.

##  Goals

Keep the codebase small, readable and educational—each feature added only after understanding its underlying system calls and edge cases.

##  Contributing / Feedback

This is a personal learning project. Ideas, suggestions, or critiques are welcome.

## 📄 License

TBD (will add a permissive license once the feature set stabilizes).

---

