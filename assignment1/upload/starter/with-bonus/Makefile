#invoke make inside following directories and in this order: loader, launch, fib
#move the lib_simpleloader.so and launch binaries inside bin directory
#Provide the command for cleanup

# Directories to build
DIRS = loader launcher test

all: $(DIRS)

$(DIRS): $(MAKE) -C $@

clean: for dir in $(DIRS); do \
    $(MAKE) -C $$dir clean; \
    	done