#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define ESC 27
#define ENTER '\n'
#define L 76
#define l 108
#define MAX_NUM_OF_SECONDS (5*365*24*60*60)

struct tm* RandomDatetimeStruct(){
    unsigned int now_seconds  = (unsigned int)time(NULL);
    unsigned int rand_seconds = (rand()*rand())%(MAX_NUM_OF_SECONDS+1);
    time_t       rand_time    = (time_t)(now_seconds-rand_seconds);
    return localtime(&rand_time);
}
char *randomDatetime();
int putData();
int log_transactions(const char *data, char *log, size_t transaction_count);
int compareDate(const void* pa, const void* pb);
int compareSimple (const void* pa, const void* pb);

struct Buffer{
	union {
		struct {
		
			char timestamp[19];
			char vehicle_registration[8];
			char product;
			int32_t mililiters;
			uint16_t transaction_id;
		};
		char raw[34];
	}data;
};
char data[128][34];
size_t n=0;
char inputbuffer[34];
char a[10000];

int putData(){
	if (n==128){
		printf("Buffer Full");
		return 1;
	}
	//strcpy(&data[n*sizeof(input)],input);
	char vehicle_registration[8] = "AAA 1111";
	memcpy(&data[n][0],randomDatetime(),19);
	memcpy(&data[n][19],vehicle_registration,8);
	data[n][19+8] ='B';
	int32_t mililiters = (-5+n); 
	memcpy(&data[n][19+8+1],&mililiters,sizeof(int32_t));
	
	uint16_t tmp_n = (uint16_t)n;
	memcpy(&data[n][19+8+1+4], &tmp_n,sizeof(uint16_t));
	
	//data[n][23]=0;
	
	fwrite(&data[n][0],1,19,stdout);
	n+=1;
	printf(", transaction_id:%d\n",n);
	return 0;
}
//------------Aca estan las funciones implementadas----------
//###########################################################


//[05/02/22 23:16:27] id: 01581, reg: AXJ 3658, prod: G, ltrs: +0000080\n



int log_transactions(const char *data, char *log, size_t transaction_count){
	//
	if (!transaction_count) return 0;
	//Se van a guardar los timestamp y se le va a asignar dos indices
	//
	//   Indice BA | Indice AB | Timestamp |
	//
	//fwrite(data,1,80,stdout);
	unsigned char tmp[transaction_count][22];
	const char *data_tmp_ptr = data;
	
	for (size_t i=0;i<transaction_count;i++){
		//  | Vacio | Indice AB | Timestamp |

		//Se crea indice incremental
		tmp[i][1] = i;//i;
		//Se copia timestamp
		memcpy(&tmp[i][2],data_tmp_ptr,19);
		data_tmp_ptr+=34;

	};
	data_tmp_ptr = data;

	//Se ordena por fecha
	qsort (&tmp[0][0], transaction_count, sizeof(tmp[0])/sizeof(*tmp[0]), compareDate);
	
	//Se añade un segundo indice incremental
	
	for (size_t i=0;i<transaction_count;i++){
		//  | Indice BA | Indice AB | Timestamp |
		tmp[i][0] = i;//i;
		data_tmp_ptr+=34;
	};
	
	//Se ordena por indice AB
	qsort (&tmp[0][0], transaction_count, sizeof(tmp[0])/sizeof(*tmp[0]), compareSimple);
	data_tmp_ptr = data;
	
	//Como ahora esta ordenado por el primer indice el buffer de entrada se lee secuencialmente y se escribe en la posicion correspondiente del log
	for (size_t i=0;i<transaction_count;i++){
		
		int32_t mililiters;
		memcpy(&mililiters,&data[19+8+1], sizeof(int32_t));
		unsigned char tmp_id = tmp[i][0];
		
		uint16_t transaction_id;
		memcpy(&transaction_id,&data[19+8+1+4], sizeof(uint16_t));
		//Linea log
		sprintf(log+(72*tmp_id), "[%.19s] id: %0.4d, reg: %.8s, prod: %.1s, ltrs: %+0.7d\n",&tmp[i][2],transaction_id,&data[19],&data[19+8],mililiters);//data+19+8+4);
		data+=34;
	}
	
	return (int)72*transaction_count;
}
int compareSimple (const void* pa, const void* pb) {
	char* a = (char*)pa;
	char* b = (char*)pb;
	char tmpa = a[1];
	char tmpb = b[1];
	return ( tmpa - tmpb );
}
int compareDate(const void* pa, const void* pb) {
	char* a = (char*)pa+2;
	char* b = (char*)pb+2;
	//Devuelve 1 si a>b; -1 si b>a; 0 si a==b 
	
	//indices de busqueda del string de la fecha en orden inverso para comparar fechas, podria ser un macro
	const char order[] = {18,17,15,14,12,11,1,0,4,3,9,8};
	
	//se hace una iteracion comparando letra por letra
	for(unsigned char i=sizeof(order)/sizeof(order[0])-1;i!=0;i--){
		char c = b[order[i]] - a[order[i]] ;
		if(c==0)continue;
		if(c > 0) return -1;
		return 1;
	}
	return 0;
	
}

//###########################################################

int main(){
	//seed
	srand((unsigned int)time(NULL));
	char line[256];
	bool exitflag = false;
	printf("ENTER para añadir aleatoriamente un elemento a la lista\nL para obtener lista ordenada, X para obtener lista y salir\n\n");
	do	{
		fgets(line, sizeof line, stdin);
		switch(line[0]){
			case ENTER:
				//Crea una fecha aleatoria

				putData();
				break;
			case 'X':
			case 'x':
				exitflag=true;
			case 'L':
			case 'l':
				
				
				int size = log_transactions(data[0],a,n);
				printf("logger: \n");
				fwrite(&a,1,size,stdout);
			
				break;
		}
	}
	while(exitflag == false);
	return 0;
}
char *randomDatetime(){
	struct tm *date;
	
	date = RandomDatetimeStruct();
	strftime(inputbuffer,80,"%d/%m/%Y %I:%M:%S", date);
	return inputbuffer;
}
//'10/12/2022 15:08.256'


