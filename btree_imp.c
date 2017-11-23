#include "btree.h"

struct Btree_Info Initialize_With_Degree(int degree){
	int max_keys=(2*degree)-1;
	int min_keys=degree-1;

	//struct node * array=malloc(sizeof(struct node)*500);
	//each node can have max_keys number of keys. array has 500 such nodes
	struct node *ListOfNodes=malloc(sizeof(struct node)*MAX);
	int j=0;
	struct record ** array=malloc(sizeof(struct record *)*MAX);
	for(int i=0;i<500;i++){
		array[i]=malloc(sizeof(struct record)*max_keys);
		struct node Node;
		Node.keys=array[i];
		ListOfNodes[j]=Node;
		j++;
	}
	//struct record array[500][max_keys];
	struct Btree_Info Btree;
	Btree.ListOfNodes=ListOfNodes;
	Btree.FreeNodes=array;
	Btree.max_keys=max_keys;
	Btree.min_keys=min_keys;
	Btree.current_free_node=0;
	Btree.degree=degree;
	Btree.max_free_nodes=MAX;
	Btree.diskread=0;
	Btree.diskwrite=0;
	Btree.number_of_records=0;

	return Btree;

}



struct NodePlusTree Allocate_Node(struct Btree_Info Btree){
	struct NodePlusTree temp;
	//printf("\nin allocate node current free node : %d",Btree.current_free_node);
	struct node Node;
	Node.start=-1;
	temp.Btree=Btree;
	temp.Node=Node;
	if(Btree.current_free_node>Btree.max_free_nodes){
		//printf("No more free nodes");
		return temp;
	}
	Node.start=Btree.current_free_node;
	Node.n=0;
	Node.c=0;
	Node.keys=malloc(sizeof(struct record)* (2*Btree.degree)-1) ;
	Node.children=malloc(sizeof(int)* (2*Btree.degree));
	Node.keys=Btree.FreeNodes[Btree.current_free_node];
	Btree.ListOfNodes[Btree.current_free_node]=Node;
	Btree.current_free_node=Btree.current_free_node+1;
	//printf("\ncurrent free node : %d",Btree.current_free_node);
	Btree.diskwrite++;
	temp.Btree=Btree;
	temp.Node=Node;
	return temp;

}

void Btree_Search(struct Btree_Info Btree, int s){
	struct timespec start;
	struct timespec end;
	
	clock_gettime(CLOCK_REALTIME, &start);
	struct record k;
	k.index=s;
	struct searchresult result=Search(Btree,Btree.Root,k);
	clock_gettime(CLOCK_REALTIME, &end);
	printf("\n_______________________________\n\ntime:%lf ms\n",time_elapsed(&start, &end));
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
	while(i<root.n && k.index>root.keys[i].index)i++;
	if(i<=root.n && k.index==root.keys[i].index){
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

		struct node childnode=Btree.ListOfNodes[root.children[i]];
		return Search(Btree,childnode,k);
	}
}

struct NodePlusTree Btree_Split_Child(struct Btree_Info Btree, struct node x, int i){
	Btree.ListOfNodes[Btree.Root.start]=Btree.Root;
	struct NodePlusTree temp=Allocate_Node(Btree);
	Btree=temp.Btree;
	struct node z=temp.Node;
	struct NodePlusTree temp2;
	temp2.Btree=Btree;
	temp2.Node=x;
	if(z.start==-1){
		//printf("\ncould not allocate node");
		return temp2;
	}
	Btree.ListOfNodes[x.start]=x;
	//printf("\nx:");
	Display_Node(x,Btree);
	//printf("\nx's ith child %d",x.children[i]);
	Display_Node(Btree.ListOfNodes[x.children[i]],Btree);
	struct node y=Btree.ListOfNodes[x.children[i]];  //y is child of x at i
	//printf("\ny:");
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
	//printf("\nsplit: z");
	Display_Node(z,Btree);
	y.n=t-1;
	//printf("\nsplit :y");
	Display_Node(y,Btree);
	for(int j=x.n;j>i;j--){
		x.children[j+1]=x.children[j];
	}
	x.children[i+1]=z.start;
	x.c++;
	//printf("\nx.children[i+1] %d",x.children[i+1]);
	for(int j=x.n ;j>i; j--){
		//printf("\nj=%d x.keys[j]=%d x.keys[j+1]=%d",j,x.keys[j].index,x.keys[j+1].index);
		x.keys[j+1]=x.keys[j];
		//printf("\nj=%d x.keys[j]=%d x.keys[j+1]=%d",j,x.keys[j].index,x.keys[j+1].index);
		
	}
	//printf("\ny.keys[t] %d y.n %d",y.keys[t].index,y.n);
	x.keys[i]=y.keys[t-1];
	x.n++;
//	printf("\nparent : x");
	Display_Node(x,Btree);
	Btree.ListOfNodes[z.start]=z;
	Btree.diskwrite++;
	Btree.ListOfNodes[y.start]=y;
	Btree.diskwrite++;
	Btree.ListOfNodes[x.start]=x;
	Btree.diskwrite++;
	temp2.Btree=Btree;
	temp2.Node=x;
	return temp2;
}

