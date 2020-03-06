all: brutef rainbow encrypt

brutef:
	  gcc -fopenmp -O -std=c11 -o brutefd decrypt.c crypwrapper.c readBulk.c `libgcrypt-config --cflags --libs`

rainbow:
	gcc -fopenmp  -std=c11 -o rainb createRainbow.c crypwrapper.c readBulk.c `libgcrypt-config --cflags --libs`

encrypt:
	gcc -std=c11 -O -o encryptf encrypt.c crypwrapper.c `libgcrypt-config --cflags --libs`