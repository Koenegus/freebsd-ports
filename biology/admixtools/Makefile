PORTNAME=	admixtools
DISTVERSION=	8.1.0
CATEGORIES=	biology

MAINTAINER=	KoenigIvan@proton.me
COMMENT=	Tools for population genetic analysis
WWW=		https://github.com/DReichLab/AdmixTools

LICENSE=	BSD3CLAUSE
LICENSE_FILE=	${WRKSRC}/LICENSE.txt

LIB_DEPENDS=	libgsl.so:math/gsl \
		libopenblas.so:math/openblas
RUN_DEPENDS=	Rscript:math/R

USES=		gmake localbase perl5 python:run shebangfix
USE_GITHUB=	yes
GH_ACCOUNT=	DReichLab
GH_PROJECT=	AdmixTools
GH_TAGNAME=	67bc219174a109951a9434e0a383ec464fb140ee

WRKSRC_SUBDIR=	src

SHEBANG_FILES=	../perlsrc/* script/* *.pl

post-patch:
	@${CP} ${FILESDIR}/Makefile ${WRKSRC}/
	@${CP} ${FILESDIR}/strsubs.c ${WRKSRC}/nicksrc/
	@${CP} ${FILESDIR}/transpose.c ${WRKSRC}/
	@${CP} ${FILESDIR}/merge_transpose.c ${WRKSRC}/
	@${REINPLACE_CMD} -e 's|/usr/bin/Rscript|${LOCALBASE}/bin/Rscript|' ${WRKSRC}/rexpfit.r

do-install:
	@${MKDIR} ${STAGEDIR}${PREFIX}/bin
	@${MKDIR} ${STAGEDIR}${PREFIX}/libexec/${PORTNAME}
	@${MKDIR} ${STAGEDIR}${DATADIR}

	(cd ${WRKSRC} && ${INSTALL_PROGRAM} qp3Pop qpDstat qpF4ratio qpAdm qpWave qp4diff \
		dowtjack qpBound qpGraph qpreroot qpff3base qpDpart qpfstats qpfmv qpmix \
		convertf mergeit snpunion simpjack2 grabpars easystats easycheck easylite \
		multimerge geno_single transpose merge_transpose nickhash \
		${STAGEDIR}${PREFIX}/bin)

	(cd ${WRKSRC} && ${INSTALL_SCRIPT} rexpfit.r wtjack.pl ${STAGEDIR}${PREFIX}/bin)

	(cd ${WRKSRC}/../perlsrc && ${INSTALL_SCRIPT} dof4 dof4a fxtract getresult \
		jackdiff mkpretty numlines xtractcol xtractcoltab xtractcolv xtractcolx \
		${STAGEDIR}${PREFIX}/bin)

	(cd ${WRKSRC}/script && ${COPYTREE_BIN} . ${STAGEDIR}${PREFIX}/libexec/${PORTNAME})

	${INSTALL_DATA} ${WRKSRC}/twtable ${STAGEDIR}${DATADIR}/

.include <bsd.port.mk>
