#include <ng/fs.h>
#include <ng/mod.h>
#include <stdio.h>

void module_procfile(struct file *ofd, void *) {
	proc_sprintf(ofd, "Hello World from a kernel module\n");
}

int modinit(struct mod *) {
	make_proc_file("mod", module_procfile, nullptr);
	return MODINIT_SUCCESS;
}

struct modinfo modinfo = {
	.name = "procmod",
	.init = modinit,
};
