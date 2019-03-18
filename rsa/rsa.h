#ifndef _BIGNUM_H_
#define _BIGNUM_H_

#define u32 unsigned int

#define WORD_WIDTH 		64
#define NEG_ONE			0xffffffffffffffff
#define NEG_ONE_OVERFLOW	0x1ffffffffffffffff
#define SIGN_BIT		0x8000000000000000

#define BITS 		 	2048

#define WORDS 			(BITS / WORD_WIDTH)
#define WORDS_INFLAT		(BITS / WORD_WIDTH + 2)
#define WORDS_LONG 		((2*BITS) / WORD_WIDTH)
#define WORDS_LONG_INFLAT	((2*BITS) / WORD_WIDTH + 2)
#define WORDS_SHORT 		((BITS/2) / WORD_WIDTH)
#define WORDS_SUPER		((3*BITS) / WORD_WIDTH + 2)
#define RECIPROCAL_SHIFT	(2 * BITS + WORD_WIDTH)
#define PK_BITS 		2 //PK=3


struct ShortBigNum {
    u32 num[WORDS_SHORT];
};

struct BigNum {
    u32 num[WORDS];
};

struct InflatBigNum {
    u32 num[WORDS_INFLAT];
};

struct LongBigNum {
    u32 num[WORDS_LONG];
};

struct InflatLongBigNum {
    u32 num[WORDS_LONG_INFLAT];
};

struct SuperBigNum {
    u32 num[WORDS_SUPER];
};


int check_keys(struct BigNum *, struct BigNum *, struct BigNum *, u32, u32, struct BigNum *);
int checkIsTrueReciprocal(struct BigNum *, struct InflatBigNum *);
void expModByReciprocal(struct BigNum *, u32, struct BigNum *, struct InflatBigNum *, struct BigNum *);
void sub_new(struct LongBigNum *, struct LongBigNum *, struct BigNum *);
void modByUsingReciprocal(struct LongBigNum *, struct BigNum *, struct InflatBigNum *, struct BigNum *);
void divideByMulReciprocal(struct LongBigNum *, struct InflatBigNum *, struct InflatBigNum *);
void mul_new(struct BigNum *, struct InflatBigNum *, struct LongBigNum *);
void mul(struct BigNum *, struct BigNum *, struct LongBigNum *);
void oneBitLeftShift(struct BigNum *);
void add(struct BigNum *, struct BigNum *);
void mul_short(struct ShortBigNum *, struct ShortBigNum *, struct BigNum *res);
void sub_one_short(struct ShortBigNum *);

struct Input {
    u32 e;				//PK exp
    struct BigNum n;			//base: p*q
    struct InflatBigNum nReciprocal; 	//Reciprocal of n
};

struct NIZKInput {
    //u32 t; 				//e * d  = (p-1) * (q-1) * t + 1 
    //struct ShortBigNum p; 		//prime p
    //struct ShortBigNum q; 		//prime q
    //struct BigNum d;			//SK exp
    struct BigNum b;			//decrypted data
};

struct Output {
    //u32 isMatched;
    struct BigNum res;		//encrypted data
};

void outsource(struct Input *input, struct NIZKInput *nizkinput, struct Output *output);
#endif
