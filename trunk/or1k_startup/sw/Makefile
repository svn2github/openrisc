as:
	or32-uclinux-as OR1K_startup.S -o OR1K_startup.o
	or32-uclinux-ld --oformat ihex -T OR1K_startup.ld OR1K_startup.o -o OR1K_startup.hex
	or32-uclinux-ld --oformat srec -T OR1K_startup.ld OR1K_startup.o -o OR1K_startup.srec
	or32-uclinux-objdump -D OR1K_startup.o > OR1K_startup-dump

vhdl:
	srec_cat OR1K_startup.srec --output OR1K_startup.vhd -VHdl

verilog:
	 srec_cat OR1K_startup.srec --output OR1K_startup.v -VMem

clean:
	rm *.o
	rm *.bin
	rm *.txt

all: as vhdl verilog
