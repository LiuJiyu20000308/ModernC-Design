doc : DesignPattern.tex
	xelatex $<
	xelatex $<
	make clean

clean:
	$(RM) *.aux *.bbl *.blg *.log *.dvi *.toc *~ *.out *.synctex.gz
	rm -r auto
