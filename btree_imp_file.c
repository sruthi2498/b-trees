#include "btree_file.h"

struct Btree_Info Initialize_With_Degree(int degree,char * filename){
	int max_keys=(2*degree)-1;
	int min_keys=degree-1;

	
	struct Btree_Info Btree;
	Btree.fp=fopen(filename,"w");
	fclose(Btree.fp);
	Btree.fp=fopen(filename,"w+");
	Btree.next_pos=0;
	//Btree.Root=0;
	Btree.max_keys=max_keys;
	Btree.min_keys=min_keys;
	Btree.degree=degree;
	Btree.max_free_nodes=500;
	Btree.diskread=0;
	Btree.diskwrite=0;
	Btree.number_of_records=0;

	return Btree;

}

struct Btree_Info BTree_Create(struct Btree_Info Btree){
	printf("\nin Btree create");
	struct NodePlusTree temp=Allocate_Node(Btree);
	Btree=temp.Btree;
	struct node Node=temp.Node;
	
	Node.n=0;
	//Node.children=NULL;
	Node.leaf=1;
	Btree.Root=Node;
	Node.c=0;
	Btree=write_file(Btree,&Node,Node.pos);
	// struct node * temp2=malloc(sizeof(struct node)*1);
	// struct node temp3=read_file(Btree,&temp2,Node.pos);
	// Display_Node(temp3,Btree);
	//free(temp2);
	//Display_Btree_Info(Btree);
	Btree=write_file(Btree,&Btree.Root,Btree.Root.pos);

	//Btree.ListO	fNodes[Btree.Root.pos]=Btree.Root;

	// Display_Node(Btree.Root,Btree);
	// Display_Node(Btree.ListOfNodes[Btree.Root.pos],Btree);*/
	//free(temp2);
	return Btree;
}

struct NodePlusTree Allocate_Node(struct Btree_Info Btree){
	struct NodePlusTree temp;
	struct node * temp2;
	printf("\nin allocate node next pos : %d",Btree.next_pos);
	struct node Node;
	Node.pos=-1;
	temp.Btree=Btree;
	temp.Node=Node;
	printf("\nBtree.next_pos=%d",Btree.next_pos);
	Node.pos=Btree.next_pos;

	//printf("\nNode.pos %d",Node.pos);
	Node.n=0;
	Node.c=0;
	//Node.keys=malloc(sizeof(struct record)* (2*Btree.degree)-1) ;
	//Node.children=malloc(sizeof(int)* (2*Btree.degree));
	//Display_Node(Node,Btree);
	Btree=write_file(Btree,&Node,-1);
	printf("\nBtree.next_pos=%d",Btree.next_pos);
	temp2=malloc(sizeof(struct node)*1);
	struct node temp3=read_file(Btree,&temp2,Node.pos);
	Display_Node(temp3,Btree);
	//free(temp2);
	Btree.diskwrite++;
	temp.Btree=Btree;
	temp.Node=Node;
	//free(temp2);
	return temp;

}

void Btree_Search(struct Btree_Info Btree, int s){
	struct timespec pos;
	struct timespec end;
	
	clock_gettime(CLOCK_REALTIME, &pos);
	struct record k;
	k.index=s;
	struct searchresult result=Search(Btree,Btree.Root,k);
	clock_gettime(CLOCK_REALTIME, &end);
	printf("\n_______________________________\n\ntime:%lf ms\n",time_elapsed(&pos, &end));
	if(result.flag==0){
		printf("\nnot found");
		return;
	}

	printf("\nSearching for %d\n\nSEARCH RESULTS :",s);
	Display_Record(result.ans);
	printf("\n_______________________________\n");
}

