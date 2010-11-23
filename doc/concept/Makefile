all: concept.pdf
loop:
	while sleep 1; do $(MAKE) -s; done
clean:
	-rm -vf concept.pdf *~

include Make/Makefile.tex

$(eval $(call build_latex,concept))
