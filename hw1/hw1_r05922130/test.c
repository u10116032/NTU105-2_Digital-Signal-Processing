//
//  test.c
//  DSP_hw1
//
//  Created by 王瀚磊 on 2017/5/5.
//  Copyright © 2017年 王瀚磊. All rights reserved.
//

#include "hmm.h"

#define obsv_length 50

double viterbi_alg(HMM hmm, char token[obsv_length]);
void acc(char* testanswer, char* result, char* acc);

int main(int argc, const char * argv[]) {
    
    HMM hmms[5];
    load_models(argv[1], hmms, 5);
    char token[obsv_length] = "";
    
    
    //load testing data
    FILE *file = open_or_die(argv[2], "r");
    //open result file
    FILE *ans = open_or_die(argv[3], "w");
    while(EOF != fscanf(file, "%s", token)){
        double maxProb = 0;
        int maxIdx = 0;
        for ( int i = 0; i < 5; i++){
            if (viterbi_alg(hmms[i], token) > maxProb){
                maxProb = viterbi_alg(hmms[i], token);
                maxIdx = i;
            }
        }
        
        
        //output predict result
        
        
        fprintf(ans, "model_0%d.txt %e\n", maxIdx + 1, maxProb);
        
    }
    
    fclose(file);
    fclose(ans);
    
    if (strcmp(argv[3] , "result1.txt") == 0)
        acc("testing_answer.txt", "result1.txt", "acc.txt");
}

double viterbi_alg(HMM hmm, char token[obsv_length]){
    double delta[obsv_length][6];
    memset((void*)delta, 0, sizeof(double)*50*6);
    
    for (int t = 0; t < 50; t++){
        
        int obsv_num = token[t] - 'A';
        
        if( t == 0){
            for (int i = 0 ; i <6; i++)
                delta[t][i] = hmm.initial[i] * hmm.observation [obsv_num][i];
        }
        
        else{
            for (int j = 0; j <6; j++){
                double max = 0;
                for (int i = 0; i < 6; i++){
                    double temp = delta[t-1][i] * hmm.transition[i][j];
                    if (temp > max)
                        max = temp;
                }
                delta[t][j] = max * hmm.observation[obsv_num][j];
            }
        }
    }
    
    double maxProb = 0;
    for (int i = 0; i < 6; i++){
        double temp = delta[obsv_length-1][i];
        if (temp > maxProb)
            maxProb = temp;
    }
    
    return maxProb;
}

void acc(char* testanswer, char* result, char* acc){
    
    //Open The File
    FILE *an = fopen(testanswer, "r");
    FILE *re = fopen(result, "r");
    FILE *ac = fopen(acc, "w");
    
    //Intial Parameters
    double score = 0;
    char ans[2500][12] = { "" };
    char res[5000][50] = { "" };
    
    //Read Answer
    for (int i = 0; i < 2500; i++){
        fscanf(an, "%s", ans[i]);
    }
    
    //Read Result
    for (int i = 0; i < 5000; i++){
        fscanf(re, "%s", res[i]);
    }
    
    //Compare
    for (int i = 0; i < 2500; i++){
        if ((res[i * 2][7]) == (ans[i][7]))
            score += 1;
    }
    
    //Output
    
    fprintf(ac, "%f\n", score / 2500);
    fclose(ac);
}








