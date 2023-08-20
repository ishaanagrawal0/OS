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

  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  //    type that contains the address of the entrypoint method in fib.c

  // reading the elf header

  

  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content

  

  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step

  

  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.

  

  // 6. Call the "_start" method and print the value returned from the "_start"

  
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