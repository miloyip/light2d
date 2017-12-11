TARGETS=basic csg shapes reflection refraction fresnel beerlambert beerlambert_color heart
OUTPUTS=$(addsuffix .png, $(TARGETS))
TEXFILES=$(basename $(wildcard *.tex))
DIAGRAMS=$(addsuffix .png, $(TEXFILES))

all: $(TARGETS)
test: $(TARGETS) $(OUTPUTS)
diagram: $(DIAGRAMS)

%: %.c
	gcc -Wall -O3 -o $@ $<

%.png: %
	time ./$<

%.png: %.tex
	xelatex $<
	convert -density 150 $(basename $<).pdf $(basename $<).png
	rm $(basename $<).aux $(basename $<).log $(basename $<).pdf

clean:
	rm $(TARGETS) *.png