struct Btree_Info Btree_Insert(struct Btree_Info Btree, struct record k){
	Btree.number_of_records++;
	Btree.ListOfNodes[Btree.Root.start]=Btree.Root;
	//printf("\nin insert k=%d",k.index);
	struct node root=Btree.Root;
	int startval=root.start;
	//printf("\nstart val %d",startval);
	//Display_Node(root,Btree);
	int t=Btree.degree;
	if(root.n==2*t-1){
		//printf("\n\troot full");
		struct NodePlusTree temp=Allocate_Node(Btree);
		Btree=temp.Btree;
		struct node s=temp.Node;;
		Display_Node(s,Btree);
		if(s.start==-1){
			//printf("\ncould not allocate node");
			return ;
		}
		struct node oldroot=root;
		printf("\nnode allocated");
		printf("\nroot");
		Display_Node(Btree.Root,Btree);
		Btree.ListOfNodes[Btree.Root.start]=Btree.Root;
		//printf("\nroot node");
		Display_Node(Btree.ListOfNodes[Btree.Root.start],Btree);
		Btree.Root=s;
		s.leaf=0;
		s.n=0;
		s.c++;
		s.children[0]=startval;
		Display_Node(s,Btree);
		//printf("\ns.children[0] %d",s.children[0]);
		//printf("\ns's first child");
		Display_Node(Btree.ListOfNodes[s.children[0]],Btree);
		
		//Btree.ListOfNodes[s.start]=s;
		//printf("\nbefore split s");

		Display_Node(s,Btree);
		//s.children[0]=startval;
		//printf("\ns's first child");
		Display_Node(Btree.ListOfNodes[s.children[0]],Btree);
		struct NodePlusTree temp2=Btree_Split_Child(Btree,s,0);
		s=temp2.Node;
		Btree=temp2.Btree;
		Btree.Root=s;
		Btree.ListOfNodes[s.start]=s;
		//printf("\ns after split");
		Display_Node(s,Btree);
		Btree=Btree_Insert_NonFull(Btree,s,k);


	}
	else {
		//printf("\nin else");
		Btree=Btree_Insert_NonFull(Btree,root,k);
		//printf("\nback from nonfull");
		Display_Node(Btree.ListOfNodes[root.start],Btree);
		Btree.Root=Btree.ListOfNodes[root.start];
	}
	//printf("\ninsert over");
	//WriteToFile_Disk(Btree.number_of_records,Btree.diskread,Btree.diskwrite);
	return Btree;
}

struct Btree_Info Btree_Insert_NonFull(struct Btree_Info Btree, struct node x, struct record k){

