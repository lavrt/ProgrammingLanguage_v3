#include <stdio.h>
#include <string.h>
#include <elf.h>

void CreateElfHeader(Elf64_Ehdr* ehdr);
void CreateProgramHeader(Elf64_Phdr* phdr, uint64_t filesz);

int main() {
    unsigned char code[] = {
        0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, // mov rax, 1
        0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00, // mov rdi, 1
        0x48, 0xc7, 0xc6, 0xa6, 0x00, 0x40, 0x00, // mov rsi, 0x400092
        0x48, 0xc7, 0xc2, 0x0e, 0x00, 0x00, 0x00, // mov rdx, 14
        0x0f, 0x05,                               // syscall
        0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00, // mov rax, 60
        0x48, 0xc7, 0xc7, 0x00, 0x00, 0x00, 0x00, // mov rdi, 0
        0x0f, 0x05,                               // syscall
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, // "Hello, World!\n"
        0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0a
    };
    size_t codeSize = sizeof(code);

    Elf64_Ehdr ehdr;
    CreateElfHeader(&ehdr);

    Elf64_Phdr phdr;
    CreateProgramHeader(&phdr, codeSize);

    FILE* file = fopen("output.elf", "wb");
    if (!file) {
        fprintf(stderr, "Cannot open output file\n");
        return 1;
    }

    fwrite(&ehdr, sizeof(Elf64_Ehdr), 1, file);
    fwrite(&phdr, sizeof(Elf64_Phdr), 1, file);
    fwrite(code, 1, codeSize, file);

    fclose(file);
    printf("ELF file created\n");

    return 0;
}

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