struct searchresult Search(struct Btree_Info Btree,struct node root, struct record k){
	struct searchresult result;
	int i=0;
	printf("\nsearching at");
	Display_Node(root,Btree);
	while(i<root.n && k.index>root.keys[i].index)i++;
	printf("\ni=%d k.index %d root.keys[i].index %d %d",i,k.index,root.keys[i].index,k.index==root.keys[i].index);
	if(i<=root.n && k.index==root.keys[i].index){
		printf("\nfound");
		result.flag=1;
		result.recordindex=i;
		result.ans=root.keys[i];
		return result;
	}
	else if(root.leaf){
		result.flag=0;
		return result;
	}
	else {
		struct node *temp=malloc(sizeof(node)*1);
		int childindex=Btree.Root.children[i];
		struct node childnode=read_file(Btree,&temp,childindex);
		return Search(Btree,childnode,k);
	}
}

struct NodePlusTree Btree_Split_Child(struct Btree_Info Btree, struct node x, int i){
	Btree=write_file(Btree,&Btree.Root,Btree.Root.pos);
	struct node * tempR;
	//Btree.ListOfNodes[Btree.Root.pos]=Btree.Root;
	struct NodePlusTree temp=Allocate_Node(Btree);
	Btree=temp.Btree;
	struct node z=temp.Node;
	struct NodePlusTree temp2;
	temp2.Btree=Btree;
	temp2.Node=x;
	if(z.pos==-1){
		printf("\ncould not allocate node");
		return temp2;
	}
	Btree=write_file(Btree,&x,x.pos);
	//Btree.ListOfNodes[x.pos]=x;
	printf("\nx:");
	Display_Node(x,Btree);
	printf("\nx's ith child %d",x.children[i]);
	int child_index=x.children[i];
	tempR=malloc(sizeof(struct node)*1);
	struct node y=read_file(Btree,&tempR,child_index);
	//free(tempR); //y is child of x at i
	printf("\ny:");
	Display_Node(y,Btree);
	z.leaf=y.leaf;
	int t=Btree.degree;
	z.n=t-1;
	for(int j=0;j<t-1;j++){
		z.keys[j]=y.keys[j+t];
	}
	if(y.leaf!=1){
		for(int j=0;j<t;j++){
			z.children[j]=y.children[j+t];
			z.c++;
		}
	}
	printf("\nsplit: z");
	Display_Node(z,Btree);
	y.n=t-1;
	printf("\nsplit :y");
	Display_Node(y,Btree);
	for(int j=x.n;j>i;j--){
		x.children[j+1]=x.children[j];
	}
	x.children[i+1]=z.pos;
	x.c++;
	printf("\nx.children[i+1] %d",x.children[i+1]);
	for(int j=x.n ;j>i; j--){
		printf("\nj=%d x.keys[j]=%d x.keys[j+1]=%d",j,x.keys[j].index,x.keys[j+1].index);
		x.keys[j+1]=x.keys[j];
		printf("\nj=%d x.keys[j]=%d x.keys[j+1]=%d",j,x.keys[j].index,x.keys[j+1].index);
		
	}
	printf("\ny.keys[t] %d y.n %d",y.keys[t].index,y.n);
	x.keys[i]=y.keys[t-1];
	x.n++;
	printf("\nparent : x");
	Display_Node(x,Btree);
	//Btree.ListOfNodes[z.pos]=z;
	Btree=write_file(Btree,&z,z.pos);
	Btree.diskwrite++;
	//Btree.ListOfNodes[y.pos]=y;
	Btree=write_file(Btree,&y,y.pos);
	Btree.diskwrite++;
	//Btree.ListOfNodes[x.pos]=x;
	Btree=write_file(Btree,&x,x.pos);
	Btree.diskwrite++;
	Btree.Root=read_file(Btree,&tempR,Btree.Root.pos);
	temp2.Btree=Btree;
	temp2.Node=x;
	//free(tempR);
	return temp2;
}

