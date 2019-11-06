
#include <basic.h>
#include <ng/string.h>
#include <ng/syscall.h>
#include <ng/uname.h>
#include <nc/errno.h>

sysret sys_uname(struct utsname *n) {
        if (!n)
                return -EINVAL;
        memset(n, 0, sizeof(struct utsname));
        strcpy((char *)&n->sysname, "nightingale");
        strcpy((char *)&n->nodename, "");
        strcpy((char *)&n->release, NIGHTINGALE_VERSION);
        strcpy((char *)&n->version, "");
#if X86_64
        strcpy((char *)&n->machine, "x86_64");
#elif I686
        strcpy((char *)&n->machine, "i686");
#endif
        return 0;
}
