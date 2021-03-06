CC=gcc
NASM=nasm

CFLAGS=-W -Wall -Wextra -nostdlib -nodefaultlibs -mcmodel=large -m64 \
	-ffreestanding -mno-red-zone -masm=intel -std=c99 \
	-Werror -Wno-error=unused-variable -Wno-error=unused-function \
	-Wno-error=unused-parameter -fno-zero-initialized-in-bss\
	-I`pwd`
LDFLAGS=-nostdlib -nodefaultlibs -m elf_x86_64

KERNEL_OUTPUT=kernel.bin

LDSCRIPT=kernel.ld


MODULES = mem hw io std threads .


KERNEL_SOURCES=$(foreach module, $(MODULES), $(wildcard $(module)/*.c)) $(wildcard *.s) $(wildcard hw/*.s)

OBJECTS=$(KERNEL_SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.s=.o)

.PHONY: all
all: $(KERNEL_OUTPUT)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.s.o:
	$(NASM) -f elf64 $< -o $@

$(KERNEL_OUTPUT): $(OBJECTS) $(LOADER_SOURCES) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -T $(LDSCRIPT) -o kernel.bin $(OBJECTS)

depend: $(KERNEL_SOURCES)
	gcc -MM $(CFLAGS) $(KERNEL_SOURCES) > depend
-include depend

.PHONY: clean
clean:
	-rm -f $(OBJECTS) $(KERNEL_OUTPUT) depend
