CC=gcc

CFLAGS  = -c -Wall
LDFLAGS = -shared
SOURCES = cablernet_address.c cablernet_socket.c
OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = cablernet.so

release: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
  
help:
	@echo
	@echo "* $(MAKE) clean"
	@echo
	@echo "* $(MAKE) help"
	@echo "* $(MAKE) release"
	@echo

clean:
	-rm -rf *.o
	-rm -rf $(EXECUTABLE)

.c.o:
	$(CC) $(CFLAGS) $< -o $@