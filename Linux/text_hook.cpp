#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

void print_function_pointers(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Read ELF header
    Elf64_Ehdr elf_header;
    fread(&elf_header, sizeof(Elf64_Ehdr), 1, file);

    // Check if ELF file
    if (elf_header.e_ident[EI_MAG0] != ELFMAG0 || elf_header.e_ident[EI_MAG1] != ELFMAG1 ||
        elf_header.e_ident[EI_MAG2] != ELFMAG2 || elf_header.e_ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "%s is not an ELF file\n", filename);
        fclose(file);
        return;
    }

    // Seek to section header table
    fseek(file, elf_header.e_shoff, SEEK_SET);

    // Read section headers
    Elf64_Shdr section_headers[elf_header.e_shnum];
    fread(section_headers, sizeof(Elf64_Shdr), elf_header.e_shnum, file);

    // Find the .text section
    Elf64_Shdr *text_section_header = NULL;
    char *section_names = NULL;
    for (int i = 0; i < elf_header.e_shnum; i++) {
        if (section_headers[i].sh_type == SHT_PROGBITS &&
            (section_headers[i].sh_flags & SHF_EXECINSTR) &&
            section_headers[i].sh_size > 0) {
            text_section_header = &section_headers[i];
            section_names = (char *)malloc(text_section_header->sh_size);
            fseek(file, text_section_header->sh_offset, SEEK_SET);
            fread(section_names, text_section_header->sh_size, 1, file);
            break;
        }
    }

    // Read and print function pointers
    if (text_section_header) {
        printf("Function Pointers:\n");
        for (int i = 0; i < text_section_header->sh_size; i++) {
            void *ptr = *(void **)&section_names[i];
            if (ptr && ptr >= (void *)elf_header.e_entry &&
                ptr < (void *)(elf_header.e_entry + text_section_header->sh_size)) {
                printf("Name: %s, Address: %p\n", &section_names[i], ptr);
            }
        }
    }

    free(section_names);
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf_filename>\n", argv[0]);
        return 1;
    }

    print_function_pointers(argv[1]);

    return 0;
}
