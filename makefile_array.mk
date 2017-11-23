a.out : main.o btree_imp.o 
	gcc main.o btree_imp.o  -w
main.o : main.c btree.h 
	gcc -c main.c btree.h -w
btree_imp.o : btree_imp.c btree.h
	gcc -c btree_imp.c btree.h -w