#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
static FILE *open_or_die( const char *filename, const char *ht )
{
    FILE *fp = fopen( filename, ht );
    if( fp == NULL ){
        perror( filename);
        exit(1);
    }

    return fp;
}
int main(){
    char *gt="result.txt",*ts="./data/test_lbl.txt";
    FILE *f1=open_or_die(gt,"r"),*f2=open_or_die(ts,"r");
    double c=0,cc=2500;
    char seq[200];
    int a1[2501],a2[2501];
    int t=0;
    while(fgets(seq,200,f1)!=NULL){
        a1[t]=seq[7]-'0';
        t++;
	}
    t=0;
    while(fgets(seq,200,f2)!=NULL){
        a2[t]=seq[7]-'0';
        t++;
	}
    for(int i=0;i<2500;i++){
        if(a1[i]==a2[i]) c++;
    }
    printf("accuracy=%lf\n",c/cc);
    return 0;
}