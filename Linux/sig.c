#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

// CRC32 polynomial (reversed representation)
#define CRC32_POLY 0xEDB88320


uint32_t crc32(uint32_t crc, const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ (CRC32_POLY & -(crc & 1));
        }
    }
    return crc;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <elf_file>\n", argv[0]);
        return 1;
    }

    FILE *elf_file = fopen(argv[1], "rb");
    if (!elf_file) {
        perror("Error opening file");
        return 1;
    }

  
    Elf32_Ehdr elf_header;
    fread(&elf_header, sizeof(Elf32_Ehdr), 1, elf_file);

 
    if (memcmp(elf_header.e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Not a valid ELF file\n");
        fclose(elf_file);
        return 1;
    }

    fseek(elf_file, elf_header.e_shoff, SEEK_SET);
    Elf32_Shdr section_headers[elf_header.e_shnum];
    fread(section_headers, sizeof(Elf32_Shdr), elf_header.e_shnum, elf_file);

 
    for (int i = 0; i < elf_header.e_shnum; ++i) {
    
        fseek(elf_file, section_headers[i].sh_offset, SEEK_SET);
        uint8_t section_data[section_headers[i].sh_size];
        fread(section_data, 1, section_headers[i].sh_size, elf_file);

        uint32_t section_crc = crc32(0xFFFFFFFF, section_data, section_headers[i].sh_size);
        
        printf("Section: %s, CRC32: 0x%08X\n", elf_section_name(elf_header, section_headers[i]), section_crc);
    }

    fclose(elf_file);
    return 0;
}
