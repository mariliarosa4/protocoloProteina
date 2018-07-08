#include<stdio.h>
#include<string.h>
#include<stdlib.h>
// um cliente p cada linha de servidor
// armazeno os aminoacidos numa fila e outra thread vai tirando dessa fila e salvando no arquivo
char ipLinha[20];

int main(){
	FILE *fips;
	if ((fips = fopen("ips_servidores.txt", "r+")) == NULL) {
		printf("Error! opening file");
		return 1;
	}
	while(fgets(ipLinha, 20, fips) != NULL) {
		printf("%s", ipLinha);
	}
}
