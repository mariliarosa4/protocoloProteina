#include<stdio.h>
#include<string.h>
#include<stdlib.h>
// um cliente p cada linha de servidor
// armazeno os aminoacidos numa fila e outra thread vai tirando dessa fila e salvando no arquivo

char listaAminoacidos[5];

int main(){
	FILE *fips;
	char ipLinha[2][20];
	if ((fips = fopen("ips_servidores.txt", "r")) == NULL) {
		printf("Error! opening file");
		return 1;
	}
	int i=0;
	while(fgets(ipLinha[i], 13, fips) != NULL) {
		
		printf("%s", ipLinha[i]);
		i++;
	}

listaAminoacidos="ABC";
while(listaAminoacidos){
	listaAminoacidos
}
}
