/* Generates crc for specified binary file.
   We do very dirty hack here -- first occurrence of 0xccccccccdddddddd in output file designates where crc
   and size should be placed. We calculate the crc on the binary and then replace the occurences in the ELF
   file directly. This was done so, we don't need to */

#include <stdio.h>

#define MAX_SIZE 500000
#ifdef __BIG_ENDIAN__
#define SWAP32(x) (x)
#else /* !__BIG_ENDIAN__ */
#define SWAP32(x) ((((x) >> 24) & 0xff) << 0 \
                 | (((x) >> 16) & 0xff) << 8 \
                 | (((x) >> 8) & 0xff) << 16 \
                 | (((x) >> 0) & 0xff) << 24)
#endif /* __BIG_ENDIAN__ */

unsigned char buf[MAX_SIZE];

unsigned long crc32 (unsigned long crc, const unsigned char *buf, unsigned long len)
{
  /* Create bitwise CRC table first */
  unsigned long crc_table[256];
  int i, k;
  for (i = 0; i < 256; i++) {
    unsigned long c = (unsigned long)i;
    for (k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >> 1) : c >> 1;
    crc_table[i] = c;
  }

  /* Calculate crc on buf */
  crc = crc ^ 0xffffffffL;
  while (len--) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
  return crc ^ 0xffffffffL;
}

int main (int argc, char *argv[])
{
  FILE *fi, *fo;
  int size, i, tsize;
  unsigned long crc;
  if (argc != 3) return -1;
	
  fi = fopen (argv[1], "rb");
  fo = fopen (argv[2], "rb+");
  if (!fi || !fo) return 1;
	
  size = fread (buf, 1, MAX_SIZE, fi);
  fclose (fi);
  
  crc = crc32 (0, buf, size);
  tsize = fread (buf, 1, MAX_SIZE, fo);

  for (i = 0; i < tsize; i++)
    if (*((unsigned long *)&buf[i]) == SWAP32(0xcccccccc) && *((unsigned long *)&buf[i + 4]) == SWAP32(0xdddddddd)) {
      *(unsigned long *)&buf[i] = SWAP32(crc);
      *(unsigned long *)&buf[i + 4] = SWAP32(size);
      break;
    }

  if (i >= tsize - 8) return 2;
  fseek (fo, 0l, SEEK_SET);
  fwrite (buf, 1, tsize, fo);
  fclose (fo);
  return 0;
}
