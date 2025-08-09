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
    std::vector<std::pair<NodeType, std::string>> nodes;
    tNode* root = ReadTree(nodes);
    dump(root);

    TCodeGen cg;
    CodeGenCtor(&cg);

    Elf64_Ehdr ehdr;
    CreateElfHeader(&ehdr);

    CodegenProgram(&cg, root, &ehdr);

    Elf64_Phdr phdr;
    CreateProgramHeader(&phdr, cg.size);

    std::ofstream file(kNameOfOutputFile, std::ios::binary);
    if (!file) {
        std::cerr << "The \"" << kNameOfOutputFile << "\" file cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }

    file.write(reinterpret_cast<const char*>(&ehdr), kElfHeaderSize);
    file.write(reinterpret_cast<const char*>(&phdr), kProgramHeaderSize);
    file.write(reinterpret_cast<const char*>(cg.code), cg.size);

    if (!file.good()) {
        std::cerr << "Error writing to the \"" << kNameOfOutputFile << "\" file." << std::endl;
        exit(EXIT_FAILURE);
    }

    file.close();

    std::cout << "Executable file created\n";

    CodeGenDtor(&cg); 

    treeDtor(root);

    return 0;
}
