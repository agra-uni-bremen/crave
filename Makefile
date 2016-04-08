SRCDIR:=$(shell pwd)
BUILD:=$(SRCDIR)/build

ifndef MAKECMDGOALS
MAKECMDGOALS=all
endif

${MAKECMDGOALS}: ${BUILD}/Makefile
	@${MAKE} -q -s -C ${BUILD}  ${MAKECMDGOALS}|| ${MAKE} -s -C ${BUILD} ${MAKECMDGOALS}


BOOSTRAP_ARGS :=
ifdef CMAKE
  BOOSTRAP_ARGS += --cmake=${CMAKE}
endif

ifdef CACHE
  BOOSTRAP_ARGS += --cache ${CACHE}
endif

${BUILD}/Makefile:
	./bootstrap.sh -d ${SRCDIR}/deps --systemc -b cudd -b sword -b boolector ${BUILD} ${BOOSTRAP_ARGS}

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
	@rm lcov_output -Rf
	@mkdir lcov_output
	@lcov --quiet --capture --rc lcov_branch_coverage=1 --directory ${BUILD} --base-directory ${SRCDIR}/src --no-external --output-file lcov_output/coverage.info
	@genhtml lcov_output/coverage.info --branch-coverage --output-directory lcov_output

.PHONY: lcov
lcov:
	@rm lcov_output -Rf
	@mkdir lcov_output
	@lcov --quiet --capture --directory ${BUILD} --base-directory ${SRCDIR}/src --no-external --output-file lcov_output/coverage.info
	@genhtml lcov_output/coverage.info --output-directory lcov_output

