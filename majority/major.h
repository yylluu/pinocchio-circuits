#ifndef _TEST_COUNT_H_
#define _TEST_COUNT_H_

#define u32 		unsigned int
#define BITS 		1792
#define WORDS_BITS 	32
#define WORDS_NUM	BITS / WORDS_BITS
#define PLAYERS_NUM 	11

struct BigNum {
    u32 num[WORDS_NUM];
};

struct Data {
    struct BigNum points[PLAYERS_NUM];
};

struct Input {
    u32 maxCount;
};

struct NIZKInput {
    struct Data d;
};

struct Output {
    u32 isMaxCountMatched;
    struct BigNum maj;
    u32 isMaj[PLAYERS_NUM];
};

int isSmaller(u32 a, u32 b);
int isEqual(struct BigNum * a, struct BigNum * b);

void outsource(struct Input *input, struct NIZKInput *nizkinput, struct Output *output);
#endif
