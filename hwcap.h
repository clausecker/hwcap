struct cap {
	const char *name, *cflag, *descr;
};

/* provided by hwcap.c */
void	register_cap(const struct cap *);
const struct cap	*have_cap(const char *);

#define MAXCAPS 100
extern	const struct cap 	*supported_caps[MAXCAPS];
extern	size_t			 ncaps;

/* provided by hwcap_$arch.c */
void	caps_from_auxv(void);
void	caps_all(void);
void	print_cflags(void);
const struct cap 	*get_archlevel(void);
