SRCDIR:=$(shell pwd)
BUILD:=$(SRCDIR)/build
LCOVDIR:=$(BUILD)/lcov_output

ifndef MAKECMDGOALS
MAKECMDGOALS=all
endif

${MAKECMDGOALS}: ${BUILD}/Makefile
	@${MAKE} -q -s -C ${BUILD}  ${MAKECMDGOALS}|| ${MAKE} -s -C ${BUILD} ${MAKECMDGOALS}

BOOSTRAP_ARGS := --systemc -b cudd -b sword -b boolector

ifdef CMAKE
  BOOSTRAP_ARGS += --cmake=${CMAKE}
endif

ifdef CACHE
  BOOSTRAP_ARGS += --cache ${CACHE}
endif

${BUILD}/Makefile:
	./bootstrap.sh -d ${SRCDIR}/deps ${BUILD} ${BOOSTRAP_ARGS}

.PHONY: update
update:
	git pull
	git submodule update

.PHONY: distclean
distclean:
	rm -rf $(BUILD)

.PHONY: doxygen
doxygen:
	doxygen doc/Doxyfile

.PHONY: lcov_with_bc
lcov_with_bc:
	@rm ${LCOVDIR} -Rf
	@mkdir ${LCOVDIR}
	@lcov --quiet --capture --rc lcov_branch_coverage=1 --directory ${BUILD} --base-directory ${SRCDIR}/src --no-external --output-file ${LCOVDIR}/coverage.info
	@genhtml ${LCOVDIR}/coverage.info --branch-coverage --output-directory ${LCOVDIR}

.PHONY: lcov
lcov:
	@rm ${LCOVDIR} -Rf
	@mkdir ${LCOVDIR}
	@lcov --quiet --capture --directory ${BUILD} --base-directory ${SRCDIR}/src --no-external --output-file ${LCOVDIR}/coverage.info
	@genhtml ${LCOVDIR}/coverage.info --output-directory ${LCOVDIR}

