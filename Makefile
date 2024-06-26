.PHONY: lib src clean
src: lib
	$(MAKE) -C src

lib: 
	$(MAKE) -C lib

clean:
	$(MAKE) -C lib clean
	$(MAKE) -C src clean
