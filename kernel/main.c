
#include <string.h>

#include <basic.h>
#include <multiboot2.h>

#include "debug.h"
#include "panic.h"
#include "term/terminal.h"
#include "term/print.h"
#include "cpu/pic.h"
#include "cpu/pit.h"
#include "cpu/portio.h"
#include "memory/allocator.h"
#include "memory/paging.h"
#include "pci.h"

#ifdef SINGLE_COMPILATION_UNIT
#include "pci.c"
#include "cpu/interrupt.c"
#include "cpu/pic.c"
#include "cpu/pit.c"
#include "cpu/uart.c"
#include "memory/allocator.c"
#include "memory/paging.c"
#include "term/print.c"
// #include "term/term_serial.c"
#include "term/term_vga.c"
#endif

void kernel_main(usize mb_info, u64 mb_magic) {
    { // initialization
        vga_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
        vga_clear();
        uart_init(COM1);
        printf("Terminal Initialized\n");
        printf("UART Initialized\n");

        remap_pic();
        for (int i=0; i<16; i++) {
            mask_irq(i); // This clearly does not work!
        }
        printf("PIC remapped\n");

        setup_interval_timer(1000);
        printf("Interval Timer Initialized\n");

        uart_enable_interrupt(COM1);
        printf("Serial Interrupts Initialized\n");

        enable_irqs();
        printf("IRQs Enabled\n");

        heap_init();
    }

    { // Multiboot
        printf("Multiboot magic: %p\n", mb_magic);
        printf("Multiboot info*: %p\n", mb_info);

        if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
            panic("Hair on fire - this bootloader isn't multiboot2\n");
        }

        multiboot_tag *tag;
        usize size;

        size = *(u32 *)mb_info;
        printf("Multiboot announced size %i\n\n", size);

        for (tag = (multiboot_tag *)(mb_info+8);
             tag->type != MULTIBOOT_TAG_TYPE_END;
             tag = (multiboot_tag *)((u8 *)tag + ((tag->size+7) & ~7))) {

            printf("tag type: %i, size: %i\n", tag->type, tag->size);
            switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE: {
                multiboot_tag_string *cmd_line_tag = (void *)tag;
                printf("Command line = \"%s\"\n", &cmd_line_tag->string);
                // parse_command_line(&cmd_line_tag->string);
                break;
            }
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: {
                printf ("boot loader name = \"%s\"\n",
                       ((struct multiboot_tag_string *) tag)->string);
                break;
            }
            case MULTIBOOT_TAG_TYPE_MMAP: {
                multiboot_mmap_entry *mmap;

                printf("Memory map:\n");

                for (mmap = ((multiboot_tag_mmap *)tag)->entries;
                     (u8 *)mmap < (u8 *)tag + tag->size;
                     mmap = (multiboot_mmap_entry *)((unsigned long) mmap
                         + ((struct multiboot_tag_mmap *) tag)->entry_size)) {

                    printf("base: %p, len: %x (%iM), type %i\n",
                            mmap->addr, mmap->len, mmap->len/(1024*1024), mmap->type);
                }
                break;
            }
            case MULTIBOOT_TAG_TYPE_ELF_SECTIONS: {
                multiboot_tag_elf_sections *elf = (void *)tag;
                printf("size    = %i\n", tag->size);
                printf("num     = %i\n", elf->num);
                printf("entsize = %i\n", elf->entsize);
                //panic();
                break;
            }
            default: {
                printf("unhandled\n");
            }
            }
        }

        //panic("exit early so i can read it properly");

        // do the multiboot magic
    }

    { // allocation 
        u8 *alloc_test0 = malloc(16);

        printf("\nalloc_test0 = %x\n", alloc_test0);

        for (i32 i=0; i<16; i++) {
            alloc_test0[i] = i;
        }

        // debug_print_mem(16, alloc_test0-4);
        // debug_dump(alloc_test0);

        free(alloc_test0);
    }

    { // page resolution and mapping
        usize resolved1 = resolve_virtual_to_physical(0x201888);
        printf("resolved vma:%p to pma:%p\n", 0x201888, resolved1);

        map_virtual_to_physical(0x201000, 0x9990000);
        usize resolved2 = resolve_virtual_to_physical(0x201888);
        printf("resolved vma:%p to pma:%p\n", 0x201888, resolved2);

        map_virtual_to_physical(0x201000, 0x10000);
        usize resolved3 = resolve_virtual_to_physical(0x201888);
        printf("resolved vma:%p to pma:%p\n", 0x201888, resolved3);

        printf("\n");

        int *pointer_to_be = (int *)0x202000;
        map_virtual_to_physical((usize)pointer_to_be, 0x300000);
        *pointer_to_be = 19;
        printf("pointer_to_be has %i at vma:%p, pma:%p\n", *pointer_to_be, pointer_to_be, resolve_virtual_to_physical(pointer_to_be));
        // debug_dump(pointer_to_be);'

        map_virtual_to_physical(0x55555000, 0x0);
    }

    { // u128 test
        printf("\n\n");
        u128 x = 0;
        x -= 1;
        // debug_dump(&x); // i can't print this, but i can prove it works this way
    }

    { // memset speed visualization
    }
    
    { // testing length of kernel
        extern usize _kernel_start;
        extern usize _kernel_end;

        usize len = (usize)&_kernel_end - (usize)&_kernel_start;

        // Why tf does _kernel_start = .; not work in link.ld?
        if ((usize)&_kernel_start == 0x100000) {
            printf("_kernel_start = %p;\n", &_kernel_start);
        } else {
            printf("_kernel_start = %p; // wtf?\n", &_kernel_start);
        }
        printf("_kernel_end   = %p;\n", &_kernel_end);
        printf("\n");
        printf("kernel is %i kilobytes long\n", len / 1024);
        printf("kernel is %x bytes long\n", len);
    }

    { // PCI testing
        printf("\n");
        printf("Discovered PCI devices:\n");

        pci_enumerate_bus_and_print();
        // should go for some sort of depth-first approach perhaps, check for a bus adapter
        
        printf("\n\n");
    }
    
    { // Network card driver testing

        u32 p = pci_find_device_by_id(0x8086, 0x100e);
        printf("Network card ID = ");
        pci_print_addr(p);

        printf("\n\n");

        

        panic("Stop early");

    }

    { // exit / fail test

        printf("\ntimer_ticks completed = %i\n", timer_ticks);
        if (timer_ticks > 9 && timer_ticks < 99) {
            printf("Theoretically this means we took 0.0%is to execute\n", timer_ticks);
        }

        /* // syscall
        asm volatile ("mov $1, %%rax" ::: "rax");
        asm volatile ("int $0x80");
        */

        // page fault
        volatile int *x = (int *)0x1000000;
        *x = 1;
        // yes I can
        

        // while (true);
        panic("kernel_main tried to return!\n");
    }
}

