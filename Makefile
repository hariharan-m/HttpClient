all:	SimpleClient.c
	gcc -o output SimpleClient.c
google: output
	./output google.com 80
clean:	
	\rm *.o output