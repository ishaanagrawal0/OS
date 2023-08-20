#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

void loader_cleanup() {
  if (fd != -1) {
    close(fd);
  }
}

void load_and_run_elf(char** exe) {
  fd = open(exe[1], O_RDONLY);
  if (fd == -1) {
    perror("Error opening ELF file");
    return;
  }

  ehdr = mmap(NULL, sizeof(Elf32_Ehdr), PROT_READ, MAP_PRIVATE, fd, 0);
  if (ehdr == MAP_FAILED) {
    perror("Error mapping ELF header");
    return;
  }

  phdr = (Elf32_Phdr *)((char *)ehdr + ehdr->e_phoff);

  for (int i = 0; i < ehdr->e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      void *segment_address = mmap((void *)phdr[i].p_vaddr, phdr[i].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_FIXED, fd, phdr[i].p_offset);
      if (segment_address == MAP_FAILED) {
        perror("Error mapping segment");
        return;
      }

      if (ehdr->e_entry >= phdr[i].p_vaddr && ehdr->e_entry < phdr[i].p_vaddr + phdr[i].p_memsz) {
        int (*_start)() = (int (*)())(segment_address + (ehdr->e_entry - phdr[i].p_vaddr));
        int result = _start();
        printf("%d", result);
      }
    }
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s <ELF Executable>\n", argv[0]);
    exit(1);
  }

  load_and_run_elf(argv);
  loader_cleanup();
  return 0;
}
