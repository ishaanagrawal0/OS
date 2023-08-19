#include "loader.h"
#include <sys/stat.h>

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
void *binary_content;
void *virtual_mem;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  // Clean up any resources used during loading and running the ELF executable
  close(fd); // Close the file descriptor
  free(binary_content); // Free the allocated memory for binary content
  munmap(virtual_mem, phdr->p_memsz); // Unmap the memory used for the loaded segment
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** exe) {
  const char* filename = exe[1]; // Convert the argument to const char*
  fd = open(filename, O_RDONLY);

  // 1. Load entire binary content into the memory from the ELF file.

  if (fd == -1) {
    perror("Error opening file");
    return;
  }

  // Get the file size
  struct stat file_stat;
  if (fstat(fd, &file_stat) == -1) {
    perror("Error getting file size");
    return;
  }

  // Use size_t to store the file size for memory allocation
  size_t binary_size = (size_t)file_stat.st_size;

  // Allocate memory for binary content
  binary_content = malloc(file_stat.st_size);

  if (binary_content == NULL) {
    perror("Error allocating memory for binary content");
    return;
  }

  // Read binary content into allocated memory
  ssize_t bytes_read = read(fd, binary_content, binary_size);
  if (bytes_read != file_stat.st_size) {
    perror("Error reading binary content");
    return;
  }

  ehdr = (Elf32_Ehdr *)binary_content; // typecast the binary content to Elf32_Ehdr

  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  //    type that contains the address of the entrypoint method in fib.c

  // Get the program header table offset
  off_t program_header_offset = ehdr->e_phoff;

  // Iterate through program headers
  for (int i = 0; i < ehdr->e_phnum; i++) {
    // phdr = (Elf32_Phdr *)(binary_content + program_header_offset + i * sizeof(Elf32_Phdr));
    phdr = (Elf32_Phdr *)(binary_content + program_header_offset + i * ehdr->e_phentsize);

    if (phdr->p_type == PT_LOAD) {
      // Check if the entry point address is within this segment
      
      if (ehdr->e_entry >= phdr->p_vaddr && ehdr->e_entry < (phdr->p_vaddr + phdr->p_memsz)) {
        // Now you've found the program header containing the entry point
        // Proceed with further steps (memory allocation, copying content, etc.)
        break; // You can break after finding the first PT_LOAD segment
      }
    }
  }

  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content

  // Check if the loaded segment has valid memory size
  if (phdr->p_memsz == 0) {
    perror("Invalid memory size");
    return;
  }

  // Allocate memory using mmap

  virtual_mem = mmap(NULL, phdr->p_memsz, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);

  if (virtual_mem == MAP_FAILED) {
    perror("Error mapping segment memory");
    return;
  }

  // Copy segment content into allocated memory
  memcpy(virtual_mem, binary_content + phdr->p_offset, phdr->p_filesz);
  
  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
  
  // Calculate the offset within the loaded segment
  off_t entry_offset = ehdr->e_entry - phdr->p_vaddr;

  // Calculate the entry point address within the allocated memory
  void *entry_address = virtual_mem + entry_offset;

  // 5. Typecast the address to that of the function pointer matching "_start" method in fib.c.
  typedef int (*StartFunc)();
  StartFunc _start = (StartFunc)entry_address;

  // 6. Call the "_start" method and print the value returned from "_start"
  
  // int result = start_func(); // chatgpt claims this one is correct
  // printf("User _start return value = %d\n", result);

  int result = _start();
  printf("User _start return value = %d\n",result);
}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n", argv[0]);
    exit(1);
  }

  // 1. carry out necessary checks on the input ELF file
  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv);
  // 3. invoke the cleanup routine inside the loader
  loader_cleanup();
  return 0;
}