struct Btree_Info Btree_Insert(struct Btree_Info Btree, struct record k){
	Btree.number_of_records++;
	Btree=write_file(Btree,&Btree.Root,Btree.Root.pos);
	struct node * tempR;
	//Btree.ListOfNodes[Btree.Root.pos]=Btree.Root;
	printf("\nin insert k=%d",k.index);
		//free(tempR);
	int posval=Btree.Root.pos;
	printf("\npos val %d",posval);
	Display_Node(Btree.Root,Btree);
	int t=Btree.degree;
	printf("\nBtree.Root.n=%d",Btree.Root.n);
	if(Btree.Root.n==2*t-1){
		printf("\n\troot full");
		struct NodePlusTree temp=Allocate_Node(Btree);
		Btree=temp.Btree;
		struct node s=temp.Node;;
		Display_Node(s,Btree);
		if(s.pos==-1){
			printf("\ncould not allocate node");
			return ;
		}
		printf("\nnode allocated");
		printf("\nroot");
		Display_Node(Btree.Root,Btree);
		Btree=write_file(Btree,&Btree.Root,Btree.Root.pos);
		printf("\nroot node");

		Display_Node(Btree.Root,Btree);
		Btree.Root=s;
		s.leaf=0;
		s.n=0;
		s.c++;
		s.children[0]=posval;
		printf("\ns");
		Display_Node(s,Btree);

		// printf("\ns.children[0] %d",s.children[0]);
		// int first_child_index=s.children[0];
		// struct node * tempFC=malloc(sizeof(struct node)*1);
		// struct node firstchild=read_file(Btree,&tempFC,first_child_index);
		// //free(tempR); 
		// printf("\ns's first child");
		// Display_Node(firstchild,Btree);
		
		// //Btree.ListOfNodes[s.pos]=s;
		printf("\nbefore split s");

		Display_Node(s,Btree);
		// //s.children[0]=posval;
		
		struct NodePlusTree temp2=Btree_Split_Child(Btree,s,0);
		s=temp2.Node;
		Btree=temp2.Btree;
		Btree.Root=s;
		Btree=write_file(Btree,&Btree.Root,Btree.Root.pos);

		printf("\ns after split");
		Display_Node(s,Btree);
		printf("\nroot node");
		Display_Node(Btree.Root,Btree);
		printf("\ncalling insert nonfull");
		Btree=Btree_Insert_NonFull(Btree,s,k);


	}
	else {
		printf("\nin else");
		Btree=Btree_Insert_NonFull(Btree ,Btree.Root,k);
		printf("\nback from nonfull");
		tempR=malloc(sizeof(struct node)*1);
		struct node temp3=read_file(Btree,&tempR,Btree.Root.pos);
		//free(tempR);
		Display_Node(temp3,Btree);
		Btree.Root=temp3;
		printf("\nroot");
		Display_Node(Btree.Root,Btree);
		Btree=write_file(Btree,&Btree.Root,Btree.Root.pos);
		tempR=malloc(sizeof(struct node)*1);
		Btree.Root=read_file(Btree,&tempR,Btree.Root.pos);
		printf("\nroot");
		Display_Node(Btree.Root,Btree);

	}
	printf("\ninsert over");
	//WriteToFile_Disk(Btree.number_of_records,Btree.diskread,Btree.diskwrite);
	//free(tempR);
	return Btree;
}

struct Btree_Info Btree_Insert_NonFull(struct Btree_Info Btree, struct node x, struct record k){
	struct node * tempR;
	printf("\nin insert nonfull x:");
	Display_Node(x,Btree);
	int i=x.n-1;
	printf("\ni=%d",i);
	if(x.leaf==1){
		printf("\nx is leaf");
		while(i>=0 && k.index<x.keys[i].index){
			printf("\ni=%d x-keys %d k %d",i,x.keys[i].index,k.index);
			x.keys[i+1]=x.keys[i];
			i--;
		}
		//for(int m=0;m<(2*Btree.degree)-1;m++)printf("%d ",x.keys[m].index);
		x.keys[i+1]=k;
		printf("\nxkeys %d",x.keys[i].index);
		x.n++;
		Btree.diskwrite++;
		Btree=write_file(Btree,&x,x.pos);
	
	}
	else if(x.leaf==0){
		printf("\nin else of insertnonfull");
		while(i>=0 && k.index<=x.keys[i].index){
			printf("\ni=%d x-keys %d k %d",i,x.keys[i].index,k.index);
			i--;
		}
		i++;
		printf("\ni=%d nextpos %d",i,Btree.next_pos);
		int child_index=x.children[i];
		struct node * tempR1=malloc(sizeof(struct node)*1);
		struct node i_th_child=read_file(Btree,&tempR1,child_index);

		printf("\ni'th child");
		Display_Node(i_th_child,Btree);

		
		Btree.diskread++;
		if(i_th_child.n==2*Btree.degree-1){
			printf("\nith child is full");
			struct NodePlusTree temp=Btree_Split_Child(Btree,x,i);
			x=temp.Node;
			Btree=temp.Btree;
			if(k.index>x.keys[i].index)i++;

		}
		Btree=Btree_Insert_NonFull(Btree,i_th_child,k);
	}
	printf("\ninsert nonfull over");
	Display_Node(x,Btree);
	Btree=write_file(Btree,&x,x.pos);
	//Btree.ListOfNodes[x.pos]=x;
	//printf("\nindex of node x:%d",x.pos);
	Btree.Root=read_file(Btree,&Btree.Root,Btree.Root.pos);
	printf("\nfor x.pos");
	tempR=malloc(sizeof(struct node)*1);
	struct node x_pos=read_file(Btree,&tempR,x.pos);
	//free(tempR);
	Display_Node(x_pos,Btree);
	return Btree;
}

