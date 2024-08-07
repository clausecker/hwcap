#include <assert.h>
#include <stdlib.h>
#include <sys/auxv.h>
#include <sysexits.h>
#include <unistd.h>

/* https://docs.kernel.org/arch/arm64/elf_hwcaps.html */
static const struct hwcap {
	const char	*name;
	const char	*cflag;
	const char	*descr;

	unsigned long	hwcap, hwcap2;
} caps[] = {
	/* AT_HWCAP */
	"fp",         "",         "floating point",                                 HWCAP_FP, 0,
	"asimd",      "",         "advanced SIMD",                                  HWCAP_ASIMD, 0,
	"evtstrm",    "",         "generic timer configured to 10 kHz",             HWCAP_EVTSTRM, 0,
	"aes",        "",         "advanced encryption standard",                   HWCAP_AES, 0,
	"pmull",      "aes",      "polynomial multiply long",                       HWCAP_PMULL, 0,
	"sha1",       "",         "secure hashing algorithm 1",                     HWCAP_SHA1, 0,
	"sha2",       "sha2",     "secure hashing algorithm 2 (SHA-224/256)",       HWCAP_SHA2, 0,
	"crc32",      "crc",      "cyclic redundancy check",                        HWCAP_CRC32, 0,
	"atomics",    "lse",      "large system extensions (atomics)",              HWCAP_ATOMICS, 0,
	"fphp",       "fp16",     "half-precision floating-point arithmetic",       HWCAP_FPHP, 0,
	"asimdhp",    "",         "ASIMD half-precision floating-point arithmetic", HWCAP_ASIMDHP, 0,
	"cpuid",      "",         "user-space access to ID registers",              HWCAP_CPUID, 0,
	"asimdrdm",   "rdma",     "ASIMD rounding double multiply-accumulate",      HWCAP_ASIMDRDM, 0,
	"jscvt",      "",         "JavaScript conversion from double to integer",   HWCAP_JSCVT, 0,
	"fcma",       "",         "floating-point complex arithmetic",              HWCAP_FCMA, 0,
	"lrcpc",      "rcpc",     "load-acquire RCpc instructions",                 HWCAP_LRCPC, 0,
	"dcpop",      "",         "data persistence writeback",                     HWCAP_DCPOP, 0,
	"sha3",       "sha3",     "secure hashing algorithm 3",                     HWCAP_SHA3, 0,
	"sm3",        "",         "SM3 cryptographic hash algorithm",               HWCAP_SM3, 0,
	"sm4",        "sm4",      "SM4 cryptographic hash algorithm",               HWCAP_SM4, 0,
	"asimddp",    "dotprod",  "ASIMD dot product",                              HWCAP_ASIMDDP, 0,
	"sha512",     "",         "secure hashing algorithm 2 (SHA-384/512)",       HWCAP_SHA512, 0,
	"sve",        "sve",      "scalable vector extensions",                     HWCAP_SVE; 0,
	"asimdfhm",   "fp16fml",  "ASIMD half-precision multiply-accumulate",       HWCAP_ASIMDFHM, 0,
	"dit",        "",         "data-independent timing",                        HWCAP_DIT, 0,
	"uscat",      "",         "large system extensions 2 (unaligned single-copy atomicity)", HWCAP_USCAT, 0,
	"ilrcpc",     "",         "load-acquire RCpc instructions 2",               HWCAP_ILRCPC, 0,
	"flagm",      "flagm",    "flag manipulation",                              HWCAP_FLAGM, 0,
	"ssbs",       "ssbs",     "speculative store bypass safe 2",                HWCAP_SSBS, 0,
	"sb",         "sb",       "speculation barrier",                            HWCAP_SB, 0,
	"paca",       "pauth",    "pointer authentication (address)",               HWCAP_PACA, 0,
	"pacg",       "pauth",    "pointer authentication (generic)",               HWCAP_PACG, 0,

	/* AT_HWCAP2 */
	"dcpodp",     "",          "data persistence writeback 2",                  0, HWCAP2_DCPODP,
        "sve2",       "sve2",      "scalable vector extensions 2",                  0, HWCAP2_SVE2,
	"sveaes",     "sve2-aes",  "SVE advanced encryption standard",              0, HWCAP2_SVEAES,    
	"svepmull",   "sve2-aes",  "SVE polynomial multiply long",                  0, HWCAP2_SVEPMULL,
	"svebitperm", "sve2-bitperm", "SVE bit permute",                            0, HWCAP2_BITPERM,
	"svesha3",    "sve2-sha3", "SVE secure hashing algorithm 3",                0, HWCAP2_SVESHA3,
	"svesm4",     "sve2-sm4",  "SVE SM3 cryptographic hash algorithm",          0, HWCAP2_SVESM4,
	"flagm2",     "",          "flag manipulation 2",                           0, HWCAP2_FLAGM2,
	"frint",      "",          "floating-point round to 32/64-bit integer"      0, HWCAP2_FRINT,
	"svei8mm",    "i8mm",      "SVE int8 matrix multiplication",                0, HWCAP2_SVEI8MM,
	"svef32mm",   "f32mm",     "SVE F32 matrix multiplication",                 0, HWCAP2_SVEF32MM,
	"svef64mm",   "f64mm",     "SVE F64 matrix multiplication",                 0, HWCAP2_SVEF64MM,
	"svebf16",    "bf16",      "SVE bfloat16 arithmetic",                       0, HWCAP2_BF16,
	"i8mm",       "i8mm",      "int8 matrix multiplication",                    0, HWCAP2_I8MM,
	"bf16",       "bf16",      "bfloat16 arithmetic",                           0, HWCAP2_BF16,
	"dgh",        "",          "data gathering hint"                            0, HWCAP2_DGH,
	"rng",        "rng",       "random number generation",                      0, HWCAP2_RNG,
	"bti",        "",          "branch target identification",                  0, HWCAP2_BTI,
	"mte",        "",          "memory tagging extension",                      0, HWCAP2_MTE,
	"ecv",        "",          "enhanced counter virtualization",               0, HWCAP2_ECV,
	"afp",        "",          "alternate floating-point behavior",             0, HWCAP2_AFP,
	"rpres",      "",          "12 bits reciprocal estimate/sqrt mantissa"      0, HWCAP2_RPRES,
	"mte3",       "",          "memory tagging extension 3",                    0, HWCAP2_MTE3,
	"sme",        "sme",       "scalable matrix extension",                     0, HWCAP2_SME,
	"sme_i16i64", "sme-i16i64", "SME accumulate int16 into int64",              0, HWCAP2_SME_I16I64",
	"sme_f64f64", "sme-f64f64", "SME accumulate FP64 into FP64",                0, HWCAP2_SME_F64F64",
	"sme_i8i32",  "",          "SME accumulate int8 into int32",                0, HWCAP2_SME_I8I32,
	"sme_f16f32", "",          "SME accumulate FP16 into FP32",                 0, HWCAP2_SME_F16F32,
	"sme_b16f32", "",          "SME accumulate BF16 into FP32",                 0, HWCAP2_SME_B16F32,
	"sme_f32f32", "",          "SME accumulate FP32 into FP32",                 0, HWCAP2_SME_F32F32,
	"sme_fa64",   "",          "all instructions supported in streaming SVE mode", 0, HWCAP2_SME_FA64,
	"wfxt",       "",          "wait for event/interrupt with timeout",         0, HWCAP2_SME_WFXT,
	"ebf16",      "",          "bfloat16 arithmetic 2",                         0, HWCAP2_EBF16,
	"sve_ebf16",  "",          "SVE bfloat16 arithmetic 2",                     0, HWCAP2_SVE_EBF16,
	"cssc",       "cssc",      "common short sequence compression",             0, HWCAP2_CSSC,
	"rprfm",      "",          "range prefetch memory",                         0, HWCAP2_RPRFM,
	"sve2p1",     "",          "scalable vector extension 2.1",                 0, HWCAP2_SVE2P1,
	"sme2",       "sme2",      "scalable matrix extension 2",                   0, HWCAP2_SME2,
	"sme2p1",     "",          "scalable matrix extension 2.1",                 0, HWCAP2_SME2P1,
	"sme_i16i32", "",          "SME accumulate int16 into int32",               0, HWCAP2_SME_I16I32,
	"sme_bi32i32", "",         "SME accumulate 32 bits into int32",             0, HWCAP2_SME_BI32I32,
	"sme_b16b16", "",          "SME accumulate BF16 into BF16",                 0, HWCAP2_SME_B16B16,
	"sme_f16f16", "",          "SME accumulate FP16 into FP16",                 0, HWCAP2_SME_F16F16,
	"mops",       "mops",      "memory copy and memory set",                    0, HWCAP2_MOPS,
	"hbc",        "",          "branch consistently",                           0, HWCAP2_HBC,

	/* architecture levels */
	"armv8-a",     "",         "architecture level Armv8.0",
	    HWCAP_FP|HWCAP_ASIMD, 0,
	"armv8.1-a",   "armv8.1-a", "architecture level Armv8.1",
	    HWCAP_FP|HWCAP_ASIMD|HWCAP_CRC32|HWCAP_ATOMICS|HWCAP_ASIMDRDM, 0,
	"armv8.2-a",   "armv8.2-a", "architecture level Armv8.2",
	    HWCAP_FP|HWCAP_ASIMD|HWCAP_CRC32|HWCAP_ATOMICS|HWCAP_ASIMDRDM|HWCAP_DCPOP, 0,
	/* armv8.3 is armv8.2 plus pauth, but we don't detect pauth individually */
	"armv8.4-a",   "armv8.4-a", "architecture level Armv8.4",
	    HWCAP_FP|HWCAP_ASIMD|HWCAP_CRC32|HWCAP_ATOMICS|HWCAP_ASIMDRDM|HWCAP_DCPOP|
	    HWCAP_FLAGM|HWCAP_ASIMDFHM|HWCAP_ASIMDDP|HWCAP_DIT|HWCAP_ILRCPC|HWCAP_USCA, 0,
	"armv8.5-a",   "armv8.5-a", "architecture level Armv8.5",
	    HWCAP_FP|HWCAP_ASIMD|HWCAP_CRC32|HWCAP_ATOMICS|HWCAP_ASIMDRDM|HWCAP_DCPOP|
	    HWCAP_FLAGM|HWCAP_ASIMDFHM|HWCAP_ASIMDDP|HWCAP_DIT|HWCAP_ILRCPC|HWCAP_USCA|
	    HWCAP_SB, /* SPECRES? */
            HWCAP2_BTI|HWCAP2_DCPODP,
	"armv8.6-a",   "armv8.6-a", "architecture level Armv8.6",
	    HWCAP_FP|HWCAP_ASIMD|HWCAP_CRC32|HWCAP_ATOMICS|HWCAP_ASIMDRDM|HWCAP_DCPOP|
	    HWCAP_FLAGM|HWCAP_ASIMDFHM|HWCAP_ASIMDDP|HWCAP_DIT|HWCAP_ILRCPC|HWCAP_USCA|
	    HWCAP_SB,
	    HWCAP2_BTI|HWCAP2_DCPODP|HWCAP2_I8MM|HWCAP2_BF16|HWCAP2_ECV,
	"armv8.7-a",   "armv8.7-a", "architecture level Armv8.7",
	    HWCAP_FP|HWCAP_ASIMD|HWCAP_CRC32|HWCAP_ATOMICS|HWCAP_ASIMDRDM|HWCAP_DCPOP|
	    HWCAP_FLAGM|HWCAP_ASIMDFHM|HWCAP_ASIMDDP|HWCAP_DIT|HWCAP_ILRCPC|HWCAP_USCA|
	    HWCAP_SB,
	    HWCAP2_BTI|HWCAP2_DCPODP|HWCAP2_I8MM|HWCAP2_BF16|HWCAP2_ECV|HWCAP2_WFXT,
	"armv8.8-a",   "armv8.8-a", "architecture level Armv8.8",
	    HWCAP_FP|HWCAP_ASIMD|HWCAP_CRC32|HWCAP_ATOMICS|HWCAP_ASIMDRDM|HWCAP_DCPOP|
	    HWCAP_FLAGM|HWCAP_ASIMDFHM|HWCAP_ASIMDDP|HWCAP_DIT|HWCAP_ILRCPC|HWCAP_USCA|
	    HWCAP_SB,
	    HWCAP2_BTI|HWCAP2_DCPODP|HWCAP2_I8MM|HWCAP2_BF16|HWCAP2_ECV|HWCAP2_WFXT|
	    HWCAP2_MOPS|HWCAP2_HBC,
	"armv8.9-a",   "armv8.9-a", "architecture level Armv8.9",
	    HWCAP_FP|HWCAP_ASIMD|HWCAP_CRC32|HWCAP_ATOMICS|HWCAP_ASIMDRDM|HWCAP_DCPOP|
	    HWCAP_FLAGM|HWCAP_ASIMDFHM|HWCAP_ASIMDDP|HWCAP_DIT|HWCAP_ILRCPC|HWCAP_USCA|
	    HWCAP_SB,
	    HWCAP2_BTI|HWCAP2_DCPODP|HWCAP2_I8MM|HWCAP2_BF16|HWCAP2_ECV|HWCAP2_WFXT|
	    HWCAP2_MOPS|HWCAP2_HBC|HWCAP2_CSSC,
};


#define MAXCAPS 100
static const struct hwcap *caps[100];
static size_t ncaps = 0;

static void
register_cap(const struct hwcap *cap)
{
	if (ncaps >= MAXCAPS) {
		fprintf(stderr, "too many capabilities\n");
		exit(EX_SOFTWARE);
	}

	caps[ncaps++] = cap;
}

static void
caps_from_auxv(void)
{
	
}

int main(void) {
}
