/* spyridoni eirini 2183
 * christodoulou dimitris 2113 */

#include<stdio.h>
#include <fcntl.h>
#include <errno.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#define BYTES_OF_MAGIC_NUMBER 5
#define BYTES_OF_FILE_NAME 255
#define MAX_BYTES 512

int my_open(char *name, int flags, int perms);
int my_read(int file, char string[], int size);
int my_write(int file, char string[], int size);
int my_lseek(int file , int pos, int whence);
void my_import(int file);
void my_search(int file);
void my_export(int file);
void my_delete(int file);
void delete_for_import(int file, char name[], int *flag_for_delete, int *counter);

int main(int argc, char *argv[]){
	
	int database, answer, counter, number=0;
	char database_name[BYTES_OF_FILE_NAME], magic_number[]="*****", string[BYTES_OF_MAGIC_NUMBER], *ptr, flag;
	
	//elexoume an dwthike onoma ths bashs sthn grammh entolwn
	if(argc==2){
		strcpy(database_name,argv[1]);
		ptr=strchr(database_name, '/');
		
		while(ptr!=NULL){
			ptr=ptr+1;
			strcpy(database_name,ptr);
			ptr=strchr(database_name, '/');
		}
	}
	else{
		printf("There is no file given\n");
		return 0;
	}
	
	database=open(database_name, O_RDWR |O_EXCL| O_CREAT, S_IRWXU);
	if (database!=-1){
		my_write(database, magic_number, BYTES_OF_MAGIC_NUMBER);
		counter=0;
		number=write(database, &counter, sizeof(int));
		if(number==-1){
			fprintf(stderr, "error in write with number at line %d, file_descriptor = %d\n", __LINE__, database);
			return (-1);
		}
	}
	else if (errno==EEXIST){
		printf("Database %s already exist\n", database_name);
		
		database=my_open(database_name, O_RDWR, 0);
		
		my_lseek(database,0,SEEK_SET);
		my_read(database, string, BYTES_OF_MAGIC_NUMBER);
		
		if(strcmp(string, magic_number)==0){
			printf("%s is database\n", database_name);
		}
		else{
			printf("%s is not database\n", database_name);
			return 0;
		}
	}
	else{
		printf("%s file can not open\n", database_name);
		return 0;
	}
	
	do {
		printf("Switch mode\n");
		printf("FOR IMPORT PRESS 1:\n");
		printf("FOR SEARCH PRESS 2:\n");
		printf("FOR EXPORT PRESS 3:\n");
		printf("FOR DELETE PRESS 4:\n");
		scanf("%d", &answer);
		switch(answer){
			case 1: {my_import(database); break;}
			case 2: {my_search(database); break;}
			case 3: {my_export(database); break;}
			case 4: {my_delete(database); break;}
			default: {printf("INVALID SELECTION\n");}
		}
		
		printf("Would you like to continue? 'y' or 'n': ");
		scanf(" %c", &flag);
		while ((flag!='y') && (flag!='n')){
			printf("Type again 'y' for continue or 'n' for stop");
			scanf(" %c", &flag);
		}
	} while(flag=='y');
	
	return 0;
	
}

int my_open(char *name, int flags, int perms) {
	
	int file;
	
	file = open(name, flags, perms);
	
	if (file == -1) {
		printf ("Error open file unexist.ent: %s\n",strerror(errno));
		return -1;
	}
	return (file);
	
}

int my_read(int file, char string[], int size){
	
	int number;
	
	number=read(file,string,size);
	
	if(number==-1){
		printf ("3 Error read  file unexist.ent %d: %s\n",file, strerror(errno));
		return -1;
	}
	else if (number==0) {
		printf("FILE END\n");
		return 0;
	}
	return(number);
	
}

int my_write(int file, char string[], int size){
	
	int num;
	num=write(file, string, size);
	
	if (num==-1) {
		printf ("4 Error write file unexist.ent %d: %s\n",file, strerror(errno));
		return -1;
	}
	
	return (num);
	
}

