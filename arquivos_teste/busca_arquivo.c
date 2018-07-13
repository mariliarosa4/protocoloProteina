#include<stdio.h>
#include<string.h>
#include<stdlib.h>
char *procuraAmino(char *proteina, char *amino, char *repl) {
  static char buffer[609];
  char *p;
  if (!(p = strstr(proteina, amino))){
  	return "xx";
  }else{
  		strncpy(buffer, proteina, p-proteina);  
  	buffer[p-proteina] = 0;
  	sprintf(buffer+(p-proteina), "%s%s", repl, p+strlen(amino));
  	
  	return buffer;
  }
   	
  
  }
  
int main()
{
	FILE *fptr;
	char c[700];
	if ((fptr = fopen("proteina.txt", "r+")) == NULL) {
		printf("Error! opening file");
		return 1;
	}
// read the file and find the match

	while(fgets(c, 700, fptr) != NULL) {
		char *proteinaMudada;
		proteinaMudada = procuraAmino(c,"B","-");
		printf(" %s", proteinaMudada);
		FILE *p;
		if (!(p = fopen("proteina.txt","w")))  {                         
	  		printf("Erro! Impossivel abrir o arquivo!\n");
	  		exit(1);
  		}
  		if (proteinaMudada=="xx"){
  			printf("farsa");
		  }
	fprintf(p,"%s", proteinaMudada);
	fclose(p);

	//	printf("%s\n", procuraAmino(c,"M","-"));
		
	}

fclose(fptr);

	return 0;

}
