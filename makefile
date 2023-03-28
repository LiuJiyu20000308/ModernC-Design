doc : DesignPattern.tex
	pdflatex $<
	pdflatex $<
	make clean

clean:
	$(RM) *.aux *.bbl *.blg *.log *.dvi *.toc *~ *.out *.synctex.gz
	rm -r auto
