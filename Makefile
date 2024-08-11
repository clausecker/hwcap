PROG=	hwcap
SRCS=	hwcap.c
CFLAGS+=	-Wall -Wno-missing-braces

HWCAP_ARCH=	${MACHINE_ARCH}
.if exists(hwcap_${HWCAP_ARCH}.c)
SRCS+=	hwcap_${HWCAP_ARCH}.c
.else
SRCS+=	hwcap_generic.c
.endif

.include <bsd.prog.mk>