int my_lseek(int file , int pos, int whence){
	
	int num;
	
	num=lseek(file, pos, whence);
	
	if(num==-1) {
		printf ("Error lseek file unexist.ent: %s\n",strerror(errno));
		return -1;
	}
	
	
	return(num);
	
}


void my_import(int file ){
	
	struct stat buff;
	int bytes, number, counter, file_import, i=0, flag_for_delete=1;
	char file_name[BYTES_OF_FILE_NAME], string[MAX_BYTES];
	
	printf("GIVE THE NAME OF THE FILE\n");
	scanf("%255s", file_name);
	file_name[BYTES_OF_FILE_NAME]='\0';
	
	file_import=my_open(file_name, O_RDWR, S_IRWXU);
	
	if(fstat(file_import, &buff)<0){
		fprintf(stderr, "error in fstat with number at line %d, file_descriptor = %d\n", __LINE__, file);
		return;
	}
	else{
		bytes=buff.st_size;
		
		my_lseek(file, 5, SEEK_SET);
		number=read(file, &counter, sizeof(int));
		if(number==-1){
			fprintf(stderr, "error in read with number at line %d, file_descriptor = %d\n", __LINE__, file);
			return;
		}
		else if (number==0){
			printf("error in read with number at line %d, (FILE END)\n", __LINE__);
			return;
		}
		
		delete_for_import(file, file_name, &flag_for_delete, &counter);
		if(flag_for_delete==2){
			return;
		}
		
		counter= counter +1; //ypologizei kai apothikeuei sto arxeio twn arithmo twn arxeiwn pou exoume eisagei
		
		my_lseek(file, 5, SEEK_SET);
		number=write(file, &counter, sizeof(int));
		if(number==-1) {
			fprintf(stderr, "error in write with number at line %d, file_descriptor = %d\n", __LINE__, file);
			return;
		}
		
		
		my_lseek(file,0,SEEK_END);
		number=write(file, &bytes, sizeof(int));
		if (number==-1) {
			fprintf(stderr, "error in write with number at line %d, file_descriptor = %d\n", __LINE__, file);
			return;
		}
		
		my_lseek(file, 0, SEEK_END);
		number=my_write(file, file_name, BYTES_OF_FILE_NAME); //eisagei to onoma tou arxeiou
		my_lseek(file, 0, SEEK_END);
		my_lseek(file_import, 0 , SEEK_SET);
		while(i <bytes/MAX_BYTES){    //pernaei to periexomeno tou arxeiou ana 512 bytes
			my_read(file_import, string, MAX_BYTES);
			my_write(file, string, MAX_BYTES);
			i++;
		}
		if(bytes % MAX_BYTES != 0){
			my_read(file_import, string,bytes % MAX_BYTES );
			my_write(file, string,bytes % MAX_BYTES );
		}
	}
}

void my_search(int file){
	
	int counter, number, bytes, i=0, flag=0;
	char file_name[BYTES_OF_FILE_NAME+1], file_search[BYTES_OF_FILE_NAME+1];
	
	my_lseek(file, 5 , SEEK_SET);
	number=read(file, &counter, sizeof(int));
	if(number==-1){
		fprintf(stderr, "error in read with number at line %d, file_descriptor = %d\n", __LINE__, file);
		return;
	}
	else if (number==0){
		printf("error in read with number at line %d, (FILE END)\n", __LINE__);
		return;
	}
	
	printf("type the name of file you want to search: ");
	scanf(" %255s", file_search); 
	file_search[BYTES_OF_FILE_NAME]='\0';
	
	if (strcmp(file_search,"*")==0){
		while (i<counter){
			number=read(file, &bytes, sizeof(int));
			if(number==-1){
				fprintf(stderr, "error in read with number at line %d, file_descriptor = %d\n", __LINE__, file);
				return;
			}
			else if (number==0){
				printf("error in read with number at line %d, (FILE END)\n", __LINE__);
				return ;
			}
			
			my_read(file, file_name, BYTES_OF_FILE_NAME);
			file_search[BYTES_OF_FILE_NAME]='\0';
			printf("%s\n",file_name);
			my_lseek(file, bytes, SEEK_CUR);
			
			i=i+1;
			flag++;
		}
	}
	else {
		while (i<counter){
			number=read(file, &bytes, sizeof(int));
			if(number==-1){
				fprintf(stderr, "error in read with number at line %d, file_descriptor = %d\n", __LINE__, file);
				return;
			}
			else if (number==0){
				printf("error in read with number at line %d, (FILE END)\n", __LINE__);
				return;
			}
			
			my_read(file, file_name, BYTES_OF_FILE_NAME);
			file_name[BYTES_OF_FILE_NAME]='\0';
			if (strstr(file_name,file_search)){
				printf("%s\n",file_name);
				flag++;
			}
			my_lseek(file, bytes, SEEK_CUR);
			i=i+1;
		}
	}
	if(flag==0){
		printf("FILE DOESN'T EXIST IN DATABASE\n");
	}
}

