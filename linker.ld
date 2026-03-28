ENTRY(_start)

SECTIONS
{
    . = 1M;

    .multiboot : {
        *(.multiboot)
    }

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }

    . = ALIGN(4);
    stack_bottom = .;
    . += 0x4000;   /* 16 KiB стек */
    stack_top = .;
}