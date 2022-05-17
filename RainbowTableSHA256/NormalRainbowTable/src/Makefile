Table:
	gcc -o main main.c SHA256/sha256.c -lpthread
	time ./main 0 $(g) $(pl) $(cl)
	sort -u -t' ' -k2,2 Files/UnsortedTables/UnsortedTable$(pl).txt > ../../Files/RainbowTables/RainbowTable_$(pl).txt
	rm main

Crack:
	gcc -o main main.c SHA256/sha256.c -lpthread
	time ./main 1 $(cl) $(pl) $(htc)
	rm main

GenerateHashFile:
	gcc -o main main.c SHA256/sha256.c -lpthread
	time ./main 2 $(pl)
	rm main

CrackFile:
	gcc -o main main.c SHA256/sha256.c -lpthread
	time ./main 3 $(pl) $(cl) $(fhtc)
	rm main