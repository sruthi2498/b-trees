#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define MAX 10000000

typedef struct node node;
typedef struct Btree_Info Btree_Info;

struct record{
	long long int index;
	char country[5];
	char status[6];
	int num1;
	int num2;
};
struct node{
		struct record * keys;
		int n; //current keys
		int leaf;
		int *children;
		int start; //start index in original free nodes array
		int c; //children count

};
struct Btree_Info{
		struct record ** FreeNodes;
		struct node *ListOfNodes; //List of nodes from which a node is allocated
		int max_keys;
		int min_keys;
		int current_free_node; //can be allocated
		int degree;
		int max_free_nodes;
		struct node Root;
		int number_of_records;
		int diskread;
		int diskwrite;
};


struct NodePlusTree{
	struct Btree_Info Btree;
	struct node Node;
};

struct searchresult{
	struct record ans;
	int flag; //found=1
	int recordindex; //index of record
};


struct Btree_Info Initialize_With_Degree(int degree);

void Display_Btree_Info(struct Btree_Info Btree);
void Display_Node(struct node Node,struct Btree_Info Btree);
void Display_Record(struct record R);

struct NodePlusTree Allocate_Node(struct Btree_Info Btree);

struct Btree_Info BTree_Create(struct Btree_Info Btree);

void Btree_Search(struct Btree_Info Btree, int s);
struct searchresult Search(struct Btree_Info Btree,struct node root, struct record k);

struct NodePlusTree Btree_Split_Child(struct Btree_Info Btree, struct node x, int i);
//internal node x, child index i
// split child into 2 and adjust x

struct Btree_Info Btree_Insert(struct Btree_Info Btree, struct record k);
struct Btree_Info Btree_Insert_NonFull(struct Btree_Info Btree, struct node x, struct record k);

struct Btree_Info Delete_Btree(struct Btree_Info Btree);


void WriteToFile_Disk(int insertnum, int dr, int dw);
void WriteToFile_Time(int insertnum, double t);
double time_elapsed(struct timespec *start, struct timespec *end);