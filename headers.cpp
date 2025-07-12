#include "headers.h"

#include <string.h>

void CreateElfHeader(Elf64_Ehdr* ehdr) {
    ehdr->e_ident[EI_MAG0] = ELFMAG0;
    ehdr->e_ident[EI_MAG1] = ELFMAG1;
    ehdr->e_ident[EI_MAG2] = ELFMAG2;
    ehdr->e_ident[EI_MAG3] = ELFMAG3;
    ehdr->e_ident[EI_CLASS] = ELFCLASS64;
    ehdr->e_ident[EI_DATA] = ELFDATA2LSB;
    ehdr->e_ident[EI_VERSION] = EV_CURRENT;
    ehdr->e_ident[EI_OSABI] = ELFOSABI_SYSV;
    ehdr->e_ident[EI_ABIVERSION] = 0;
    memset(&ehdr->e_ident + EI_PAD, 0, EI_NIDENT - EI_PAD);

    ehdr->e_type = ET_EXEC;
    ehdr->e_machine = EM_X86_64;
    ehdr->e_version = EV_CURRENT;
    ehdr->e_entry = 0x400000 + sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);
    ehdr->e_phoff = sizeof(Elf64_Ehdr);
    ehdr->e_shoff = 0;
    ehdr->e_flags = 0;
    ehdr->e_ehsize = sizeof(Elf64_Ehdr);
    ehdr->e_phentsize = sizeof(Elf64_Phdr);
    ehdr->e_phnum = 1;
    ehdr->e_shentsize = 0;
    ehdr->e_shnum = 0;
    ehdr->e_shstrndx = 0;
}

void CreateProgramHeader(Elf64_Phdr* phdr, uint64_t filesz) {
    phdr->p_type = PT_LOAD;                                                 // segment type
    phdr->p_flags = PF_X | PF_R;                                            // segment access flags
    phdr->p_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);               // segment offset in the file
    phdr->p_vaddr = 0x400000 + sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);     // segment virtual address
    phdr->p_paddr = phdr->p_vaddr;                                          // segment physical address
    phdr->p_filesz = filesz;                                                // segment size in the file
    phdr->p_memsz = filesz;                                                 // segment size in memory
    phdr->p_align = 0x1000;                                                 // segment alignment
}