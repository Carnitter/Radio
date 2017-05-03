#ifndef X1205_INCLUDE
#define X1205_INCLUDE

#define BCD2DEC(x) ((((u_char)(x)) >> 4) * 10 + ((x) & 0x0F))
#define DEC2BCD(x) (((((u_char)(x)) / 10) << 4) + (x) % 10)

/* prototypes */
void x1205Init(void);
int x1205Enable(void);
int x1205ReadNByte(u_char, u_char *, u_char);
int x1205WriteNBytes(unsigned char, unsigned char *, u_char);
int x1205WriteBuffer(unsigned char, unsigned char *);

#endif
