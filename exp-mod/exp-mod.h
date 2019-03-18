#ifndef _BIGNUM_H_
#define _BIGNUM_H_

#define u32 unsigned int

#define BITS 		 	256
#define WORD_WIDTH 		32
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

void exp_mod(struct BigNum *, u32, struct BigNum *, struct BigNum *);
void mul(struct BigNum *, struct BigNum *, struct LongBigNum *);
void mod(struct LongBigNum *, struct BigNum *, struct BigNum *);
void div(struct LongBigNum *, struct BigNum *, struct BigNum *);
void oneBitLeftShift(struct BigNum *);
int greaterOrEqual(struct BigNum *, struct BigNum *);
void sub(struct BigNum *, struct BigNum *);


int checkIsTrueReciprocal(struct BigNum *, struct InflatBigNum *);
void expModByReciprocal(struct BigNum *, u32, struct BigNum *, struct InflatBigNum *, struct BigNum *);
void sub_new(struct LongBigNum *, struct LongBigNum *, struct BigNum *);
void modByUsingReciprocal(struct LongBigNum *, struct BigNum *, struct InflatBigNum *, struct BigNum *);
void divideByMulReciprocal(struct LongBigNum *, struct InflatBigNum *, struct InflatBigNum *);
void mul_new(struct BigNum *, struct InflatBigNum *, struct LongBigNum *);


int check_keys(struct BigNum *, struct BigNum *, struct BigNum *, u32, struct BigNum *);

void add(struct BigNum *, struct BigNum *);
void mod_short(struct BigNum *, struct BigNum *, struct BigNum *);
void mul_short(struct ShortBigNum *, struct ShortBigNum *, struct BigNum *res);
void sub_one(struct ShortBigNum *);

struct Input {
    u32 e;		//PK exp
    struct BigNum n;
    struct InflatBigNum nReciprocal;
};

struct NIZKInput {
    struct ShortBigNum p; //prime p
    struct ShortBigNum q; //prime q
    struct BigNum d;	//SK exp
    struct BigNum b;	//encrypted data
};

struct Output {
    int isKeyMatched;	//whether e, b, n
    struct BigNum res;	//decrypted data
};

void outsource(struct Input *input, struct NIZKInput *nizkinput, struct Output *output);
#endif
