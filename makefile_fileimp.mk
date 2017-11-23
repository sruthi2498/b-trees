a.out : main_file.o btree_imp_file.o 
	gcc -DT=2 main_file.o btree_imp_file.o  -w
main_file.o : main_file.c btree_file.h 
	gcc -DT=2 -c main_file.c btree_file.h -w
btree_imp_file.o : btree_imp_file.c btree_file.h
	gcc -DT=2 -c btree_imp_file.c btree_file.h -w