void my_export(int file){
	
	char file_search[BYTES_OF_FILE_NAME], file_name[BYTES_OF_FILE_NAME], file_export[BYTES_OF_FILE_NAME], answer, string[MAX_BYTES];
	int fd, counter, i=0, bytes, flag=0, number, k=0;
	
	my_lseek(file, 5 , SEEK_SET);
	number=read(file, &counter, sizeof(int));
	if(number==-1){
		fprintf(stderr, "error in read with number at line %d, file_descriptor = %d\n", __LINE__, file);
		return;
	}
	else if (number==0){
		printf("error in read with number at line %d, (FILE END)\n", __LINE__);
		return;
	}
	
	printf("type the name of the file in database that you want to export\n");
	scanf("%255s", file_search);
	file_search[BYTES_OF_FILE_NAME]='\0';
	printf("type the export file name\n");
	scanf("%255s", file_export);
	file_export[BYTES_OF_FILE_NAME]='\0';
	
	fd=open(file_export, O_RDWR | O_EXCL |O_CREAT, S_IRWXU); //arxeio sto opoio tha ginei h exagwgh
	
	if(fd!=-1){ //to arxeio den uparxei kai dhmiourgeite
		while (i<counter){
			number=read(file, &bytes, sizeof(int));
			if(number==-1){
				fprintf(stderr, "error in read with number at line %d, file_descriptor = %d\n", __LINE__, file);
				return;
			}
			else if (number==0){
				printf("error in read with number at line %d, (FILE END)\n", __LINE__);
				return;
			}
			
			my_read(file, file_name, BYTES_OF_FILE_NAME);
			file_name[BYTES_OF_FILE_NAME]='\0';
			if (strcmp(file_name,file_search)==0){
				while(k <bytes/MAX_BYTES){    //pernaei to periexomeno tou arxeiou
					my_read(file, string, MAX_BYTES);
					my_write(fd, string, MAX_BYTES);
					k++;
					flag++;
				}
				if(bytes % MAX_BYTES != 0){
					my_read(file, string,bytes % MAX_BYTES );
					my_write(fd, string,bytes % MAX_BYTES );
					flag++;
				}
			}
			else {
				my_lseek(file, bytes, SEEK_CUR);
			}
			i=i+1;
		}
		if(flag==0){
			printf("FILE DOESN'T EXIST IN DATABASE\n");
		}
		
	}
	else if (errno==EEXIST){
		do{
			printf("Do you want to replace the contain? y or n\n");
			scanf(" %c", &answer);
		} while((answer=='y') || (answer=='n'));
		if (answer=='y') {
			fd=my_open(file_export, O_RDWR | O_TRUNC, 0);
			while(i<counter) {
				number=read(file, &bytes, sizeof(int));
				if(number==-1){
					fprintf(stderr, "error in read with number at line %d, file_descriptor = %d\n", __LINE__, file);
					return;
				}
				else if (number==0){
					printf("error in read with number at line %d, (FILE END)\n", __LINE__);
					return;
				}
				
				my_read(file, file_name, BYTES_OF_FILE_NAME);
				file_name[BYTES_OF_FILE_NAME]='\0';
				if(strcmp(file_name,file_search)==0){
					while(k <bytes/MAX_BYTES){    //pernaei to periexomeno tou arxeiou
						my_read(file, string, MAX_BYTES);
						my_write(fd, string, MAX_BYTES);
						k++;
					}
					if(bytes % MAX_BYTES != 0){
						my_read(file, string,bytes % MAX_BYTES );
						my_write(fd, string,bytes % MAX_BYTES );
					}
				}
				else {
					my_lseek(file, bytes, SEEK_CUR);
				}
				i++;
			}
		}
	}
	else{
		fprintf(stderr, "error in open at line %d, file_descriptor = %d\n", __LINE__, file);
		return;
	}
}

