.PHONY: all

all:
	bash script/build_iso.bash

clean:
	rm -r build*
