all:
	cd mercury && ./genmake.sh > Makefile && make
	cd ..

clean:
	cd mercury && make clean
	cd ..

test:
	./runtests.sh
