#include "hmm.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
/* 
Input 1. a file of paths to the models trained in the previous step (modellist.txt)
      2. observation sequences (test_seq.txt)
Output 
      best answer labels and P(O | £f) 
 */


int main( int argc, char* argv[] ) {
	HMM modellist[5];
	int  model_num = load_models(argv[1], modellist, 5);
	FILE *test_seq = open_or_die(argv[2], "r");
	FILE *ans_label = open_or_die(argv[3], "w");
	/*int  model_num = load_models("../modellist.txt", modellist, 5);
	FILE *test_seq = open_or_die("../data/test_seq.txt", "r");
	FILE *ans_label = open_or_die("../result.txt", "w");*/
	int lines = 0;
	int T = 50;
	char text[2500][MAX_SEQ];
	char seq[MAX_SEQ], ans[MAX_SEQ];
	while (fscanf(test_seq,"%s", text[lines]) != EOF){
			lines++; 
	}
	fclose(test_seq);
	for(int line =0; line<lines; line++) {
		for (int i = 0; i < T ; i++) {
        		seq[i] = text[line][i];
			}
		for(int t=0; t<T; t++)  seq[t] -= 'A';
		double best_prob = -1;
		int best_model =-1 ;
		for ( int model_index = 0; model_index < model_num; model_index++ ) {
			HMM model = modellist[model_index];
			int state_num = model.state_num;
			double delta[T][state_num] = {0};
			for ( int i = 0; i < state_num; i++ ){
				//initialize delta
				delta[0][i] = model.initial[i]*model.observation[seq[0]][i];
			}
			//propagating delta
			for ( int t = 1; t < T; t++ ) {
				for ( int j = 0; j < state_num; j++ ) {
					double max = -1;
					for ( int i = 0; i < state_num; i++ ) {
						if ( delta[t-1][i] * model.transition[i][j] > max )
							max = delta[t-1][i] * model.transition[i][j];
					}
					delta[t][j] = max * model.observation[seq[t]][j];
				}
			}
			// determine prob of all models thus find best_model
			double prob_max = -1;
			for ( int i = 0; i < state_num; i++ ){
				if (delta[T-1][i] > prob_max)
					prob_max = delta[T-1][i];
			}
			if ( prob_max > best_prob ) {
				best_prob = prob_max;
				best_model = model_index;
			}
		}
		fprintf(ans_label, "%s %e\n", modellist[best_model].model_name, best_prob);
	}
	fclose(ans_label);
	return 0;
}
