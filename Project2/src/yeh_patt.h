#ifndef YEH_PATT_H
#define YEH_PATT_H

#include "branchsim.h"
#include <vector>

// Yeh-Patt predictor definition
class yeh_patt : public branch_predictor_base
{
private:
    struct entry {
        uint64_t tag;
        uint64_t shiftRegister;
    };

    std::vector<entry> historyTable;
    std::vector<uint64_t> patternTable;

public:
    void init_predictor(branchsim_conf *sim_conf);
    
    // Return the prediction ({taken/not-taken}, target-address)
    bool predict(branch *branch, branchsim_stats *sim_stats);
    
    // Update the branch predictor state
    void update_predictor(branch *branch);

    // Cleanup any initialized memory here
    ~yeh_patt();
};

#endif
