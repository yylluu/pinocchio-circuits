#include "keyChecking.h"

void outsource(struct Input *input, struct NIZKInput *nizkinput, struct Output *output) {
	
	//Verify what n can be produced by p and q
	struct BigNum nn;
	mul_short(&nizkinput->p, &nizkinput->q, &nn);

	//Calcualte phi by multiplying p-1 and q-1
	struct BigNum phi;
	sub_one_short(&nizkinput->p);
	sub_one_short(&nizkinput->q);
	mul_short(&nizkinput->p, &nizkinput->q, &phi);

	//verify n's reciprocal is correct
	int isReciprocal = checkIsTrueReciprocal(&input->n, &input->nReciprocal);

	//verify phi, n get from public, n produced by p and q, e and d are matched
	output->isMatched = (isReciprocal & check_keys(&nn, &input->n, &phi, nizkinput->t, input->e, &nizkinput->d));
	//struct BigNum res;
	//expModByReciprocal(&nizkinput->b, input->e, &input->n, &input->nReciprocal, &res);

	
}




int check_keys(struct BigNum *nn, struct BigNum *n, struct BigNum *phi, u32 t, u32 e, struct BigNum *d){
	int i;
	int val = 0;

	if (e == 3) {
		val = 1;
	}

	// val = given n != produced n ?  0 : val 
	for(i = 0; i < WORDS; i += 1){
		if(!(n->num[i] == nn->num[i]))
			val = 0;
	}

	//res = e * d
	struct BigNum res;
	for (i = 0; i < WORDS; i += 1) {
		res.num[i] = d->num[i];
	}
	oneBitLeftShift(&res);
	add(d, &res); //res = e * d when e = 3
	
	//res1 = phi * N + 1
	struct BigNum res1;
	for (i = 0; i < WORDS - 1; i += 1) {
		res1.num[i] += phi->num[i] * t;
		u32 carry = res1.num[i] >> WORD_WIDTH;
		res1.num[i+1] += carry;
		res1.num[i] = res1.num[i] & NEG_ONE;
	}
	res1.num[WORDS - 1] += phi->num[WORDS - 1] * t;
	add_one(&res1);

	// val = (e * d) != (phi * N + 1) ?  0 : val 
	for(i = 0; i < WORDS; i += 1) {
		if( !(res1.num[i] == res.num[i])){
			val = 0;
		} 
	}

	return val;
}




void oneBitLeftShift(struct BigNum *a) {
	a->num[WORDS-1] = (a->num[WORDS-1] << 1) & NEG_ONE_OVERFLOW;
	int i;
	for (i = WORDS - 2; i >=0; i = i - 1) {
		a->num[i+1] = a->num[i+1] | ((SIGN_BIT & a->num[i]) >> (WORD_WIDTH-1));
		a->num[i] = (a->num[i] << 1) & NEG_ONE;
	}
}




void add(struct BigNum *a, struct BigNum *b) {
	u32 carry = 0;
	int i;
	for (i = 0; i < WORDS - 1; i += 1) {
		b->num[i] = a->num[i] + b->num[i]+ carry;
		carry = b->num[i] >> WORD_WIDTH;
		b->num[i] = b->num[i] & NEG_ONE;
	}
	b->num[WORDS-1] = a->num[WORDS-1] + b->num[WORDS-1]+ carry;
}




void add_one(struct BigNum *a) {
	u32 carry = 1;
	int i;
	for (i = 0; i < WORDS - 1; i += 1) {
		a->num[i] = a->num[i] + carry;
		carry = a->num[i] >> WORD_WIDTH;
		a->num[i] = a->num[i] & NEG_ONE;
	}
	a->num[WORDS-1] = a->num[WORDS-1] + carry;
}



//subtracted by one, for shorter
void sub_one_short(struct ShortBigNum *a) {
	int i;
	u32 carry1 = 0;
	for (i = 0; i < WORDS_SHORT; i += 1) {
		a->num[i] = a->num[i] + NEG_ONE + carry1;
		carry1 = a->num[i] >> WORD_WIDTH;
		a->num[i] = a->num[i] & NEG_ONE;
	}
}



//multiplication for shorter
void mul_short(struct ShortBigNum *a, struct ShortBigNum *b, struct BigNum *res) {
	int i;
	for (i = 0; i < WORDS; i += 1) {
		res->num[i] = 0;
	}
	int m;
	int n;
	for (m = 0; m < WORDS_SHORT; m += 1) {
		for (n = 0; n < WORDS_SHORT; n += 1) {
			int ind = n + m;
			res->num[ind] += a->num[m] * b->num[n];
			u32 carry = res->num[ind] >> WORD_WIDTH;
			//if( !(carry == 0) ) {
				res->num[ind+1] = res->num[ind+1] + carry;
				res->num[ind] = res->num[ind] & NEG_ONE;
			//}
		}
	}
}