	//printf("\nin insert nonfull x:");
	Display_Node(x,Btree);
	int i=x.n-1;
	if(x.leaf){
		while(i>=0 && k.index<x.keys[i].index){
			//printf("\ni=%d x-keys %d k %d",i,x.keys[i].index,k.index);
			x.keys[i+1]=x.keys[i];
			i--;
		}

		x.keys[i+1]=k;
		//printf("\nxkeys %d",x.keys[i].index);
		x.n++;
		Btree.diskwrite++;
		Btree.ListOfNodes[x.start]=x;
	
	}
	else{
		while(i>=0 && k.index<=x.keys[i].index){
			//printf("\ni=%d x-keys %d k %d",i,x.keys[i].index,k.index);
			i--;
		}
		i++;
		Btree.diskread++;
		if(Btree.ListOfNodes[x.children[i]].n==2*Btree.degree-1){
			//printf("\nith child is full");
			struct NodePlusTree temp=Btree_Split_Child(Btree,x,i);
			x=temp.Node;
			Btree=temp.Btree;
			if(k.index>x.keys[i].index)i++;

		}
		Btree=Btree_Insert_NonFull(Btree,Btree.ListOfNodes[x.children[i]],k);
	}
	//printf("\ninsert nonfull over");
	Display_Node(x,Btree);
	Btree.ListOfNodes[x.start]=x;
	//printf("\nindex of node x:%d",x.start);
	//printf("\nfor x.start");
	Display_Node(Btree.ListOfNodes[x.start],Btree);
	return Btree;
}

struct Btree_Info BTree_Create(struct Btree_Info Btree){
	//printf("\nin Btree create");
	struct NodePlusTree temp=Allocate_Node(Btree);
	Btree=temp.Btree;
	struct node Node=temp.Node;
	if(Node.start==-1){
		//printf("\ncould not allocate node");
		return ;
	}
	Node.n=0;
	Node.children=NULL;
	Node.leaf=1;
	Btree.Root=Node;
	Node.c=0;
	//Display_Btree_Info(Btree);
	Btree.ListOfNodes[Node.start]=Node;
	Btree.ListOfNodes[Btree.Root.start]=Btree.Root;
	// Display_Node(Btree.Root,Btree);
	// Display_Node(Btree.ListOfNodes[Btree.Root.start],Btree);
	return Btree;
}
void Display_Btree_Info(struct Btree_Info Btree){
	//printf("\nmax free nodes : %d\ncurrent free node :%d\ndegree of tree : %d\nmax keys per node : %d\nmix keys per node : %d\n",
	//	Btree.max_free_nodes,Btree.current_free_node,Btree.degree,Btree.max_keys,Btree.min_keys);
}

void Display_Node(struct node Node,struct Btree_Info Btree){
	/*printf("\n######################");
	printf("\ncurrent keys : %d\ncurrent children : %d\nstart from free nodes : %d\nis leaf : %d\n",Node.n,Node.c,Node.start,Node.leaf);
	printf("keys :");
	for(int i=0;i<Node.n;i++)printf("%d ",Node.keys[i].index);
	printf("\nchildren indices :");
	for(int i=0;i<Node.c;i++)printf("%d ",Node.children[i]);
		printf("\n");
	
	printf("\n######################");*/
}
void Display_Record(struct record R){
	//printf("\nRECORD :\n");
	//printf("Index : %d\nCountry : %s\nStatus : %s\nnum1 : %d\nnum2 : %d\n",R.index,R.country,R.status,R.num1,R.num2);
}

struct Btree_Info Delete_Btree(struct Btree_Info Btree){
	free(Btree.ListOfNodes);
	free(Btree.FreeNodes);
	return(Btree);
}
void WriteToFile_Disk(int insertnum, int dr, int dw){
	FILE *fptr;
	fptr=fopen("DiskRead5.csv","a");
	fprintf(fptr,"%d",insertnum);
	fprintf(fptr,",");
	fprintf(fptr,"%d",dr);
	fprintf(fptr,",");
	fprintf(fptr,"%d\n",dw);

	fclose(fptr);
}

void WriteToFile_Time(int insertnum, double t){
	FILE *fptr;
	fptr=fopen("Time5.csv","a");

	fprintf(fptr,"%d",insertnum);
	fprintf(fptr,",");
	fprintf(fptr,"%lf\n",t);

	fclose(fptr);
}
double time_elapsed(struct timespec *start, struct timespec *end) {
	double t = 0.0;
	t = (end->tv_sec - start->tv_sec) * 1000;
	t += (end->tv_nsec - start->tv_nsec) * 0.000001;
	return t;
}