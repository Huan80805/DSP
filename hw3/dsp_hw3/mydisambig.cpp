#include <stdio.h>
#include <stdlib.h>
#include "Ngram.h"
#include"Vocab.h"
#include"File.h"
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <algorithm>
#include <map>


using namespace std;
// used in viterbi algorithm (prob = delta, pre = index of previous word) 
struct Word{
	string str;
	double prob;
	int pre;
	Word(string a){
		str = a;
		prob = 0;
		pre = 0;
	}
};
// P(w2 | w1)
double getBigramProb(const char *w1, const char *w2, Vocab& voc, Ngram& lm){
	VocabIndex id1 = voc.getIndex(w1);
	VocabIndex id2 = voc.getIndex(w2);

	if(id1 == Vocab_None)  //OOV
		id1 = voc.getIndex(Vocab_Unknown);
	if(id2 == Vocab_None)  //OOV
		id2 = voc.getIndex(Vocab_Unknown);

	VocabIndex context[] = { id1, Vocab_None };
    // note:wordprob is in logarithm
	return lm.wordProb( id2, context);
}


int main(int argc, char* argv[])
{
	int order = 2;
    if (argc != 5){
        perror("error: command error");
    }
    char* text_path = argv[1];
	char* map_path = argv[2];
	char* lm_path = argv[3];
	char* of_path = argv[4];
    ifstream text (text_path, ifstream::in);
    ifstream maps (map_path, ifstream::in);

    // read language model
	Vocab voc;
	Ngram lm(voc, order);
	File lmFile(lm_path, "r");
	lm.read(lmFile);
	lmFile.close();

	// read map file and stores it in a map
	string map_line;
	map <string, vector<Word> > M;
    if(!maps){
            perror("error: mapfile error");
        }
    while(getline(maps,map_line)){
		int l = strlen(map_line.c_str());
		string key;
		string word;
		key.assign(map_line.begin(),map_line.begin()+2);
		for(int i = 3; i < l; i+=3){
			word.assign(map_line.begin()+i,map_line.begin()+i+2);
			M[key].push_back(word);
        }
	}
    maps.close();

	// read in text and predict
    ofstream out_file(of_path,ofstream::out);
    vector<vector<Word> > ztc;		//zhuyin to character
    vector<string> word_list;
    string line;
    int line_count = 0;
	while(getline(text, line)){
        line_count ++;
		ztc.clear();
        word_list.clear();
	    for(int i = 0; i < line.length()-1; i++){
		    string word;
            if (line[i] != ' ' ){
                word.assign(line.begin()+i,line.begin()+i+2);
			    word_list.push_back(word);
                i++;
            }
	    }
              
        vector<Word> z2w;
        for (int i = 0; i < word_list.size(); i++){
			// find all character mapped by the zhuyin and push to the vector
            for (int j=0; j< M[word_list[i]].size(); j++){
                z2w.push_back(M[word_list[i]][j]);
            }
			ztc.push_back(z2w);
            z2w.clear();
		}
        vector<Word> end_str;
        end_str.push_back(Word("</s>"));  
        ztc.push_back(end_str);        

        // Viterbi alg  :find delta(best_prob) and record prev node
        // initialize
        for(int i=0; i< ztc[0].size(); i++){
			ztc[0][i].prob = getBigramProb("<s>", ztc[0][i].str.c_str(), voc, lm);
        }
		// induction

        for(int i=1; i<ztc.size(); i++){
            for(int j=0; j<ztc[i].size(); j++){
				double delta = -9999999;
				int pw = 0;  // best previous word
				int cur_pw = 0;
                for(int k=0; k<ztc[i-1].size(); k++){
					double prob = (getBigramProb(ztc[i-1][k].str.c_str(), ztc[i][j].str.c_str(), voc, lm) + ztc[i-1][k].prob);
					if(prob >= delta){
						delta = prob;
						pw = cur_pw;
					}
					cur_pw ++;
				}
				ztc[i][j].pre = pw;
				ztc[i][j].prob = delta;
			}
		}

        // output result depending on delta
		vector<string> result;
        // backtrack  previous node
        int final = ztc.size()-1;
        double prob = -9999999;
        int best_word;
        for(int i=0; i<ztc[final].size(); i++){
            Word pos = ztc[final][i];
            if (pos.prob > prob){
                prob = pos.prob;
                best_word = i;
            }
        }
        for(int i=final; i>=0; i-- ){
            vector<Word> wpos = ztc[i];
			result.push_back(wpos.at(best_word).str);
			best_word = wpos.at(best_word).pre;
		}
        if(!out_file){
            perror("error: outputfile error");
        }
        out_file << "<s>";
        for(int i= result.size()-1; i>=0; i--){
            string word = result[i];
			out_file << " " << word.c_str()  ;
        }
        result.clear();
		out_file<< "\n" ;
        cerr << "line_count: "<<line_count <<endl;
	}
    text.close();
    out_file.close();
}