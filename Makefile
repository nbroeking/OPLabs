# make the whole project
all:
	cd mercury && ./genmake.sh > Makefile && make
	cd ..

clean:
	cd mercury && make clean
	cd ..

test:
	./testall.sh
