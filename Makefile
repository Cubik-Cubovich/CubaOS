# CubaOS Makefile

PROJECT = CubaOS

SRCDIR = src
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
BINDIR = $(BUILDDIR)/bin
ISODIR = $(BUILDDIR)/iso

AS = nasm
ASFLAGS = -f elf32

CC = gcc
CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -Wall -Wextra -O0 -g

LD = ld
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib -static

# Поиск всех исходных файлов
ASM_SOURCES = $(shell find $(SRCDIR) -name '*.asm')
C_SOURCES = $(shell find $(SRCDIR) -name '*.c')
ALL_SOURCES = $(ASM_SOURCES) $(C_SOURCES)

# Преобразование путей к объектным файлам
ASM_OBJECTS = $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.o, $(ASM_SOURCES))
C_OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(C_SOURCES))
OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

KERNEL = $(BINDIR)/kernel.elf
ISO = $(BUILDDIR)/$(PROJECT).iso

all: $(KERNEL)

$(KERNEL): $(OBJECTS) linker.ld
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

# Сборка asm
$(OBJDIR)/%.o: $(SRCDIR)/%.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Сборка C
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

iso: $(KERNEL)
	@mkdir -p $(ISODIR)/boot/grub
	@cp $(KERNEL) $(ISODIR)/boot/kernel.elf
	@echo 'set timeout=0' > $(ISODIR)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(ISODIR)/boot/grub/grub.cfg
	@echo 'menuentry "CubaOS" {' >> $(ISODIR)/boot/grub/grub.cfg
	@echo '    multiboot2 /boot/kernel.elf' >> $(ISODIR)/boot/grub/grub.cfg
	@echo '    boot' >> $(ISODIR)/boot/grub/grub.cfg
	@echo '}' >> $(ISODIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $(ISO) $(ISODIR)
	@echo "ISO created: $(ISO)"

run-iso: iso
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean iso run-iso