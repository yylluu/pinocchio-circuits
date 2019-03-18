#include "exp-mod.h"

void outsource(struct Input *input, struct NIZKInput *nizkinput, struct Output *output) {


	struct BigNum nn;

	mul_short(&nizkinput->p, &nizkinput->q, &nn);

	struct BigNum phi;
	sub_one(&nizkinput->p);
	sub_one(&nizkinput->q);

	mul_short(&nizkinput->p, &nizkinput->q, &phi);

	//output->isKeyMatched = check_keys(&nn, &input->n, &phi, input->e, &nizkinput->d);


	expModByReciprocal(&nizkinput->b, input->e, &input->n, &input->nReciprocal, &output->res);

}


int check_keys(struct BigNum *nn, struct BigNum *n, struct BigNum *phi, u32 e, struct BigNum *d){
	int i;
	int val = 0;
	if (e == 3) {
		val = 1;
	}
	for(i = 0; i < WORDS; i += 1){
		if(!(n->num[i] == nn->num[i]))
			val = 0;
	}
	/*
	   struct BigNum dTmp;
	   struct BigNum res;
	   for (i = 0; i < WORDS; i += 1) {
	   dTmp.num[i] = d->num[i];
	   }
	   oneBitLeftShift(&dTmp);
	   add(d, &dTmp);
	   mod_short(&dTmp, phi, &res);

	   for(i = 1; i < WORDS; i += 1){
	   if( !(res.num[i] == 0)){
	   val = 0;
	   }
	   }
	   if(!(res.num[0] == 1)){
	   val = 0;
	   }
	 */
	return val;
}


//left shift 1 single bit
void oneBitLeftShift(struct BigNum *a) {
	a->num[WORDS-1] = (a->num[WORDS-1] << 1) & 0x1ffffffff;
	int i;
	for (i = WORDS - 2; i >=0; i = i - 1) {
		a->num[i+1] = a->num[i+1] | ((0x80000000 & a->num[i]) >> 31);
		a->num[i] = (a->num[i] << 1) & 0xffffffff;
	}
}


//is greater or equal
int greaterOrEqual(struct BigNum *a, struct BigNum *b) {
	int flag = 1;
	int val = 1;
	int i;
	for (i = WORDS - 1; i >= 0; i = i - 1) {
		if ( flag == 1 ){
			if(a->num[i] < b->num[i]){
				flag = 0;
				val = 0;
			} 
			if (a->num[i] > b->num[i]){
				flag = 0;
				val = 1;
			}
		}
	}
	return val;
}




//add
void add(struct BigNum *a, struct BigNum *b) {
	u32 carry = 0;
	int i;
	for (i = 0; i < WORDS - 1; i += 1) {
		b->num[i] = a->num[i] + b->num[i]+ carry;
		//carry = (b->num[i] & 0xffffffff00000000) >> 32;
		carry = b->num[i] >> 32;
		b->num[i] = b->num[i] & 0xffffffff;
	}
	b->num[WORDS-1] = a->num[WORDS-1] + b->num[WORDS-1]+ carry;
}


//subtracted by one, for shorter
void sub_one(struct ShortBigNum *a) {
	int i;
	u32 carry1 = 0;
	for (i = 0; i < WORDS_SHORT; i += 1) {
		a->num[i] = a->num[i] + 0xffffffff + carry1;
		carry1 = a->num[i] >> 32;
		a->num[i] = a->num[i] & 0xffffffff;
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
			int carry = res->num[ind] >> 32;
			if( !(carry == 0) ) {
				res->num[ind+1] = res->num[ind+1] + carry;
				res->num[ind] = res->num[ind] & 0xffffffff;
			}
		}
	}
}


