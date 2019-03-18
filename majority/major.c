#include "major.h"

void outsource(struct Input *input, struct NIZKInput *nizkinput, struct Output *output) {

	int i;
	int j;

	int count[PLAYERS_NUM];
	
	for (i = 0; i < PLAYERS_NUM; i+=1) {
		count[i] = 0;
	}

	for (i = 0; i < PLAYERS_NUM; i+=1) {
		for (j = 0; j < PLAYERS_NUM; j+=1) {
			if( isEqual(&(nizkinput->d.points[i]), &(nizkinput->d.points[j])) == 1 ){
				count[i] += 1;
			}
		}
	}


	int flag = 1;
        output->isMaxCountMatched = 0;
	for (i = 0; i < PLAYERS_NUM; i+=1) {
                output->isMaj[i] = 0;
		if ( input->maxCount == count[i] ) {
                    output->isMaj[i] = 1;
		    if (flag == 1) {
		        output->isMaxCountMatched = 1;
			output->maj = nizkinput->d.points[i];
		    }
		}
		if ( isSmaller(input->maxCount, count[i]) == 0x80000000 ) {
                    output->isMaxCountMatched = 0;
		    flag = 0;
		}
	}

}



int isSmaller(u32 a, u32 b) {
	int tmp = (a + ~b + 1);
	return (tmp & 0x80000000);
}


int isEqual(struct BigNum * a, struct BigNum * b) {
	int flag = 1;
	int i;
	for (i = 0; i < WORDS_NUM; i += 1) {
		if (!(a->num[i] == b->num[i])){
			flag = 0;
		}
	}
	return flag;
}
