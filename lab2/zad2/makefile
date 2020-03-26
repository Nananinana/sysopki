dir:
	gcc -Wall -o main find_readdir.c
	./main ../../ mtime = 0 maxdepth 2
	./main ../../ mtime + 7 
	./main ../../ maxdepth 1 atime - 3 
	./main ../../ maxdepth 3

nftw:
	gcc -Wall -o main find_nftw.c
	./main ../../ mtime + 7 -1
	./main ../../ mtime + 7 1
	./main ../../ atime - 3 0