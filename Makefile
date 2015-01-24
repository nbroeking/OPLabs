# make the whole project
all:
	cd mercury && ./genmake.py > Makefile && make
	cd ..

# clean the whole project
clean:
	cd mercury && make clean
	cd ..

# run all of the tests
test:
	./testall.sh
