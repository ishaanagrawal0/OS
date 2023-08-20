#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  free(ehdr); // freeing the elf header
  free(phdr); // freeing the program header
  close(fd); // closing the file descriptor
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** exe) {
  fd = open(exe[1], O_RDONLY); // fd is our file descriptor for the elf file that is exe[1]

  // 1. Load entire binary content into the memory from the ELF file.

  if (fd == -1) {
    perror("Error opening file");
    return;
  }

  // getting the size of the file
  off_t file_size = lseek(fd, 0, SEEK_END);
  if (file_size == -1) {
    perror("Error getting file size");
    return;
  }

  // Use size_t to store the file size for memory allocation
  size_t binary_size = file_size;

  // Allocate memory for binary content
  void* binary_content = malloc(binary_size);

  if (binary_content == NULL) {
    perror("Error allocating memory for binary content");
    return;
  }

  // Set the file pointer back to the beginning of the file
  if (lseek(fd, 0, SEEK_SET) == -1) {
    perror("Error resetting file pointer");
    return;
  }

  // Read binary content into allocated memory
  ssize_t bytes_read = read(fd, binary_content, binary_size);
  
  if (bytes_read != binary_size) {
    fprintf(stderr, "Incomplete read: expected %zu bytes, but only read %zd bytes\n", binary_size, bytes_read);
    return;
  }

  ehdr = (Elf32_Ehdr *) binary_content; // setting the elf header to the binary content

  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  //    type that contains the address of the entrypoint method in fib.c

  phdr = (Elf32_Phdr *) (binary_content + ehdr->e_phoff); // setting the program header to the binary content + the offset of the elf header

  Elf32_Phdr *load_phdr = NULL;
  
  for (int i = 0; i < ehdr->e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      load_phdr = &phdr[i];
      break;
    }
  }

  if (load_phdr == NULL) {
    fprintf(stderr, "No PT_LOAD section found\n");
    return;
  }

  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content

  void *virtual_mem = mmap(NULL, load_phdr->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

  if (virtual_mem == MAP_FAILED) {
    perror("Error allocating virtual memory");
    return;
  }

  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step

  size_t entrypoint_offset = ehdr->e_entry - load_phdr->p_vaddr;
  void *entrypoint_address = virtual_mem + entrypoint_offset;

  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.

  int (*_start)(void) = (int (*)(void)) ehdr->e_entry;

  // 6. Call the "_start" method and print the value returned from the "_start"
  
  int result = _start();
  printf("Result: %d\n", result);
}

int main(int argc, char** argv) 
{
  // argc will be 2, since we provide two arguments: the program name (loader) and the argument (fib)
  // argv[0] will be the program name ("loader")
  // argv[1] will be the argument that has been provided ("fib")

  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv); // changed the function call to pass the argument vector
  // 3. invoke the cleanup routine inside the loader  
  loader_cleanup();
  return 0;
}
