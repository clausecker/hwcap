#include <string.h>
#include <x86/specialreg.h>

#include "hwcap.h"

/* from Linux: arch/x86/include/asm/cpufeatures.h */
static const struct hwcap {
	struct cap cap;
	unsigned int reg;
	unsigned int bits;
} caps[] = {
	"fpu", "", "x87 floating point unit", 0, CPUID_FPU,
	"vme", "", "virtual 8086 mode enhancements", 0, CPUID_VME,
	"de", "", "debugging extensions", 0, CPUID_DE,
	"pse", "", "page size extensions", 0, CPUID_PSE,
	"tsc", "", "time stamp counter", 0, CPUID_TSC,
	"msr", "", "model-specific registers", 0, CPUID_MSR,
	"pae", "", "physical address extension", 0, CPUID_PAE,
	"mce", "", "machine check exception", 0, CPUID_MCE,
	"cx8", "", "compare-and-exchange 8 bytes", 0, CPUID_CX8,
	"apic", "", "on-chip APIC", 0, CPUID_APIC,
/*	"b10", "", "???", 0, CPUID_B10, */
	"sep", "", "sysenter and sysexit", 0, CPUID_SEP,
	"mtrr", "", "memory type range registers", 0, CPUID_MTRR,
	"pge", "", "page global bit", 0, CPUID_PGE,
	"mca", "", "machine check architecture", 0, CPUID_MCA,
	"cmov", "", "conditional move instructions", 0, CPUID_CMOV,
	"pat", "", "page attribute table", 0, CPUID_PAT,
	"pse36", "", "36-bit page size extension", 0, CPUID_PSE36,
	"pn", "", "processor serial number", 0, CPUID_PSN,
	"clflush", "", "cache-line flush instruction", 0, CPUID_CLFSH,
/*	"b20", "", "???", 0, CPUID_B20, */
	"dts", "", "debug store", 0, CPUID_DS,
	"acpi", "", "thermal monitor and software-controlled clock", 0, CPUID_ACPI,
	"mmx", "", "multimedia extensions", 0, CPUID_MMX,
	"fxsr", "", "fxsave and fxrstor", 0, CPUID_FXSR,
	"sse", "", "scalable SIMD extensions", 0, CPUID_SSE,
	"sse2", "", "scalable SIMD extensions 2", 0, CPUID_SSE2,
	"ss", "", "self snoop", 0, CPUID_SS,
	"ht", "", "hyper threading", 0, CPUID_HTT,
	"tm", "", "thermal monitor automatic thermal control circuitry", 0, CPUID_TM,
	"ia64", "", "IA-64 (Itanium) processor", 0, CPUID_IA64,
	"pbe", "", "pending break enable", 0, CPUID_PBE,

	"pni", "", "streaming SIMD extensions 3", 1, CPUID2_SSE3,
	"pclmulqdq", "", "carryless multiply quadword", 1, CPUID2_PCLMULQDQ,
	"dtes64", "", "64-bit debug store area", 1, CPUID2_DTES64,
	"monitor", "", "monitor and mwait", 1, CPUID2_MON,
	"ds_cpl", "", "CPL-qualified debug store", 1, CPUID2_DS_CPL,
	"vmx", "", "virtual macine extensions", 1, CPUID2_VMX,
	"smx", "", "safer mode extensions", 1, CPUID2_SMX,
	"est", "", "enhanced SpeedStep", 1, CPUID2_EST,
	"tm2", "", "thermal monitor 2", 1, CPUID2_TM2,
	"ssse3", "", "supplemental streaming SIMD extensions 3", 1, CPUID2_SSSE3,
	"cid", "", "L1 data cache context ID", 1, CPUID2_CNXTID,
	"sdbg", "", "silicon debug", 1, CPUID2_SDBG,
	"fma", "", "fused multiply-add", 1, CPUID2_FMA,
	"cx16", "", "compare-and-exchange 16 bytes", 1, CPUID2_CX16,
	"xtpr", "", "xTPR update control", 1, CPUID2_XTPR,
	"pdcm", "", "perfmon and debug capability", 1, CPUID2_PDCM,
	"pcid", "", "process-context identifiers", 1, CPUID2_PCID,
	"dca", "", "direct cache access", 1, CPUID2_DCA,
	"sse4_1", "", "scalable SIMD extensions 4.1", 1, CPUID2_SSE41,
	"sse4_2", "", "scalable SIMD extensions 4.2", 1, CPUID2_SSE42,
	"x2apic", "", "x2APIC", 1, CPUID2_X2APIC,
	"movbe", "", "move big-endian", 1, CPUID2_MOVBE,
	"popcnt", "", "population count", 1, CPUID2_POPCNT,
	"tsc_deadline_timer", "", "APIC supports TSC deadline oneshot operation", 1, CPUID2_TSCDLT,
	"aes", "", "advanced encryption standard new instructions", 1, CPUID2_AESNI,
	"xsave", "", "xsave/xrstor/xsetbv/xgetbv instructions", 1, CPUID2_XSAVE,
	"osxsave", "", "xsave enabled by OS", 1, CPUID2_OSXSAVE,
	"avx", "", "advanced vector extensions", 1, CPUID2_AVX,
	"f16c", "", "16-bit floating point conversion", 1, CPUID2_F16C,
	"rdrand", "", "read random instruction", 1, CPUID2_RDRAND,
	"hv", "", "running on a hypervisor", 1, CPUID2_HV,

	NULL, NULL, NULL, 0, 0,
};

/*
 *  0 -- leaf 0, edx
 *  1 -- leaf 0, ecx
 */
static unsigned int cpuid_bits[2];

static inline void
cpuid(unsigned leaf, unsigned *eax, unsigned *ebx, unsigned *ecx, unsigned *edx)
{
	unsigned a, b, c, d;

	asm ("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(leaf));

	if (eax != NULL)
		*eax = a;

	if (ebx != NULL)
		*ebx = b;

	if (ecx != NULL)
		*ecx = c;

	if (edx != NULL)
		*edx = d;
}

static void
populate_cpuid_bits(void) {
	unsigned max_leaf;

	/* TODO: on i386, check if cpuid supported before trying it */
	memset(cpuid_bits, 0, sizeof(cpuid_bits));

	cpuid(0, &max_leaf, NULL, NULL, NULL);

	if (max_leaf < 1)
		return;

	cpuid(1, NULL, NULL, cpuid_bits + 1, cpuid_bits + 0);
}

void
caps_from_auxv(void)
{
	size_t i;

	populate_cpuid_bits();

	for (i = 0; caps[i].cap.name != NULL; i++)
		if ((cpuid_bits[caps[i].reg] & caps[i].bits) == caps[i].bits)
			register_cap(&caps[i].cap);
}

void
caps_all(void)
{
	size_t i;

	for (i = 0; caps[i].cap.name != NULL; i++)
		register_cap(&caps[i].cap);
}

const struct cap *
get_archlevel(void) {
	return (NULL);
}

void
print_cflags(void) {
}
