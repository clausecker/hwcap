#include <string.h>
#include <x86/specialreg.h>

#include "hwcap.h"

/* from Linux: arch/x86/include/asm/cpufeatures.h */
static const struct hwcap {
	struct cap cap;
	unsigned int reg;
	unsigned int bits;
} caps[] = {
	/* leaf 1, edx */
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

	/* leaf 1, ecx */
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

	/* leaf 7:0, ebx */
	"fsgsbase", "", "read/write fs/gs base address", 2, CPUID_STDEXT_FSGSBASE,
	"tsc_adjust", "", "TSC adjustment MSR", 2, CPUID_STDEXT_TSC_ADJUST,
	"sgx", "", "software guard extensions", 2, CPUID_STDEXT_SGX,
	"bmi1", "", "bit manipulation instructions 1", 2, CPUID_STDEXT_BMI1,
	"hle", "", "hardware lock elision", 2, CPUID_STDEXT_HLE,
	"avx2", "", "advanced vector extensions 2", 2, CPUID_STDEXT_AVX2,
/*	"???", "", "FPU data pointer updated on x87 exceptions only", 2, CPUID_STDEXT_FDP_EXC, */
	"smep", "", "supervisor mode execution prevention", 2, CPUID_STDEXT_SMEP,
	"bmi2", "", "bit manipulation instructions 2", 2, CPUID_STDEXT_BMI2,
	"erms", "", "enhanced rep movsb/stosb instructions", 2, CPUID_STDEXT_ERMS,
	"invpcid", "", "invalidate processor context ID", 2, CPUID_STDEXT_INVPCID,
	"rtm", "", "restricted transactional memory", 2, CPUID_STDEXT_RTM,
	"cqm", "", "resource director technology monitoring", 2, CPUID_STDEXT_PQM,
/*	"???", "", "FPU CS/DS values deprecated", 2, CPUID_STDEXT_NFPUSG, */
	"mpx", "", "memory protections extension", 2, CPUID_STDEXT_MPX,
	"rdt_a", "", "resource director technology allocation", 2, CPUID_STDEXT_PQE,
	"avx512f", "", "AVX-512 foundation", 2, CPUID_STDEXT_AVX512F,
	"avx512dq", "", "AVX-512 double and quadword", 2, CPUID_STDEXT_AVX512DQ,
	"rdseed", "", "random seed instruction", 2, CPUID_STDEXT_RDSEED,
	"adx", "", "add with carry through CF/OF instructions", 2, CPUID_STDEXT_ADX,
	"smap", "", "supervisor mode access prevention", 2, CPUID_STDEXT_SMAP,
	"avx512ifma", "", "AVX-512 integer fused multiply-add", 2, CPUID_STDEXT_AVX512IFMA,
	"clflushopt", "", "flush cacheline optimized", 2, CPUID_STDEXT_CLFLUSHOPT,
	"clwb", "", "cache line writeback", 2, CPUID_STDEXT_CLWB,
	"intel_pt", "", "intel processor trace", 2, CPUID_STDEXT_PROCTRACE,
	"avx512pf", "", "AVX-512 prefetch", 2, CPUID_STDEXT_AVX512PF,
	"avx512er", "", "AVX-512 exponential and reciprocal", 2, CPUID_STDEXT_AVX512ER,
	"avx512cd", "", "AVX-512 conflict detection", 2, CPUID_STDEXT_AVX512CD,
	"sha_ni", "", "secure hashing algorithm extensions", 2, CPUID_STDEXT_SHA,
	"avx512bw", "", "AVX-512 byte and word", 2, CPUID_STDEXT_AVX512BW,
	"avx512vl", "", "AVX-512 vector length extensions", 2, CPUID_STDEXT_AVX512VL,

	/* leaf 7:0, ecx */
/*	"???", "", "prefetch for writing with T1 hint", 3, CPUID_STDEXT2_PREFETCHWT1, */
	"avx512vbmi", "", "AVX-512 vector bit manipulation", 3, CPUID_STDEXT2_AVX512VBMI,
	"umip", "", "user-mode instruction prevention", 3, CPUID_STDEXT2_UMIP,
	"pku", "", "protection keys for user-mode pages", 3, CPUID_STDEXT2_PKU,
	"ospke", "", "protection keys enabled by OS", 3, CPUID_STDEXT2_OSPKE,
	"waitpkg", "", "umonitor/umwait/tpause instructions", 3, CPUID_STDEXT2_WAITPKG,
	"avx512_vbmi2", "", "AVX-512 vector bit manipulation 2", 3, CPUID_STDEXT2_AVX512VBMI2,
/*	"???", "", "shadow stack", 3, 0x00000080, */
	"gfni", "", "galois field new instructions", 3, CPUID_STDEXT2_GFNI,
	"vaes", "", "vector AES", 3, CPUID_STDEXT2_VAES,
	"vpclmulqdq", "", "vector carryless multiply quadword", 3, CPUID_STDEXT2_VPCLMULQDQ,
	"avx512_vnni", "", "AVX-512 vector neural network instructions", 3, CPUID_STDEXT2_AVX512VNNI,
	"avx512_bitalg", "", "AVX-512 bit algorithms", 3, CPUID_STDEXT2_AVX512BITALG,
	"tme", "", "total memory encryption", 3, CPUID_STDEXT2_TME,
	"avx512_vpopcntdq", "", "vector population count", 3, CPUID_STDEXT2_AVX512VPOPCNTDQ,
	"la57", "", "5-level page tables", 3, CPUID_STDEXT2_LA57,
	"rdpid", "", "read processor ID", 3, CPUID_STDEXT2_RDPID,
	"bus_lock_detect", "", "bus lock detect", 3, 0x01000000, /* TODO: add symbolic name */
	"cldemote", "", "cache line demote", 3, CPUID_STDEXT2_CLDEMOTE,
	"movdiri", "", "move doubleword as direct store", 3, CPUID_STDEXT2_MOVDIRI,
	"movdir64b", "", "move 64 bytes as direct store", 3, CPUID_STDEXT2_MOVDIR64B,
	"enqcmd", "", "enqueue command", 3, CPUID_STDEXT2_ENQCMD,
	"sgx_lc", "", "software guard extensions launch control", 3, CPUID_STDEXT2_SGXLC,

	/* leaf 7:0, edx */
	"avx512_4vnniw", "", "AVX-512 vector neuronal network instructions word variable precision", 4, CPUID_STDEXT3_AVX5124VNNIW,
	"avx512_4fmaps", "", "AVX-512 fused multiply accumulate packed single precision", 4, CPUID_STDEXT3_AVX5124FMAPS,
	"fsrm", "", "fast short rep movsb", 4, CPUID_STDEXT3_FSRM,
	"avx512_vp2intersect", "", "AVX-512 vector pair intersection to a pair of mask registers", 4, CPUID_STDEXT3_AVX512VP2INTERSECT,
	"srbds_ctrl", "", "SRBDS mitigation MSR available", 4, CPUID_STDEXT3_MCUOPT,
	"md_clear", "", "verw clears microarchitectural buffers", 4, CPUID_STDEXT3_MD_CLEAR,
	"rtm_always_abort", "", "RTM transactions always abort", 4, 0x00000800,
	"tsx_force_abort", "", "TSX_FORCE_ABORT MSR available", 4, CPUID_STDEXT3_TSXFA,
	"serialize", "", "the serialize instruction", 4, 0x00004000,
	"hybrid_cpu", "", "multiple types of CPU cores installed", 4, 0x00008000,
	"tsxldtrk", "", "TSX suspend load address tracking", 4, 0x00010000,
	"pconfig", "", "platform configuration instruction", 4, CPUID_STDEXT3_PCONFIG,
	"arch_lbr", "", "architectural last branch record", 4, 0x00080000,
	"ibt", "", "indirect branch tracking", 4, 0x00100000,
	"amx_bf16", "", "AMX bfloat16 instructions", 4, 0x00400000,
	"avx512_fp16", "", "AVX-512 binary16 instructions", 4, 0x00800000,
	"amx_tile", "", "AMX tile support", 4, 0x01000000,
	"amx_int8", "", "AMX int8 instructions", 4, 0x02000000,
	"spec_ctrl", "", "indirect branch prediction barrier / indirect branch restricted speculation", 4, CPUID_STDEXT3_IBPB,
	"intel_stibp", "", "single thread indirect branch predictors", 4, CPUID_STDEXT3_STIBP,
	"flush_l1d", "", "IA32_FLUSH_CMD MSR available", 4, CPUID_STDEXT3_L1D_FLUSH,
	"arch_capabilities", "", "IA32_ARCH_CAPABILITIES MSR available", 4, CPUID_STDEXT3_ARCH_CAP,
	"core_capabilities", "", "IA32_CORE_CAPABILITIES MSR available", 4, CPUID_STDEXT3_CORE_CAP,
	"spec_ctrl_ssbd", "", "speculative store bypass disable", 4, CPUID_STDEXT3_SSBD,

	NULL, NULL, NULL, 0, 0,
};

/*
 *  0 -- leaf 0x00000001, edx
 *  1 -- leaf 0x00000001, ecx
 *  2 -- leaf 0x00000007:0, ebx
 *  3 -- leaf 0x00000007:0, ecx
 *  4 -- leaf 0x00000007:0, edx
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

static inline void
cpuidx(unsigned leaf, unsigned sub, unsigned *eax, unsigned *ebx, unsigned *ecx, unsigned *edx)
{
	unsigned a, b, c, d;

	asm ("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(leaf), "2"(sub));

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

	if (max_leaf < 7)
		return;

	cpuidx(7, 0, NULL, cpuid_bits + 2, cpuid_bits + 3, cpuid_bits + 4);
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
