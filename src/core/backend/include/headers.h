#ifndef HEADERS_H
#define HEADERS_H

#include <elf.h>

constexpr uint64_t kElfHeaderSize = sizeof(Elf64_Ehdr);
constexpr uint64_t kProgramHeaderSize = sizeof(Elf64_Phdr);

#endif // HEADERS_H