int checkIsTrueReciprocal(struct BigNum * n, struct InflatBigNum * nRe){
	struct InflatLongBigNum tmp;
	int i;
	int j;
	for (i = 0; i < WORDS; i += 1) {
		for (j = 0; j < WORDS_INFLAT; j += 1) {
			int ind = i + j;
			tmp.num[ind] += n->num[i] * nRe->num[j];
			u32 carry = tmp.num[ind] >> WORD_WIDTH;
			//if( !(carry == 0) ) {
				tmp.num[ind+1] = tmp.num[ind+1] + carry;
				tmp.num[ind] = tmp.num[ind] & NEG_ONE;
			//}
		}
	}
	int flag = 1;
	if ( !(tmp.num[WORDS + WORDS_INFLAT - 1] == 0) ){
		flag = 0;
	}
	for (i = WORDS + WORDS_INFLAT - 2; i >= WORDS + WORDS_INFLAT - 1 - WORDS; i = i - 1) {
			if ( !(tmp.num[i] == NEG_ONE) ){
				flag = 0;
			}
	}
	return flag;
}



void expModByReciprocal(struct BigNum * b, u32 e, struct BigNum * n, struct InflatBigNum * nReciprocal, struct BigNum *res) {
	
	struct LongBigNum tmp;

//Let's make it only good for 3 as PK exp
	mul(b, b, &tmp);
	modByUsingReciprocal(&tmp, n, nReciprocal, res);
	mul(res, b, &tmp);
	modByUsingReciprocal(&tmp, n, nReciprocal, res);
}




void modByUsingReciprocal(struct LongBigNum * n, struct BigNum * d, struct InflatBigNum * dR, struct BigNum * r) {
	struct InflatBigNum quo;
	divideByMulReciprocal(n, dR, &quo);
	struct LongBigNum times;
	mul_new(d, &quo, &times);
	sub_new(n, &times, r);
}




void divideByMulReciprocal(struct LongBigNum * a, struct InflatBigNum * b, struct InflatBigNum * res1){
	struct SuperBigNum tmpRes;
	int i;
	for (i = 0; i < WORDS_SUPER; i += 1) {
		tmpRes.num[i] = 0;
	}
	int j;
	for (i = 0; i < WORDS_LONG; i += 1) {
		for (j = 0; j < WORDS_INFLAT; j += 1) {
			int ind = j + i;
			tmpRes.num[ind] += a->num[i] * b->num[j];
			u32 carry = tmpRes.num[ind] >> WORD_WIDTH;
			//if( !(carry == 0) ) {
				tmpRes.num[ind+1] = tmpRes.num[ind+1] + carry;
				tmpRes.num[ind] = tmpRes.num[ind] & NEG_ONE;
			//}
		}
	}
	for (i = WORDS_SUPER - WORDS_INFLAT + 1; i < WORDS_SUPER; i += 1) {
		int ind = i - WORDS_SUPER + WORDS_INFLAT - 1;
		res1->num[ind] = tmpRes.num[i];
	}
	res1->num[WORDS_INFLAT-1] = 0;
}




void sub_new(struct LongBigNum * a, struct LongBigNum * b, struct BigNum * res){
	
	u32 carry = 1;
	int i;
	for (i = 0; i < WORDS_LONG; i += 1) {
		a->num[i] = a->num[i] + ( (~b->num[i]) & NEG_ONE ) + carry;
		carry = a->num[i] >> WORD_WIDTH;
		a->num[i] = a->num[i] & NEG_ONE;
	}
	for (i = 0; i < WORDS; i += 1) {
		res->num[i] = a->num[i];
	}
}




void mul_new(struct BigNum * a, struct InflatBigNum * b, struct LongBigNum * res){
	
	struct InflatLongBigNum tmp;
	int i;
	for (i = 0; i < WORDS_LONG_INFLAT; i += 1) {
		tmp.num[i] = 0;
	}
	int j;
	for (i = 0; i < WORDS; i += 1) {
		for (j = 0; j < WORDS_INFLAT; j += 1) {
			int ind = i + j;
			tmp.num[ind] += a->num[i] * b->num[j];
			u32 carry = tmp.num[ind] >> WORD_WIDTH;
			//if( !(carry == 0) ) {
				tmp.num[ind+1] = tmp.num[ind+1] + carry;
				tmp.num[ind] = tmp.num[ind] & NEG_ONE;
			//}
		}
	}
	for (i = 0; i < WORDS_LONG; i += 1) {
		res->num[i] = tmp.num[i];
	}
}




//multiplication
void mul(struct BigNum *a, struct BigNum *b, struct LongBigNum *res) {
	int i;
	for (i = 0; i < WORDS_LONG; i += 1) {
		res->num[i] = 0;
	}
	int j;
	int k;
	for (j = 0; j < WORDS; j += 1) {
		for (k = 0; k < WORDS; k += 1) {
			int ind = j + k;
			res->num[ind] += a->num[j] * b->num[k];
			u32 carry = res->num[ind] >> WORD_WIDTH;
			//if( !(carry == 0) ) {
				res->num[ind+1] = res->num[ind+1] + carry;
				res->num[ind] = res->num[ind] & NEG_ONE;
			//}
		}
	}
}
