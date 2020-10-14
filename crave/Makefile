CRAVE_SOLVERS ?= cudd z3

SRCDIR:=$(shell pwd)
BUILD:=$(SRCDIR)/build
LCOVDIR:=$(BUILD)/lcov_output
DEPS_CLEAN:=$(BUILD)/deps-clean
PARENTDIR:=$(shell dirname ${SRCDIR})

ifndef MAKECMDGOALS
MAKECMDGOALS=all
endif

${MAKECMDGOALS}: ${BUILD}/Makefile
	@${MAKE} -q -s -C ${BUILD}  ${MAKECMDGOALS}|| ${MAKE} -s -C ${BUILD} ${MAKECMDGOALS}

BOOSTRAP_ARGS := --systemc $(foreach SOLVER, $(CRAVE_SOLVERS), -b $(SOLVER))

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

$(BUILD)/deps-clean:
	CRAVE_OFFLINE_BUILD=OFF ./bootstrap.sh --boost=do/not/download -d ${DEPS_CLEAN} --download-only ${BOOSTRAP_ARGS}

.PHONY: cleandeps
cleandeps:
	rm -rf ${DEPS_CLEAN}

.PHONY: package_deps
package_deps: $(BUILD)/deps-clean
	cd $(BUILD) && tar -czf crave_package_deps.tar.gz deps-clean && cd $(SRCDIR)

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

