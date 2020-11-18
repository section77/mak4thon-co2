.PHONY: clean

TARGETS = ./README.pdf ./Visualisierung/grafana/README.pdf ./Visualisierung/poll-volkszaehler/README.pdf

all: $(TARGETS)

%.pdf: %.md
	pandoc -s -V papersize:a4 -V margin-left:2cm -V margin-top:2cm $^ -o $@

clean:
	rm -f $(TARGETS)
