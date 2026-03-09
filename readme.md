# Programming Language

A small programming language implemented in C++20.

The project is split into two main compilation stages:

- **Frontend**: tokenizer + recursive-descent parser &rarr; builds and serializes an AST
- **Backend**: reads the serialized AST &rarr; generates x86-64 machine code packaged into a Linux ELF executable

The repository also contains a small **driver** program (`compile`) that parses CLI arguments and runs the frontend and backend in sequence.


---

## Project layout

- `src/core/frontend/` — tokenizer, parser, frontend executable
- `src/core/backend/` — code generator, backend executable
- `src/core/tree/` — AST, serialization/deserialization, tree utilities
- `src/app/cli/` — command-line parsing
- `src/app/proc/` — process launching helpers
- `src/main.cpp` — driver program (`compile`)
- `examples/` — sample source programs
- `conanfile.txt` — Conan dependencies
- `CMakeLists.txt` — build configuration

---

## Requirements

- **Linux x86-64** (the backend generates a Linux ELF executable)
- **C++20**
- **CMake**
- **Conan 2**
- **Boost 1.90.0** (installed via Conan)

---

## Build

### 1. Clone the repository

```bash
git clone https://github.com/lavrt/ProgrammingLanguage_v3.git
cd ProgrammingLanguage_v3
```

### 2. Install dependencies with Conan

```bash
conan install . -of build -s build_type=Release --build=missing
```

### 3. Configure and build
```bash
cmake --preset conan-release
cmake --build --preset conan-release -j
```

---

## Usage

The main entry point is the driver executable:

```bash
./build/compile -i <input.rt> [-a <ast-file>] [-o <output.elf>]
```

### Options

- ```-i, --input``` — path to the input source file (required)

- ```-a, --ast``` — path to the serialized AST file (default: ./tmp/tree.txt)

- ```-o, --output``` — path to the output ELF file (default: ./bin/a.elf)

- ```-h, --help``` — show help and exit

### Example

Compile the sample program:

```bash
./build/compile -i ./examples/factorial.rt
```

Compile with custom output paths:

```bash
./build/compile -i ./examples/factorial.rt -a ./tmp/factorial.tree -o ./bin/factorial.elf
```

Run the produced executable:

```bash
./bin/a.elf
```

or, if you used a custom output path:

```bash
./bin/factorial.elf
```

---

## What the driver does

When you run ```compile```, it:

1. creates ```./tmp``` and ```./bin``` if they do not exist

2. runs the frontend executable

3. runs the backend executable

4. marks the produced ELF file as executable

At the moment, the driver launches the stage executables from these paths:

- ```./build/src/core/frontend/frontend```

- ```./build/src/core/backend/backend```

---

## Language overview

The language is minimal and currently centered around integers.

Supported constructs:

- Functions: ```def name(args...) { ... };```

- Function calls: ```call name(args...)```

- Return: ```return <expr>```

- Variables / assignment: ```x = <expr>```

- Arithmetic: ```+``` ```-``` ```*``` ```/```

- Comparisons: ```<``` ```<=``` ```>``` ```>=``` ```==``` ```!=```

- Control flow:
    - ```if (<cond>) <stmt>```
    - ```while (<cond>) <stmt>```

- Blocks: ```{ stmt; stmt; ... };```

- I/O builtins:
    - ```read_int()```
    - ```print_int(x)```
    - ```print_ascii(x)```

---

## Example program

```examples/factorial.rt``` computes factorial both recursively and iteratively:

```r
# calculating the factorial using a recursion function
def factorial_1(a) {
    if (a < 0) {
        return 0 - 1;
    };
    if ((a == 0) + (a == 1)) {
        return 1;
    };
    a2 = a - 1;
    a3 = call factorial_1(a2);
    return (a * a3);
};

# calculating the factorial using a loop
def factorial_2(a) {
    if (a < 0) {
        return 0 - 1;
    };
    b1 = 1;
    k2 = 1;
    while (b1 <= a) {
        k2 = k2 * b1;
        b1 = b1 + 1;
    };
    return k2;
};

def test_factorials(c) {
    res_1 = call factorial_1(c);
    print_int(res_1);
    print_ascii(10);

    res_2 = call factorial_2(c);
    print_int(res_2);
    print_ascii(10);
};

def main() {
    a = read_int();
    call test_factorials(a);
};
```

---

## Notes

- The output format is **Linux ELF**, so the generated binaries are intended to run on Linux x86-64.

- The project currently uses a serialized AST file as an interface between the frontend and backend.

- The build uses **Conan + CMake** for dependency management and compilation.
