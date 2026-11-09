PORTNAME=	eigensoft
DISTVERSION=	9.0.0
CATEGORIES=	biology

MAINTAINER=	KoenigIvan@proton.me
COMMENT=	Population genetics methods for detecting and correcting stratification
WWW=		https://github.com/DReichLab/EIG

LICENSE=	BSD3CLAUSE
LICENSE_FILE=	${WRKSRC}/LICENSE.txt

LIB_DEPENDS=	libgsl.so:math/gsl \
		libopenblas.so:math/openblas
RUN_DEPENDS=	convertf:biology/admixtools

USES=		gmake localbase:ldflags perl5 shebangfix
USE_GITHUB=	yes
GH_ACCOUNT=	DReichLab
GH_PROJECT=	EIG
GH_TAGNAME=	04971e5

SHEBANG_FILES=	bin/evec2pca.perl \
		bin/gc.perl \
		bin/ploteig \
		bin/smarteigenstrat.perl \
		bin/smartpca.perl
perl_OLD_CMD+= /usr//bin/perl

BUILD_WRKSRC=	${WRKSRC}/src
ALL_TARGET=	all install

BIN_PROGRAMS=	baseprog eigenstrat eigenstratQTL pca pcatoy \
		smarteigenstrat smartpca smartrel twstats
BIN_SCRIPTS=	evec2pca.perl gc.perl ploteig \
		smarteigenstrat.perl smartpca.perl

do-install:
.for p in ${BIN_PROGRAMS}
	${INSTALL_PROGRAM} ${WRKSRC}/bin/${p} ${STAGEDIR}${PREFIX}/bin
.endfor
.for s in ${BIN_SCRIPTS}
	${INSTALL_SCRIPT} ${WRKSRC}/bin/${s} ${STAGEDIR}${PREFIX}/bin
.endfor

.include <bsd.port.mk>
