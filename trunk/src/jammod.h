/* loadfile.c */
unsigned char *loadfile(const char *path);

/* linker.c */
int elf_relocate(unsigned char *data, unsigned offset);
unsigned elf_get_symbol(unsigned char *data, unsigned offset,
                        const char *name, unsigned char type);

/* symbols.c */
int init_symbols(void);
unsigned get_symbol(char type, const char *name);
unsigned get_symbol_ex(char type, const char *name, ...);

/* kmem.c */
int open_kmem(void);
int rkm(int fd, void *buf, int count, unsigned long off);
int wkm(int fd, void *buf, int count, unsigned long off);
int rkml(int fd, unsigned long *l, unsigned long off);
int wkml(int fd, unsigned long l, unsigned long off);
