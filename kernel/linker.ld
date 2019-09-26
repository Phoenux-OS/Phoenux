OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS
{
    . = 0x100000;

    .text : {
        KEEP(*(.entry))
        *(.text*)
    }

    .data : {
        *(.data*)
        *(.rodata*)
    }

    .bss : {
        bss_start = .;
        *(COMMON)
        *(.bss*)
        bss_end = .;
    }
}
