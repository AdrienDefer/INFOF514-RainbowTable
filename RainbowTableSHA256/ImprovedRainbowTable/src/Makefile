ImprovedTable:
	gcc -o main main.c SHA256/sha256.c -lpthread
	time ./main 0 $(g) $(cl)
	sort -u -t' ' -k2,2 Files/UnsortedTables/UnsortedRainbowTable.txt > ../../Files/RainbowTables/RainbowTable.txt
	rm main

Crack:
	gcc -o main main.c SHA256/sha256.c -lpthread
	time ./main 1 $(cl) $(htc)
	rm main

GenerateHashFile:
	gcc -o main main.c SHA256/sha256.c -lpthread
	time ./main 2
	rm main

CrackFile:
	gcc -o main main.c SHA256/sha256.c -lpthread
	time ./main 3 $(cl)
	rm main