#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define SIZE 1000

// Number of items that will be produced. This number is less than the size of the buffer. Hence, we can model the buffer as being unbounded.
#define LINE_COUNT 50


//buffer 1 variables
char buffer_1[SIZE*LINE_COUNT];

int count_1 = 0;

int prod_idx_1 = 0;

int con_idx_1 = 0;

pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


//buffer 2 vars
int buffer_2[SIZE*LINE_COUNT];

int count_2 = 0;

int prod_idx_2 = 0;

int con_idx_2 = 0;

pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;


//buffer 3 vars
int buffer_3[SIZE*LINE_COUNT];

int count_3 = 0;

int prod_idx_3 = 0;

int con_idx_3 = 0;

pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

//buffer 1 get & put from the example
void put_buff_1(char item){
	pthread_mutex_lock(&mutex_1);
	buffer_1[prod_idx_1] = item;
	prod_idx_1 = prod_idx_1 + 1;
	count_1++;
	pthread_cond_signal(&full_1);
	pthread_mutex_unlock(&mutex_1);
}

char get_buff_1(){
	pthread_mutex_lock(&mutex_1);
	while(count_1==0)
		pthread_cond_wait(&full_1, &mutex_1);
	char item = buffer_1[con_idx_1];
	con_idx_1 = con_idx_1 + 1;
	count_1--;

	pthread_mutex_unlock(&mutex_1);

	return item;
}

//buffer 2 get & put from the example
void put_buff_2(char item){
	pthread_mutex_lock(&mutex_2);
	buffer_2[prod_idx_2] = item;
	prod_idx_2 = prod_idx_2 + 1;
	count_2++;
	pthread_cond_signal(&full_2);
	pthread_mutex_unlock(&mutex_2);
}

char get_buff_2(){
	pthread_mutex_lock(&mutex_2);
	while(count_2==0)
		pthread_cond_wait(&full_2, &mutex_2);
	char item = buffer_2[con_idx_2];
	con_idx_2 = con_idx_2 + 1;
	count_2--;

	pthread_mutex_unlock(&mutex_2);

	return item;
}


//buffer 3 get & put from the example
void put_buff_3(char item){
	pthread_mutex_lock(&mutex_3);
	buffer_3[prod_idx_3] = item;
	prod_idx_3 = prod_idx_3 + 1;
	count_3++;
	pthread_cond_signal(&full_3);
	pthread_mutex_unlock(&mutex_3);
}

char get_buff_3(){
	pthread_mutex_lock(&mutex_3);
	while(count_3==0)
		pthread_cond_wait(&full_3, &mutex_3);
	char item = buffer_3[con_idx_3];
	con_idx_3 = con_idx_3 + 1;
	count_3--;

	pthread_mutex_unlock(&mutex_3);

	return item;
}


//functions to be used by input thread
void *get_input(void *args){

	for(int i=0; i< LINE_COUNT; i++){
		char* line=NULL;
		int lineC=0;
		size_t len=-1;
		lineC= getline(&line, &len, stdin);
		if(strcmp(line, "STOP\n")==0){
			put_buff_1(0);					//if stop has been read, push 0 as an indicator of STOP\n
			free(line);
			break;
		}
		else{
			for(int x=0; x<lineC; x++)		//otherwise just push to the buffer
				put_buff_1(line[x]);
		}
		free(line);
	}
	return NULL;
}

void *lineSep(void *args){
	char item=-1;
	while(item!=0){
		item = get_buff_1();
		if(item=='\n')						//if the \n push a space instead
			item=' ';
		put_buff_2(item);
	}
	return NULL;
}

void *plusAr(void *args){
	char plus;
	char curr=-1;
	int i=0;

	while(curr!=0){
		//if in first iteration, initialize curr to a value
		if(i==0){
			curr = get_buff_2();
			i=1;
		}
		//this statement checks if a plus sign is read, if so if the next is also a plus push ^, otherwise push +
		if(curr=='+'){
			plus=curr;
			curr=get_buff_2();
			if(curr=='+' && plus== '+'){
				put_buff_3('^');
				curr=get_buff_2();
			}
			else
				put_buff_3(plus);
		}

		//otherwise just push the current char
		else{
			put_buff_3(curr);
			curr=get_buff_2();
		}
	}

	return NULL;	
	
}

void *outThread(void *args){

	char outBuff[81];

	//initialize the counter variable
	int ctr=0;
	while(1){
		ctr=0;	
		while(ctr!=81){					//while ctr has not read until end of buffer
			char x=get_buff_3();
			if(x==0)					//if stop is read, quit program
				break;					
			outBuff[ctr]=x;				//start filling print buffer
			printf("%c", outBuff[ctr]);
			ctr++;
		}
		outBuff[81]='\0';
		printf("\n");
		if(count_3<=80)
			return NULL;
	}
	return NULL;
}



int main(){


	//initialize pthread vars
	pthread_t input_t, lineSep_t, caret_t, out_t;


	pthread_create(&input_t, NULL, get_input, NULL);
	pthread_create(&lineSep_t, NULL, lineSep, NULL);
	pthread_create(&caret_t, NULL, plusAr, NULL);
	pthread_create(&out_t, NULL, outThread, NULL);

	pthread_join(input_t, NULL);
	pthread_join(lineSep_t, NULL);
	pthread_join(caret_t, NULL);
	pthread_join(out_t, NULL);

	return EXIT_SUCCESS;
}
