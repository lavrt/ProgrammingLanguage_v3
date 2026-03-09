# Programming Language

A small programming language implemented in C++20 with a classic split into:

- **Frontend**: tokenizer + recursive-descent parser → builds an AST  
- **Backend**: AST → x86-64 machine code packaged into a Linux ELF executable

The default build pipeline runs the frontend on ```./examples/code.rt```, serializes the AST into ```./tmp/```, then runs the backend to generate a Linux ELF binary at ```./bin/output.elf```.

---

## Project layout

- `frontend/` — tokenizer + parser (builds AST, serializes it)   
- `backend/` — code generator (reads serialized AST and emits an ELF executable)  
- `tree/` + `common/` — AST implementation, serialization/deserialization, helpers 
- `examples/` — sample source program(s), e.g. `code.rt`
- `compile.sh` — one-command compile pipeline

---

## Requirements

- **Linux x86-64** (output is an ELF executable)
- **CMake**
- **C++20**

---

## Build

#### Get the code

```bash
git clone https://github.com/lavrt/ProgrammingLanguage_v3.git
cd ProgrammingLanguage_v3
```

#### Configure and build the project:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

#### Run (compile example program):

```bash
./compile.sh
```

This script will:

1) create ```./bin``` and ```./tmp``` if they don't exist

2) run ```./build/frontend/frontend```

3) run ```./build/backend/backend```

4) mark ```./bin/output.elf``` as executable and print its path

#### Run the produced executable:

```bash
./bin/output.elf
```

#### Dump AST and generate a graph (Graphviz)

After running the frontend, you can dump the AST:
```cpp
ast.Dump("./tmp/filename");
```

Convert the dump to an image with Graphviz:
```bash
dot -Tpng ./tmp/filename.dot -o ./tmp/filename.png
```

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

- Control flow: ```if (<cond>) <stmt>``` and ```while (<cond>) <stmt>```

- Blocks: ```{ stmt; stmt; ... };``` (statements separated by ```;```)

- I/O builtins: ```read_int()```, ```print_int(x)```, ```print_ascii(x)```

---

## Example

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

    res_2 = call factorial_2(c);
    print_int(res_2);
};

def main() {
    a = read_int();
    call test_factorials(a);
};
```
