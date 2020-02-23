UML=plantuml
DOT=dot
OUT=output
SRC= $(wildcard *.dot)
OBJ= $(SRC:.dot=.jpg)

all: datadir connection_protocole.png $(OBJ)

datadir:
	mkdir -p $(OUT)

connection_protocole.png: 04_ConnectionProtocole.txt
	$(UML) $^ -o $(OUT)/

%.jpg: %.dot
	$(DOT) -Tjpg $^ -o $(OUT)/$@

.PHONY: clean

clean:
	rm -rf $(OUT)
