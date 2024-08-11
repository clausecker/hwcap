#include <assert.h>
#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "hwcap.h"

static char **wanted_caps;

const struct cap *supported_caps[MAXCAPS];
size_t ncaps = 0;

static int
want_cap(const char *name)
{
	size_t i;

	if (wanted_caps == NULL)
		return (1);

	for (i = 0; wanted_caps[i] != NULL; i++)
		if (strcmp(name, wanted_caps[i]) == 0)
			return (1);

	return (0);
}

const struct cap *
have_cap(const char *name)
{
	size_t i;

	for (i = 0; i < ncaps; i++)
		if (strcmp(name, supported_caps[i]->name) == 0)
			return (supported_caps[i]);

	return (NULL);
}

void
register_cap(const struct cap *cap)
{
	if (!want_cap(cap->name))
		return;

	if (ncaps >= MAXCAPS)
		errx(EX_SOFTWARE, "too many capabilities");

	supported_caps[ncaps++] = cap;
}

static void
print_caps(void) {
	size_t i;

	for (i = 0; i < ncaps; i++) {
		if (i > 0)
			putchar(' ');

		printf("%s", supported_caps[i]->name);
	}

	putchar('\n');
}

static void
print_caps_verbose(void) {
	size_t i;

	for (i = 0; i < ncaps; i++)
		printf("%-15s %s\n", supported_caps[i]->name, supported_caps[i]->descr);
}

static void
print_archlevel(void) {
	const struct cap *lvl;

	lvl = get_archlevel();
	if (lvl != NULL)
		puts(get_archlevel()->name);
}

static int
all_caps_supported(char **args) {
	size_t i, j;

	for (i = 0; args[i] != NULL; i++) {
		for (j = 0; j < ncaps; j++)
			if (strcmp(args[i], supported_caps[j]->name) == 0)
				goto capability_supported;

		return (0);

	capability_supported:
		;
	}

	return (1);
}

static enum {
	MODE_FLAGS,   /* -f */
	MODE_VERBOSE, /* -v */
	MODE_QUERY,   /* -q */
	MODE_CFLAGS,  /* -c */
	MODE_LEVEL,   /* -l */
} mode;

static enum {
	SOURCE_DEFAULT,
	SOURCE_HWCAP,   /* -h */
	SOURCE_CPUID,   /* -i */
	SOURCE_ALL,    /* -a */
//	SOURCE_ISA,     /* -m */
} source;

int main(int argc, char *argv[]) {
	int opt;

	while (opt = getopt(argc, argv, "fvqclhia"), opt != -1)
		switch (opt) {
		case 'f': mode = MODE_FLAGS;   break;
		case 'v': mode = MODE_VERBOSE; break;
		case 'q': mode = MODE_QUERY;   break;
		case 'c': mode = MODE_CFLAGS;  break;
		case 'l': mode = MODE_LEVEL;   break;

		case 'h': source = SOURCE_HWCAP; break;
		case 'i': source = SOURCE_CPUID; break;
		case 'a': source = SOURCE_ALL;  break;
//		case 'm': source = SOURCE_ISA;   break;
		case '?':
		default:
			fprintf(stderr, "usage: %s (-fvqcl) (-hiam) [cap...]\n",
			    basename(argv[0]));
			return (EX_USAGE);
		}

	if (optind < argc)
		wanted_caps = argv + optind;

	if (source == SOURCE_DEFAULT)
		/* machine dependent */
		source = SOURCE_HWCAP;

	switch (source) {
	case SOURCE_HWCAP: caps_from_auxv(); break;
//	case SOURCE_CPUID: caps_from_cpuid(); break;
	case SOURCE_ALL:   caps_all(); break;
//	case SOURCE_ISA:   caps_from_isa(); break;
	}
	
	switch (mode) {
	case MODE_FLAGS:   print_caps(); break;
	case MODE_VERBOSE: print_caps_verbose(); break;
	case MODE_CFLAGS:  print_cflags(); break;
	case MODE_LEVEL:   print_archlevel(); break;
	case MODE_QUERY:
		return (all_caps_supported(argv + optind)
		    ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