//module for shorter
void mod_short(struct BigNum *n, struct BigNum *d, struct BigNum *r) {
	int k;
	for (k = 0; k < WORDS; k += 1) {
		r->num[k] = 0;
	}
	int cond = n->num[WORDS-1] & 0x100000000;
	if ( !(cond == 0) ) {
		oneBitLeftShift(r);
		r->num[0] = r->num[0] | 0x1;
		//r->num[0] = r->num[0] | (n->num[WORDS-1] >> 32);
		if(greaterOrEqual(r, d) == 1) {
			sub(r, d);
		}
	}
	int i;
	int j;
	for (i = WORDS - 1; i >= 0; i = i - 1) {
		for (j = 31; j >= 0; j = j - 1) {
			oneBitLeftShift(r);
			r->num[0] = r->num[0] | ( (n->num[i] >> j) & 0x1 );
			if(greaterOrEqual(r, d) == 1) {
				sub(r, d);
			}
		}
	}
}




//int checkIsTrueReciprocal(struct BigNum *, struct InflatBigNum *);


void sub_new(struct LongBigNum * a, struct LongBigNum * b, struct BigNum * res){

	u32 carry = 1;
	int i;
	for (i = 0; i < WORDS_LONG; i += 1) {
		a->num[i] = a->num[i] + ( (~b->num[i]) & 0xffffffff ) + carry;
		carry = a->num[i] >> 32;
		a->num[i] = a->num[i] & 0xffffffff;
	}
	for (i = 0; i < WORDS; i += 1) {
		res->num[i] = a->num[i];
	}
}


void mul_new(struct BigNum * a, struct InflatBigNum * b, struct LongBigNum * res){

	struct InflatLongBigNum tmp;
	int j;
	int k;
	for (j = 0; j < WORDS; j += 1) {
		for (k = 0; k < WORDS_INFLAT; k += 1) {
			int ind = j + k;
			tmp.num[ind] += a->num[j] * b->num[k];
			int carry = tmp.num[ind] >> 32;
			if( !(carry == 0) ) {
				tmp.num[ind+1] = tmp.num[ind+1] + carry;
				tmp.num[ind] = tmp.num[ind] & 0xffffffff;
			}
		}
	}
	int i;
	for (i = 0; i < WORDS_LONG; i += 1) {
		res->num[i] = tmp.num[i];
	}
}

void divideByMulReciprocal(struct LongBigNum * a, struct InflatBigNum * b, struct InflatBigNum * res1){
	struct SuperBigNum tmpRes;
	int i;
	for (i = 0; i < WORDS_SUPER; i += 1) {
		tmpRes.num[i] = 0;
	}
	int j;
	int k;
	for (j = 0; j < WORDS_LONG; j += 1) {
		for (k = 0; k < WORDS_INFLAT; k += 1) {
			int ind = j + k;
			tmpRes.num[ind] += a->num[j] * b->num[k];
			int carry = tmpRes.num[ind] >> 32;
			if( !(carry == 0) ) {
				tmpRes.num[ind+1] = tmpRes.num[ind+1] + carry;
				tmpRes.num[ind] = tmpRes.num[ind] & 0xffffffff;
			}
		}
	}
	int m;
	for (m = WORDS_SUPER - WORDS_INFLAT + 1; m < WORDS_SUPER; m += 1) {
		int ind = m - WORDS_SUPER + WORDS_INFLAT - 1;
		res1->num[ind] = tmpRes.num[m];
	}
}


void modByUsingReciprocal(struct LongBigNum * n, struct BigNum * d, struct InflatBigNum * dR, struct BigNum * r) {
	struct InflatBigNum quo;
	divideByMulReciprocal(n, dR, &quo);
	struct LongBigNum times;
	mul_new(d, &quo, &times);
	sub_new(n, &times, r);
}


void expModByReciprocal(struct BigNum * b, u32 e, struct BigNum * n, struct InflatBigNum * nReciprocal, struct BigNum *res2) {

	struct LongBigNum tmp;

	//Let's make it only good for 3 as PK exp
	mul(b, b, &tmp);
	modByUsingReciprocal(&tmp, n, nReciprocal, res2);
	mul(res2, b, &tmp);
	modByUsingReciprocal(&tmp, n, nReciprocal, res2);
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
			int carry = res->num[ind] >> 32;
			if( !(carry == 0) ) {
				res->num[ind+1] = res->num[ind+1] + carry;
				res->num[ind] = res->num[ind] & 0xffffffff;
			}
		}
	}
}
