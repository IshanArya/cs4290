#ifndef GSHARE_H
#define GSHARE_H

#include "branchsim.h"
#include <vector>




// Gshare predictor definition
class gshare : public branch_predictor_base
{

private:
    uint64_t ghr;
    struct entry {
        uint8_t pattern;
        uint64_t tag;
    };
    std::vector<entry> patternTable;

public:
    void init_predictor(branchsim_conf *sim_conf);
    
    // Return the prediction ({taken/not-taken}, target-address)
    bool predict(branch *branch, branchsim_stats *sim_stats);
    
    // Update the branch predictor state
    void update_predictor(branch *branch);

    // Cleanup any initialized memory here
    ~gshare();
};

#endif
