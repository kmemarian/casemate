/**
 * A very simple test of the tracer and driver,
 * generates a sequence of all the transitions over dummy variables and traces them.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "common.h"

/* locations we can pretend are pagetables
 */
__attribute__((aligned(4096)))
u64 root[512];
u64 child[512];
u64 new_child[512];
u64 l;

int main(int argc, char **argv)
{
	common_init(argc, argv);
	MARK_VAR(root);
	MARK_VAR(child);
	MARK_VAR(new_child);
	MARK_VAR(l);

	/* tell the modle pud and pgd tables exist,
	 * and logically associate them with the lock. */
	TRANS_MEM_INIT((u64)root, 4096);
	TRANS_MEM_INIT((u64)child, 4096);
	TRANS_MEM_INIT((u64)new_child, 4096);
	HINT(GHOST_HINT_SET_ROOT_LOCK, (u64)root, (u64)&l);
	HINT(GHOST_HINT_SET_OWNER_ROOT, (u64)child, (u64)root);
	HINT(GHOST_HINT_SET_OWNER_ROOT, (u64)new_child, (u64)root);

	/* make root[0] point to child */
	WRITE_ONCE(root[0], (u64)child | 0b11);

	/* track pud as the root */
	MSR(SYSREG_VTTBR, (u64)root);

	LOCK(l);
	WRITE_ONCE(root[0], 0);
	DSB(ish);
	TLBI_ADDR(ipas2e1is,0,3);
	DSB(ish);
	TLBI_ALL(vmalle1is);
	DSB(ish);
	WRITE_ONCE(root[0], (u64)new_child | 0b11);
	UNLOCK(l);
}