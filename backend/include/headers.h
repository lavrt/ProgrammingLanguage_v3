#ifndef HEADERS_H
#define HEADERS_H

#include <elf.h>

constexpr uint64_t kElfHeaderSize = sizeof(Elf64_Ehdr);
constexpr uint64_t kProgramHeaderSize = sizeof(Elf64_Phdr);

void CreateElfHeader(Elf64_Ehdr* ehdr);
void CreateProgramHeader(Elf64_Phdr* phdr, uint64_t filesz);

#endif // HEADERS_H