void Display_Btree_Info(struct Btree_Info Btree){
	printf("\nmax free nodes : %d\nnext pos :%d\ndegree of tree : %d\nmax keys per node : %d\nmix keys per node : %d\n",
		Btree.max_free_nodes,Btree.next_pos,Btree.degree,Btree.max_keys,Btree.min_keys);
}

void Display_Node(struct node Node,struct Btree_Info Btree){
	printf("\n######################");
	printf("\ncurrent keys : %d\ncurrent children : %d\npos : %d\nis leaf : %d\n",Node.n,Node.c,Node.pos,Node.leaf);
	printf("keys :");
	for(int i=0;i<Node.n;i++)printf("%d ",Node.keys[i].index);
	printf("\nchildren indices :");
	for(int i=0;i<Node.c;i++)printf("%d ",Node.children[i]);
		printf("\n");
	
	printf("\n######################");
}
void Display_Record(struct record R){
	printf("\nRECORD :\n");
	printf("Index : %d\nCountry : %s\nStatus : %s\nnum1 : %d\nnum2 : %d\n",R.index,R.country,R.status,R.num1,R.num2);
}

struct Btree_Info Delete_Btree(struct Btree_Info Btree){
	fclose(Btree.fp);
	return(Btree);
}
void WriteToFile_Disk(int insertnum, int dr, int dw){
	FILE *fptr;
	fptr=fopen("DiskRead2.csv","a");
	fprintf(fptr,"%d",insertnum);
	fprintf(fptr,",");
	fprintf(fptr,"%d",dr);
	fprintf(fptr,",");
	fprintf(fptr,"%d\n",dw);

	fclose(fptr);
}

double time_elapsed(struct timespec *pos, struct timespec *end) {
	double t = 0.0;
	t = (end->tv_sec - pos->tv_sec) * 1000;
	t += (end->tv_nsec - pos->tv_nsec) * 0.000001;
	return t;
}

struct Btree_Info write_file(struct Btree_Info Btree, struct node * p, int pos) // pos = -1; use next_pos
{

	if(pos == -1)
	{
		pos = Btree.next_pos++;
	}
	printf("\nwriting to %d",pos);
	fseek(Btree.fp, pos * sizeof(struct node), 0);
	fwrite(p, sizeof(struct node), 1, Btree.fp);
	return Btree;
}

struct node read_file(struct Btree_Info Btree, struct node * p, int pos)
{
	printf("\nreading from %d",pos);
	fseek(Btree.fp, pos * sizeof(struct node), 0);
	fread(p, sizeof(struct node), 1, Btree.fp);
	//Display_Node(*p,Btree);
	struct node temp=*p;
	return temp;
}
void WriteToFile_Time(int insertnum, double t){
	FILE *fptr;
	fptr=fopen("Time2.csv","a");

	fprintf(fptr,"%d",insertnum);
	fprintf(fptr,",");
	fprintf(fptr,"%lf\n",t);

	fclose(fptr);
}