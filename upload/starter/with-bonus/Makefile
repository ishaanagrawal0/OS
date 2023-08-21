#invoke make inside following directories and in this order: loader, launch, fib
#move the lib_simpleloader.so and launch binaries inside bin directory
#Provide the command for cleanup
.PHONY: all clean

all:
	@$(MAKE) -C loader
	@$(MAKE) -C launcher
	@$(MAKE) -C test
	@mkdir -p bin
	@mv launcher/launch bin/
	@mv loader/lib_simpleloader.so bin/
	@mv test/fib bin/

clean:
	@$(MAKE) -C loader clean
	@$(MAKE) -C launcher clean
	@$(MAKE) -C test clean
	@rm -rf bin
