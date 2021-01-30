#include "hmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
using namespace std;

/* header :
define MAX_STATE MAX_OBSERV MAX_SEQ MAX_LINE
class HMM
	char model_name;
    int state_num;				
    int observ_num;				
    double initial[MAX_STATE];		
    double transition[MAX_STATE][MAX_STATE];	
    double observation[MAX_OBSERV][MAX_STATE];
function : 
loadHMM(model_name , file_name)  
open_or_die 
dumpHMM(file_name , model_name)

Input 
1. number of iterations
2. initial model (model_init.txt)
3. observation sequences(train_seq_01~05.txt)
*/
char modellist[5][30] = {"../data/train_seq_01.txt","../data/train_seq_02.txt","../data/train_seq_03.txt","../data/train_seq_04.txt","../data/train_seq_05.txt"};
char output[5][30] = {"model_01.txt","model_02.txt","model_03.txt","model_04.txt","model_05.txt"};
int main ( int argc ,char* argv[] ) {
	for (int i=0; i<5 ;i++){

	HMM hmm;
	int lines = 0;
	char text[10000][MAX_SEQ];
	int iteration =100;
	loadHMM(&hmm, "../model_init.txt");
	FILE *seq_file = open_or_die(modellist[i],"r");
	
	while (fscanf(seq_file,"%s", text[lines]) != EOF){
			lines++; 
	}
	fclose(seq_file);
	for (int epoch=0; epoch<iteration; epoch++){
		char seq[MAX_SEQ];
		int count = 0;
		int state_num = hmm.state_num,observ_num = hmm.observ_num ;
		double pi_sum[state_num] = {0};
		double gamma_sum[state_num] = {0};
		double gamma_sum2[state_num] = {0};
		double epsilon_sum[state_num][observ_num] = {0} ;
		double ob_prob[state_num][observ_num] = {0} ;
		for (int line=0;line<lines;line++){
			count++ ;
			for (int i = 0; i < 50 ; i++) {
        		seq[i] = text[line][i];
			}
			int T = 50;
			for (int t = 0; t<T; t++) seq[t]-= 'A';
			//initialize alpha 
			double alpha[T][state_num] ={0};
			for(int  i=0 ; i<state_num ;i++ ){ 
            	alpha[0][i] = hmm.initial[i]*hmm.observation[seq[0]][i];
        	}
        	//calculate alpha
        	for(int t=1;t<T;t++){ 
            	for(int  i=0 ; i<state_num ; i++ ){ 
                	alpha[t][i] = 0;
            	
	                for(int  j=0 ; j<state_num ; j++ ){ 
	                    alpha[t][i] += alpha[t-1][j]*hmm.transition[j][i];
	                }
	                alpha[t][i] *= hmm.observation[seq[t]][i];
            	}
        	}
			//initialize beta
			double beta[T][state_num] ={0} ;
			for(int  i=0 ; i<state_num ;i++ ){ 
            	beta[T-1][i] = 1;
        	}
        	//calculate beta
        	for(int t= T-2 ; t >= 0; t--){ 
            	for(int  i=0 ; i<state_num ; i++ ){ 
                	beta[t][i] = 0;
	                for(int  j=0 ; j<hmm.state_num ; j++ ){ 
	                    beta[t][i] += hmm.transition[i][j]*hmm.observation[seq[t+1]][j]*beta[t+1][j];
	                }
            	}
        	}
			//calculate gamma 
			double gamma[T][state_num] ={0}  ;
			for ( int t = 0; t < T; t++ ) {
				double sum =0;
				for ( int i = 0; i < state_num; i++ ){
					gamma[t][i] = 0 ;
					sum += alpha[t][i] * beta[t][i];
				}
				for ( int i = 0; i < state_num; i++ ){
					gamma[t][i] = (alpha[t][i] * beta[t][i] / sum);
					
				}
			}
			//calculate epsilon
			double epsilon[T-1][state_num][state_num] ={0} ;
			for ( int t = 0; t < T-1; t++ ) {
				double sum = 0;
				for ( int i = 0; i < state_num; i++ ) {
					for ( int j = 0; j < state_num; j++ ) {
						epsilon[t][i][j] = alpha[t][i] * hmm.transition[i][j] * hmm.observation[seq[t+1]][j] * beta[t+1][j];
						sum += epsilon[t][i][j];
					}
				}
				for ( int i = 0; i < state_num; i++ ){
					for ( int j = 0; j < state_num; j++ ){ 					
						epsilon[t][i][j] /= sum;
					}
				}
				//cout<< sum <<endl;
			}
			//update summation
			for ( int i = 0; i < state_num; i++ ){
				pi_sum[i] += gamma[0][i];
				/*cout << gamma[0][i]<<endl;*/		
			}
			for ( int t = 0; t < T-1; t++ ) {
				for ( int i = 0; i < state_num; i++ ){
					gamma_sum[i] += gamma[t][i];
					gamma_sum2[i] += gamma[t][i];				
					ob_prob[i][seq[t]] += gamma[t][i];
					for ( int j = 0; j < hmm.state_num; j++ ){
						epsilon_sum[i][j] += epsilon[t][i][j];
					}
				}
			}
			for (int i = 0; i<state_num; i++){
				gamma_sum2[i] += gamma[T-1][i];
				ob_prob[i][seq[T-1]] += gamma[T-1][i];
			}
		
		}	
		//update pi
		for ( int i = 0; i < state_num; i++ ) {	
			hmm.initial[i] = pi_sum[i]/count;
			pi_sum[i] = 0;	
		}
		//update transition_prob
		for ( int i = 0; i < state_num; i++ ) {		
			for ( int j = 0; j < state_num; j++ ) {
				hmm.transition[i][j] = epsilon_sum[i][j] / gamma_sum[i];
				epsilon_sum[i][j] = 0;	
			}
			gamma_sum[i] = 0;
		}
		//update observation_prob
		for ( int i = 0; i < state_num; i++ ) { 	
				for ( int k = 0; k < observ_num; k++) {
					hmm.observation[k][i] = ob_prob[i][k]/ gamma_sum2[i];
					ob_prob[i][k] = 0;
				}
			gamma_sum2[i] = 0; 
		}
		printf("iteration: %d\n", epoch);
		//dumpHMM(stderr, &hmm);
	}
	FILE *trained_model = open_or_die( output[i], "w" );
	
	dumpHMM(trained_model, &hmm)	;
		
}
return 0;
}
