#include <vector>
#include <fstream>
#include <iostream>

#include "tree.h"
#include "generator.h"
#include "headers.h"

// static ------------------------------------------------------------------------------------------

static const char* const kNameOfOutputFile = "./bin/output.elf";

// global ------------------------------------------------------------------------------------------

int main() {
    Tree ast;
    ast.Deserialize("./tmp/tree.txt");
    // ast.Dump("./tmp/dump_backend");

    CodeGen cg;

    Elf64_Ehdr ehdr;
    CreateElfHeader(&ehdr);

    CodegenProgram(&cg, ast.GetRoot(), &ehdr);

    Elf64_Phdr phdr;
    CreateProgramHeader(&phdr, cg.code.GetSize());

    std::ofstream file(kNameOfOutputFile, std::ios::binary);
    if (!file) {
        std::cerr << "The \"" << kNameOfOutputFile << "\" file cannot be opened. \n" <<
            "It is possible that this file is already being used by another process. "
            "Try to kill this process or delete the file." << std::endl;
        exit(EXIT_FAILURE);
    }

    file.write(reinterpret_cast<const char*>(&ehdr), kElfHeaderSize);
    file.write(reinterpret_cast<const char*>(&phdr), kProgramHeaderSize);
    file.write(reinterpret_cast<const char*>(cg.code.GetData()), cg.code.GetSize() * sizeof(uint8_t));

    if (!file.good()) {
        std::cerr << "Error writing to the \"" << kNameOfOutputFile << "\" file." << std::endl;
        exit(EXIT_FAILURE);
    }

    file.close();

    std::cout << "Executable file created\n";

    return 0;
}
