HWCAP(1) - FreeBSD General Commands Manual

# NAME

**hwcap** - query hardware capabilities

# SYNOPSIS

**hwcap**
\[**-cflqv**]
\[**-ahimt**]
\[*capability&nbsp;...*]  
**hwcap**
\[**-cflqv**]
**-I**
*isa-string*
\[*capability&nbsp;...*]

# DESCRIPTION

The
**hwcap**
utility queries the capabilities of the hardware it runs on.
Capabilities are sourced from a user-defined capability source.
If any capabilities are listed as arguments,
the set of capabilities is filtered to only those requested.
The requested capabilities are then returned in a user-defined
format.

The following options control capability source; the default
capability source is architecture-dependent:

**-a**

> All capabilities known are listed.
> This is never the default capability source.
> This can be used to translate the capabilities given as arguments to
> **hwcap**
> some other format.

**-h**

> Capabilities are sourced
> from the
> `AT_HWCAP`
> and
> `AT_HWCAP`
> elf auxiliary vectors as given by
> elf\_aux\_info(3).

**-i**

> On
> **riscv64**,
> the RISC-V ISA string is used to determine capabilities.

**-I** *isa-string*

> Like
> **-i**,
> but the given
> *isa-string*
> is used.

**-m**

> Hardware-specific capability-identification registers are
> used to determine capabilities.

**-t**

> Capabilities are determined by trial of affected instructions.

The following options control the output format:

**-c**

> Print a list of options for
> cc(1)
> or
> c++(1)
> to enable the generation of instructions corresponding to all
> requested capabilities.

**-f**

> Capabilities are printed as flags, intended to match the
> flags listed in
> */proc/cpuinfo*
> on Linux&#174; systems.
> This is the default output format.

**-l**

> Print the highest supported architecture level.

**-q**

> Query support of capabilities and return a zero (true) exit status
> if and only if all capabilities requested are supported by the
> capability source.
> No output is produced.

**-v**

> Print flags with a brief description of their meaning.

# EXIT STATUS

The
**hwcap**
utility exits 0 on success or if all requested capabilies were detected.
If no error occured but some requested capabilities were not detected,
the utility exits 1.
On error, an exit status of &gt;63 is returned.

# CAVEATS

The set of detected capabilities may vary depending on capability source
as not all capability sources can supply information about all capabilities.

# SEE ALSO

arch(7),
elf\_aux\_info(3),
linprocfs(5),
simd(7),
uname(1).

# AUTHOR

Robert Clausecker &lt;[fuz@FreeBSD.org](mailto:fuz@FreeBSD.org)&gt;

FreeBSD 14.1-RELEASE - August 11, 2024