void my_delete(int file){
	
	struct stat buff;
	char file_name[BYTES_OF_FILE_NAME], file_delete[BYTES_OF_FILE_NAME], content_file[MAX_BYTES];
	int counter, total_bytes, start_bytes=9, i=1, number, point_start_copy=9, k=0, bytes;
	
	printf("GIVE THE NAME OF THE FILE FOR DELETE\n");
	scanf("%225s", file_delete);
	file_delete[BYTES_OF_FILE_NAME]='\0';
        
	if(fstat(file, &buff)<0){
		fprintf(stderr, "error in fstat with number at line %d, file_descriptor = %d\n", __LINE__, file);
		return;
	}
	else{
		total_bytes=buff.st_size; //sinolika_bytes tou arxeiou prin ginei diagrafh
		total_bytes=total_bytes-9;//sinolika_bytes xwris ta bytes tou magic number kai tou metriti
		
		my_lseek(file, BYTES_OF_MAGIC_NUMBER, SEEK_SET);
		number=read(file, &counter, sizeof(int));
		if(number==-1){
			fprintf(stderr, "error in read with number at line %d, file_descriptor = %d\n", __LINE__, file);
			return;
		}
		else if (number==0){
			printf("error in read with number at line %d, (FILE END)\n", __LINE__);
			return;
		}
		while(i<=counter){
			number=read(file, &bytes, sizeof(int));
			if(number==-1){
				fprintf(stderr, "error in read with number at line %d, file_descriptor = %d\n", __LINE__, file);
				return;
			}
			else if (number==0){
				printf("error in read with number at line %d, (FILE END)\n", __LINE__);
				return;
			}
			my_read(file, file_name, BYTES_OF_FILE_NAME);
			file_name[BYTES_OF_FILE_NAME]='\0';
			if(strcmp(file_name,file_delete)==0){
				if(i<counter) {//an einai diaforetiko apo to teleutaio arxeio
					point_start_copy=point_start_copy+sizeof(int)+BYTES_OF_FILE_NAME+bytes;
					total_bytes=total_bytes-sizeof(int)-BYTES_OF_FILE_NAME-bytes;
					while(k<total_bytes/MAX_BYTES){
						my_lseek(file , point_start_copy, SEEK_SET);
						my_read(file, content_file, MAX_BYTES);
						my_lseek(file, start_bytes, SEEK_SET);
						my_write(file, content_file, MAX_BYTES);
						start_bytes=start_bytes+MAX_BYTES;
						point_start_copy=point_start_copy+MAX_BYTES;
						k++;
					}
					if(total_bytes%MAX_BYTES!=0){
						my_lseek(file, point_start_copy, SEEK_SET);
						my_read(file, content_file, total_bytes%MAX_BYTES);
						my_lseek(file, start_bytes, SEEK_SET);
						my_write(file, content_file, total_bytes%MAX_BYTES);
						start_bytes=start_bytes+(total_bytes%MAX_BYTES);
						ftruncate(file , start_bytes);
					}
					counter--;
					my_lseek(file, 5, SEEK_SET);
					number=write(file, &counter, sizeof(int));
					if(number==-1){
						fprintf(stderr, "error in write with number at line %d, file_descriptor = %d\n", __LINE__, file);
						return;
					}
					return ;
				}
				else{
					ftruncate(file, start_bytes);
					counter--;
					my_lseek(file, 5, SEEK_SET);
					number=write(file, &counter, sizeof(int));
					if(number==-1){
						fprintf(stderr, "error in write with number at line %d, file_descriptor = %d\n", __LINE__, file);
						return;
					}
					return;
				}
			}
			else{
				point_start_copy=point_start_copy+sizeof(int)+BYTES_OF_FILE_NAME+bytes;
				start_bytes=start_bytes+sizeof(int)+BYTES_OF_FILE_NAME+bytes;
				total_bytes=total_bytes-sizeof(int)-BYTES_OF_FILE_NAME-bytes;
				my_lseek(file, bytes, SEEK_CUR);
			}
			i++;    
		}
		printf("file doesn't exist in database and not delete\n");
	}
}


