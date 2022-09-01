ifeq ($(OS),Windows_NT) 
outfile=flipperzero_badusb_kl.exe
else
outfile=flipperzero_badusb_kl
endif

${outfile}: main.c
	gcc main.c -o ${outfile}

all: ${outfile}
