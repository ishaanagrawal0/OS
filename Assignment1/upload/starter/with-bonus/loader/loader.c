#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
void* segment_memory = NULL;
size_t segment_size = 0;

void loader_cleanup() {
  int result;

  if (segment_memory != NULL) {
    result = munmap(segment_memory, segment_size);
    if (result == -1) {
      perror("Encountered an error while unmapping segment memory");
    }
  }

  if (fd != -1) {
    result = close(fd);

    if (result == -1) {
      perror("Encountered an error while closing ELF file");
    }
  }

  if (ehdr != MAP_FAILED) {
    result = munmap(ehdr, sizeof(Elf32_Ehdr));
    if (result == -1) {
      perror("Encountered an error while unmapping binary content");
    }
  }
}

// signal handler for SIGSEGV
void signal_handler(int signo) {
  if (signo == SIGSEGV) {
    // Handle segmentation fault by allocating memory using mmap
    // and then loading the appropriate segment.
    void* fault_address = (void*)(((unsigned long)ehdr) & ~0xFFF); // Align to page boundary
    size_t page_size = getpagesize();

    // Calculate the size of the segment containing the fault address
    for (int i = 0; i < ehdr->e_phnum; i++) {
      Elf32_Phdr* p = (Elf32_Phdr*)((char*)ehdr + ehdr->e_phoff + i * ehdr->e_phentsize);
      if (p->p_type == PT_LOAD && fault_address >= (void*)p->p_vaddr && fault_address < (void*)(p->p_vaddr + p->p_memsz)) {
        size_t segment_offset = fault_address - (void*)p->p_vaddr;
        size_t segment_size = p->p_memsz - segment_offset;
        segment_size = (segment_size + page_size - 1) & ~(page_size - 1); // Round up to the nearest page size

        // Allocate memory for the segment
        segment_memory = mmap(fault_address, segment_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
        if (segment_memory == MAP_FAILED) {
          perror("Encountered an error while mapping segment memory");
          exit(1);
        }

        // Copy the segment content from the ELF file
        lseek(fd, p->p_offset + segment_offset, SEEK_SET);
        if (read(fd, segment_memory, segment_size) != segment_size) {
          perror("Encountered an error while copying segment content");
          exit(1);
        }

        // Reset the signal handler to the default behavior
        struct sigaction act;
        act.sa_handler = SIG_DFL;
        sigaction(SIGSEGV, &act, NULL);

        // Continue execution
        return;
      }
    }

    // If we reach this point, there was a SIGSEGV, but it wasn't due to loading a segment.
    // Handle or report the error as needed.
    perror("Segmentation fault not related to loading a segment");
    exit(1);
  }
}


void load_and_run_elf(char** exe) {
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_handler = signal_handler;
  act.sa_flags = SA_SIGINFO;

  if (sigaction(SIGSEGV, &act, NULL) < 0) {
    perror("sigaction");
    exit(1);
  }

  // gracefully exit the program if exe is NULL
  if (exe == NULL) {
    printf("No executable file provided\n");
    return;
  }

  // open the file in read-only mode
  fd = open(exe[1], O_RDONLY);

  // error handling for opening ELF file
  if (fd == -1) {
    perror("Encountered an error while opening the file");
    return;
  }

  // Read the first 16 bytes of the file to check for ELF magic number
  char elf_magic[4];

  if (read(fd, elf_magic, 4) != 4 || elf_magic[0] != 0x7f || elf_magic[1] != 'E' || elf_magic[2] != 'L' || elf_magic[3] != 'F') {
    printf("File is not an ELF file\n");
    close(fd);
    return;
  }

  ehdr = mmap(NULL, sizeof(Elf32_Ehdr), PROT_READ, MAP_PRIVATE, fd, 0); // loading binary content using mmap

  // error handling for loading binary content
  if (ehdr == MAP_FAILED) {
    perror("Encountered an error while mapping binary content");
    close(fd);
    return;
  }

  // Initialize segment_memory and segment_size
  segment_memory = NULL;
  segment_size = 0;

  int (*_start)() = (int (*)())(ehdr->e_entry);
  int result = _start();
  printf("Result: %d\n", result);
}