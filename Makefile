# make the whole project
all:
	cd mercury && ./genmake.sh > Makefile && make
	cd ..

# clean the whole project
clean:
	cd mercury && make clean
	cd ..

test:
	./testall.sh
