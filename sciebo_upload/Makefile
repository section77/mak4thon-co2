.PHONY: clean

TARGETS = ./Struktur.pdf ./Visualisierung/grafana/README.pdf ./Visualisierung/poll-volkszaehler/README.pdf

all: $(TARGETS)

%.pdf: %.md
	pandoc -s -V papersize:a4  $^ -o $@

clean:
	rm -f $(TARGETS)
