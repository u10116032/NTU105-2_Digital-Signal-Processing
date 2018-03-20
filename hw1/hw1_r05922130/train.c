//
//  train.c
//  DSP_hw1
//

//

#include "hmm.h"
#include "stdio.h"

# define obsv_length  50



void forward_alg(double alpha[50][6], char token[obsv_length], const HMM *hmm);
void backward_alg (double beta[50][6], char token[obsv_length], const HMM *hmm);
void cal_Gamma (double alpha[50][6], double beta[50][6], double Gamma[50][6], const HMM *hmm);
void cal_epsilon(double alpha[50][6], double beta[50][6], char token[obsv_length], double epsilon[50][6][6], const HMM *hmm);
void train(double Gamma[50][6], double epsilon[50][6][6], double sum_B_Gamma_numerator[6][6], HMM *hmm, int count);

long  iteration;


int main(int argc, const char * argv[]) {
    
    iteration = strtol(argv[1], NULL, 10);
    
    printf("iteration: %ld\n", iteration);
    //load initial model
    
    HMM hmm;
    loadHMM(&hmm, argv[2]);
    
    for (int i = 0; i < iteration; i++){
        
        printf("%.0f%%\n", (double)(i+1) * 100/ (double)iteration);
        
        double sum_Gamma[50][6];
        memset( (void *)sum_Gamma, 0, sizeof(double)*50*6 );
        double sum_epsilon[50][6][6];
        memset( (void *)sum_epsilon, 0, sizeof(double)*50*6*6 );
        double sum_B_Gamma_numerator[6][6];
        memset( (void *)sum_B_Gamma_numerator, 0, sizeof(double)*6*6 );
        char token[obsv_length] = "";
        
        
        
        int count = 0;
        
        //training data loading
        FILE *file = open_or_die(argv[3], "r");

        //calculate each gamma & epsilon
        while(EOF != fscanf(file, "%s", token)){
            count++;
            
            double alpha[50][6];
            double beta[50][6];
            double Gamma[50][6];
            double epsilon[50][6][6];
            
            
            forward_alg(alpha, token, &hmm);
            backward_alg(beta, token, &hmm);
            cal_Gamma(alpha, beta, Gamma, &hmm);
            cal_epsilon(alpha, beta, token, epsilon, &hmm);
            
            //sum N for Gamma
            for(int i =0; i<50; i++){
                for(int j = 0; j <6; j++){
                    sum_Gamma[i][j] += Gamma[i][j];
                    
                    int obsv_num = token[i] - 'A';
                    sum_B_Gamma_numerator[obsv_num][j] += Gamma[i][j];
                }
                
            }
            
            //sum N for epsilon
            for (int i = 0; i <50; i++){
                for (int j = 0; j<6; j++){
                    for(int k = 0; k < 6; k++){
                        sum_epsilon[i][j][k] += epsilon[i][j][k];
                    }
                }
            }
            
        }
        fclose(file);
        
        //train
        train(sum_Gamma, sum_epsilon, sum_B_Gamma_numerator, &hmm, count);
        
    }
    
    //output trained model
    FILE *ofp = open_or_die(argv[4], "w");
    dumpHMM(ofp, &hmm);

}



void forward_alg (double alpha[50][6], char token[obsv_length], const HMM *hmm){
    //calculate alpha[0]
    
    int obsv_index = 0;
    
    
    while (obsv_index < obsv_length){
        /*read obsv*/
    
        int obsv_num = token[obsv_index] - 'A';
        
        if (obsv_index == 0){
            for (int i=0;i<6;i++){
                alpha[obsv_index][i] = hmm->initial[i] * hmm->observation[obsv_num][i];
            }
        }
        else{
            for (int i= 0; i< 6; i++){
                double sum = 0;
                for (int j= 0; j< 6; j++){
                    sum += alpha[obsv_index - 1][j] * hmm->transition[j][i];
                }
                alpha[obsv_index][i] = sum * hmm->observation[obsv_num][i];
            }
        }
        

        obsv_index++;
    }
}

void backward_alg (double beta[50][6], char token[obsv_length], const HMM *hmm){
    
    int obsv_index = obsv_length-1;
    
    
    while (obsv_index >= 0){
        /*read obsv*/
        
        
        if (obsv_index == obsv_length-1){
            for (int i=0;i<6;i++){
                beta[obsv_index][i] = 1;
            }
        }
        else{
            int obsv_num = token[obsv_index + 1] - 'A';

            for (int i= 0; i< 6; i++){
                double sum = 0;
                for (int j= 0; j< 6; j++){
                    sum += hmm->transition[i][j] * hmm->observation[obsv_num][j] * beta[obsv_index+1][j];
                }
                beta[obsv_index][i] = sum ;
            }
        }
        
        
        obsv_index--;
    }
}

void cal_Gamma (double alpha[50][6], double beta[50][6], double Gamma[50][6], const HMM *hmm){
    
    for (int i = 0; i < obsv_length; i ++){
        double sum = 0;
        
        for (int j = 0; j < 6; j++){
            sum += alpha[i][j] * beta[i][j];
        }
        
        for (int j = 0; j < 6; j++){
            Gamma[i][j] = ( alpha[i][j] * beta[i][j] ) / sum;
        }

    }
}

void cal_epsilon(double alpha[50][6], double beta[50][6], char token[obsv_length], double epsilon[50][6][6], const HMM *hmm){
    
    
    for (int obsv_idx = 0; obsv_idx < obsv_length - 1 ; obsv_idx++){
        
        int obsv_num = token[obsv_idx + 1] - 'A';

        
        double sum = 0;
        for (int j = 0; j <6; j++){
            for (int i = 0; i <6; i++){
                sum += alpha[obsv_idx][i] * hmm->transition[i][j] * hmm->observation[obsv_num][j] * beta[obsv_idx + 1][j];
            }
        }
        for (int j = 0; j <6; j++){
            for (int i = 0; i <6; i++){
                epsilon[obsv_idx][i][j] = (alpha[obsv_idx][i] * hmm->transition[i][j] * hmm->observation[obsv_num][j] * beta[obsv_idx + 1][j]) / sum;
            }
        }
        
    }
    
}

void train(double Gamma[50][6], double epsilon[50][6][6], double sum_B_Gamma_numerator[6][6], HMM *hmm, int count){
    
    //update initial
    for (int i = 0; i < 6 ; i++)
        hmm->initial[i] = Gamma[0][i] / count;
    
    //update A
    for (int i = 0; i < 6; i ++){
        for (int j = 0; j <6; j++){
            double sum1 = 0;
            double sum2 = 0;
            for (int t = 0 ; t < (obsv_length - 1) ; t++){
                sum1 += epsilon[t][i][j];
                sum2 += Gamma[t][i];
            }
            hmm->transition[i][j] = (sum1 / sum2);
        }
    }
    
    //update B
    for (int j = 0; j < 6; j++){
        for (int k = 0; k <6; k++){
            double sum = 0;
            for (int t = 0; t < obsv_length;  t++){
                sum += Gamma[t][j];
            }
            hmm->observation[k][j] = sum_B_Gamma_numerator[k][j] / sum ;
        }
    }
    
}



