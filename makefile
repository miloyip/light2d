TARGETS=basic csg shapes
OUTPUTS=$(addsuffix .png, $(TARGETS))
TEXFILES=$(basename $(wildcard *.tex))
DIAGRAMS=$(addsuffix .png, $(TEXFILES))

all: $(TARGETS)
test: $(TARGETS) $(OUTPUTS)
diagram: $(DIAGRAMS)

%.: %.c
	gcc -Wall -O3 -o $@ $<

%.png: %
	time ./$<

%.png: %.tex
	xelatex $<
	convert $(basename $<).pdf $(basename $<).png
	rm $(basename $<).aux $(basename $<).log $(basename $<).pdf

clean:
	rm $(TARGETS) *.png