/* voithitikh sunarthsh sthn periptwsh pou exoume epilexei na ginei import me antikatastash..
 * ara prepei na diagrafei to sugkekrimeno arxeio kai na antikatastathei apo to neo */

void delete_for_import(int file, char file_delete[], int *flag_for_delete, int *counter){
	 
	struct stat buff;
	char file_name[BYTES_OF_FILE_NAME], content_file[MAX_BYTES], answer;
	int total_bytes, start_bytes=9, i=1, number, point_start_copy=9, k=0, bytes;
	
	if(fstat(file, &buff)<0){
		fprintf(stderr, "error in fstat with number at line %d, file_descriptor = %d\n", __LINE__, file);
		return;
	}
	else{
		total_bytes=buff.st_size; //sinolika_bytes tou arxeiou prin ginei diagrafh
		total_bytes=total_bytes-9;//sinolika_bytes xwris ta bytes tou magic number kai tou metriti
		
		my_lseek(file, 9, SEEK_SET);
		while(i<=*counter){
			number=read(file, &bytes, sizeof(int));
			if(number==-1){
				fprintf(stderr, "error in read with number at line %d, file_descriptor = %d\n", __LINE__, file);
				return;
			}
			else if (number==0){
				printf("error in read with number at line %d, (FILE END)\n", __LINE__);
				return;
			}
			my_read(file, file_name, BYTES_OF_FILE_NAME);
			file_name[BYTES_OF_FILE_NAME]='\0';
			if(strcmp(file_name,file_delete)==0){
				do{
					printf("do you want to replace the file? y or n\n");
					scanf(" %c", &answer);
				} while(answer=='y' || answer=='n');
				if(answer=='n'){
					*flag_for_delete=2;
					return;
				}
				else {
					if(i<*counter) {
						point_start_copy=point_start_copy+sizeof(int)+BYTES_OF_FILE_NAME+bytes;
						total_bytes=total_bytes-sizeof(int)-BYTES_OF_FILE_NAME-bytes;
						while(k<total_bytes/MAX_BYTES){
							my_lseek(file , point_start_copy, SEEK_SET);
							my_read(file, content_file, MAX_BYTES);
							my_lseek(file, start_bytes, SEEK_SET);
							my_write(file, content_file, MAX_BYTES);
							start_bytes=start_bytes+MAX_BYTES;
							point_start_copy=point_start_copy+MAX_BYTES;
							k++;
						}
						if(total_bytes%MAX_BYTES!=0){
							my_lseek(file, point_start_copy, SEEK_SET);
							my_read(file, content_file, total_bytes%MAX_BYTES);
							my_lseek(file, start_bytes, SEEK_SET);
							my_write(file, content_file, total_bytes%MAX_BYTES);
							start_bytes=start_bytes+(total_bytes%MAX_BYTES);
							ftruncate(file , start_bytes);
						}
						*counter= *counter - 1;
						return ;
					}
					else{
						*flag_for_delete=1;
						ftruncate(file, start_bytes);
						*counter= *counter - 1;
						return;
					}
				}
			}
			else{
				point_start_copy=point_start_copy+sizeof(int)+BYTES_OF_FILE_NAME+bytes;
				start_bytes=start_bytes+sizeof(int)+BYTES_OF_FILE_NAME+bytes;
				total_bytes=total_bytes-sizeof(int)-BYTES_OF_FILE_NAME-bytes;
				my_lseek(file, bytes, SEEK_CUR);
			}
			i++; 
		}
	}
}