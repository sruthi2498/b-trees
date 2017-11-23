#include "btree_file.h"

int main(){
	struct Btree_Info Btree=Initialize_With_Degree(2,"mytree.dat");
	Display_Btree_Info(Btree);
	Btree=BTree_Create(Btree);

	FILE *fp;
	fp=fopen("dataset.csv","r");
	
 	struct record R;
 	int i=1;
 	char * temp=malloc(sizeof(char)*10);
 	struct timespec start;
	struct timespec end;
	
	
   while (!feof(fp)){
	   	fscanf(fp, "%d,%3s,%4s,", &R.index,&R.country,&R.status);
	   	R.country[3]='\0';
	   	R.status[4]='\0';
	   	fscanf(fp,"%d,%d\n",&R.num1,&R.num2);
		printf("\ni %d index %d count %s stat %s num1 %d num2 %d",i,R.index, R.country,R.status,R.num1,R.num2);
	 	clock_gettime(CLOCK_REALTIME, &start);
	 	Btree=Btree_Insert(Btree,R);
		clock_gettime(CLOCK_REALTIME, &end);
		printf("\nInserted %d records",i);
	//display(arr,n);
		//WriteToFile_Time(i,time_elapsed(&start, &end));
	 	i++;
    }	

	fclose(fp);

	
	Btree_Search(Btree,782891);
	Btree=Delete_Btree(Btree);
}