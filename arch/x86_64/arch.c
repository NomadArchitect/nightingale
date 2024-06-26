#include <assert.h>
#include <ng/arch.h>
#include <ng/limine.h>
#include <ng/thread.h>
#include <ng/x86/acpi.h>
#include <ng/x86/apic.h>
#include <ng/x86/cpu.h>
#include <ng/x86/gdt.h>
#include <ng/x86/pic.h>

void arch_init() {
	gdt_cpu_setup(0);
	gdt_cpu_load();

	uint64_t tmp;
	__asm__ volatile("mov %%cr3, %0" : "=a"(tmp));
	running_process->vm_root = tmp & 0x00FFFFFFFFFFF000;

	acpi_rsdp_t *rsdp = limine_rsdp();
	acpi_init(rsdp);
	void *madt = acpi_get_table("APIC");
	assert(madt);

	pic_init();

	ioapic_init(madt);
	lapic_init();

	if (supports_feature(_X86_FSGSBASE)) {
		enable_bits_cr4(1 << 16); // enable fsgsbase
	}
}

void arch_ap_setup(int cpu) { gdt_cpu_setup(cpu); }

void arch_ap_init() {
	gdt_cpu_load();
	lapic_init();
	if (supports_feature(_X86_FSGSBASE)) {
		enable_bits_cr4(1 << 16); // enable fsgsbase
	}
}

void arch_thread_context_save(struct thread *th) {
	asm volatile("fxsaveq %0" : : "m"(th->fpctx));
}

void arch_thread_context_restore(struct thread *th) {
	asm volatile("fxrstorq %0" : : "m"(th->fpctx));
}
