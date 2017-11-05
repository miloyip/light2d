TARGETS=basic csg
OUTPUTS=$(addsuffix .png, $(TARGETS))

all: $(TARGETS)
test: $(TARGETS) $(OUTPUTS)

%.: %.c
	gcc -Wall -O3 -o $@ $<

%.png: %
	time ./$<

clean:
	rm $(TARGETS) *.png
