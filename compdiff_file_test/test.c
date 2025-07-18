#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  FILE* fp1 = NULL;
  FILE *fp2 = NULL;
  char input[100];
  char output[100];
  //scanf("%s %s",input,output);
  fp1 = fopen(argv[1], "r");
  fp2 = fopen(argv[2], "w");
  char buf[8];

  for(int i = 0; i<8 ;++i){
    if(fscanf(fp1,"%c", &buf[i]) == EOF){
        fprintf(fp2,"error!\n");
        exit(1);
    }
  }
  if (buf[0] == '0'){
    fprintf(fp2,"I'm wasm one!\n");
    //fprintf(fp2, "I'm x86 one!\n");
  }
    //printf("I'm native one!\n");
    //printf("I'm wasm one!\n");
  else{
    fprintf(fp2,"A non-zero value? How quaint!\n");
  }
  exit(0);
}