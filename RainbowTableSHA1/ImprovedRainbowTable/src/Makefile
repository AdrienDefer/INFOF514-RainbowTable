ImprovedTable:
	gcc -o main main.c SHA1/sha1.c -lpthread
	time ./main 0 $(g) $(cl)
	sort -u -t' ' -k2,2 Files/UnsortedTables/UnsortedRainbowTable.txt > ../../Files/RainbowTables/WNSRainbowTable.txt
	rm main

Crack:
	gcc -o main main.c SHA1/sha1.c -lpthread
	time ./main 1 $(cl) $(htc)
	rm main

GenerateHashFile:
	gcc -o main main.c SHA1/sha1.c -lpthread
	./main 2
	rm main

CrackFile:
	gcc -o main main.c SHA1/sha1.c -lpthread
	time ./main 3 $(cl)
	rm main