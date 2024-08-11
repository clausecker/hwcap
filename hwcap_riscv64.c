#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <sys/auxv.h>
#include <sysexits.h>

#include "hwcap.h"

static const struct hwcap {
	struct cap cap;
	unsigned long hwcap;
} caps[] = {
	/* AT_HWCAP */
	"i", "", "integer base ISA",                    HWCAP_ISA_I,
	"e", "", "reduced integer base ISA",            HWCAP_ISA_BIT('e'),
	"m", "", "integer multiplication and division", HWCAP_ISA_M,
	"a", "", "atomics",                             HWCAP_ISA_A,
        "f", "", "single-precision floating-point",     HWCAP_ISA_F,
	"d", "", "double-precision floating-point",     HWCAP_ISA_D,
	"g", "", "general-purpose base ISA",            HWCAP_ISA_G,
	"q", "", "quad-precision floating-point",       HWCAP_ISA_BIT('q'),
	"c", "", "16-bit compressed instructions",      HWCAP_ISA_C,
	"b", "", "bit manipulation",                    HWCAP_ISA_BIT('b'),
	"k", "", "cryptography extensions",             HWCAP_ISA_BIT('k'),
	"j", "", "dynamic languages",                   HWCAP_ISA_BIT('j'),
	"p", "", "packed-SIMD extensions",              HWCAP_ISA_BIT('p'),
	"v", "", "vector extensions",                   HWCAP_ISA_BIT('v'),
	NULL, NULL, NULL, 0,
};

void
caps_from_auxv(void)
{
	size_t i;
	unsigned long hwcap = 0;
	int status;

	status = elf_aux_info(AT_HWCAP, &hwcap, sizeof(hwcap));
	if (status != 0 && errno != ENOENT)
		err(EX_SOFTWARE, "elf_aux_info(AT_HWCAP)");

	for (i = 0; caps[i].cap.name != NULL; i++)
		if ((hwcap & caps[i].hwcap) == caps[i].hwcap)
			register_cap(&caps[i].cap);
}

void
caps_all(void)
{
	size_t i;

	for (i = 0; caps[i].cap.name != NULL; i++)
		register_cap(&caps[i].cap);
}

void
print_cflags(void)
{
	const struct cap *lvl;

	lvl = get_archlevel();
	if (lvl != NULL)
		printf("-march=%s\n", lvl->name);
}

static struct hwcap archlevel = {
	NULL, "", "ISA string", 0
};

const struct cap *
get_archlevel(void)
{
	FILE *isastr;
	size_t len = 0, i;
	char *buf = NULL;
	long ignore = HWCAP_ISA_I|HWCAP_ISA_BIT('e');

	if (archlevel.cap.name != NULL)
		return (&archlevel.cap);

	isastr = open_memstream(&buf, &len);
	if (isastr == NULL)
		err(EX_UNAVAILABLE, "open_memstream");

#if __riscv_xlen == 32
	fputs("riscv32", isastr);
#elif __riscv_xlen == 64
	fputs("riscv64", isastr);
#elif __riscv_xlen == 128
	fputs("riscv128", isastr);
#else
#error	value of __riscv_xlen out of range
#endif

	if (have_cap("g")) {
		fputc('g', isastr);
		ignore |= HWCAP_ISA_G;
	} else if (have_cap("i"))
		fputc('i', isastr);
	else if (have_cap("e"))
		fputc('e', isastr);
	else {
		warnx("cannot determine ISA string: neither flags e nor i present");

		return (NULL);
	}

	for (i = 0; i < ncaps; i++) {
		const struct hwcap *cap;

		cap = (const struct hwcap *)supported_caps[i];
		if (cap->hwcap != 0 && (cap->hwcap & ignore) == cap->hwcap)
			continue;

		fputs(cap->cap.name, isastr);
	}

	if (ferror(isastr) || fclose(isastr) != 0)
		err(EX_UNAVAILABLE, NULL);

	archlevel.cap.name = buf;

	return (&archlevel.cap);
}
