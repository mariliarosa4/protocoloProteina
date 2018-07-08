#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
char gerarAminoacido(){
char aminoacidos[61]= "ARRNNNNDCQEGGGGHHHHIIIIIIIILKMMMMMMMMFPPPPSTWWWWWWWWYYYYYYYYV";
srand(time(NULL));
int num = rand()%61;
printf("%d", num);
printf("%c", aminoacidos[num]);
return aminoacidos[num];
} 


int main(){
printf("%c", gerarAminoacido());

} 
