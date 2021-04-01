#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include "branchsim.h"
#include <vector>

// Perceptron predictor definition
class perceptron_predictor : public branch_predictor_base
{

private:
    uint64_t ghr;
    int64_t theta;
    int64_t last_y;
    struct entry {
        std::vector<int64_t> weights;
        uint64_t tag;
    };
    std::vector<entry> perceptronTable;

public:
    void init_predictor(branchsim_conf *sim_conf);
    
    // Return the prediction 
    bool predict(branch *branch, branchsim_stats *sim_stats);
    
    // Update the branch predictor state
    void update_predictor(branch *branch);

    // Cleanup any initialized memory here
    ~perceptron_predictor();
};


#endif
