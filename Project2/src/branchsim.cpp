
#include <iostream>

#include "branchsim.h"
#include "gshare.h"
#include "perceptron.h"
#include "yeh_patt.h"

// Don't modify this line -- Its to make the compiler happy
branch_predictor_base::~branch_predictor_base() {}


// ******* Student Code starts here *******

uint64_t G;
uint64_t P;
uint64_t N;

uint64_t index_bits(uint64_t G, uint64_t ip) {
    return (ip >> 2) & ((1 << G) - 1);
}

uint64_t branch_tag(uint64_t G,  uint64_t ip) {
    return (ip >> (G + 2));
}

// Gshare Branch Predictor

void gshare::init_predictor(branchsim_conf *sim_conf)
{

    G = sim_conf->G;
    N = sim_conf->N;

    ghr = 0;
    uint64_t table_size = 1 << sim_conf->G;

//    patternTable.resize(table_size);


    for(int i = 0; i < table_size; i++) {
        patternTable.push_back({
           .tag = 0,
           .pattern = 1
        });
    }

//    std::cout << "Table 1st entry: " << (int) patternTable[0].pattern << "\n";
}

bool gshare::predict(branch *branch, branchsim_stats *sim_stats)
{
    uint64_t predictor = index_bits(G, branch->ip);
    uint64_t index = predictor ^ ghr;
    uint64_t tag = branch_tag(G, branch->ip);

//    std::cout << "Index: " << (uint64_t) index << "\n";
//    std::cout << "Predictor: " << (uint64_t) predictor << "\n";
//    std::cout << "GHR: " << (uint64_t) ghr << "\n";

    if(patternTable[index].tag != tag) {
        sim_stats->num_tag_conflicts++;
    }

    return patternTable[index].pattern > 1;
}

void gshare::update_predictor(branch *branch)
{
    uint64_t index = index_bits(G, branch->ip) ^ ghr;
    uint64_t tag = branch_tag(G, branch->ip);

    ghr = ghr << 1;
    if(branch->is_taken) {
        ghr |= 1;
    }
    if(G < 64) {
        ghr &= ((1 << (G)) - 1);
    }

    if(branch->is_taken) {
        if(patternTable[index].pattern < 3) {
            patternTable[index].pattern++;
        }
    } else {
        if(patternTable[index].pattern > 0) {
            patternTable[index].pattern--;
        }
    }

    patternTable[index].tag = tag;

}

gshare::~gshare()
{

}


// Yeh-Patt Branch Predictor

void yeh_patt::init_predictor(branchsim_conf *sim_conf)
{
    G = sim_conf->G;
    P = sim_conf->P;
    N = sim_conf->N;

    uint64_t history_table_size = 1 << sim_conf->G;
    uint64_t pattern_table_size = 1 << sim_conf->P;

    historyTable.resize(history_table_size);

    for(int i = 0; i < pattern_table_size; i++) {
        patternTable.push_back(1);
    }
}

bool yeh_patt::predict(branch *branch, branchsim_stats *sim_stats)
{
    uint64_t index = index_bits(G, branch->ip);
    uint64_t tag = branch_tag(G, branch->ip);

    struct entry hist_entry = historyTable[index];

    if(hist_entry.tag != tag) {
        sim_stats->num_tag_conflicts++;
    }

    return patternTable[hist_entry.shiftRegister] > 1;
}

void yeh_patt::update_predictor(branch *branch) {
    uint64_t index = index_bits(G, branch->ip);
    uint64_t tag = branch_tag(G, branch->ip);

    struct entry *hist_entry = &historyTable[index];
    uint64_t pt_entry = hist_entry->shiftRegister;

    hist_entry->tag = tag;

    hist_entry->shiftRegister = hist_entry->shiftRegister << 1;
    if(branch->is_taken) {
        hist_entry->shiftRegister |= 1;
    }
    if(P < 64) {
        hist_entry->shiftRegister &= ((1 << P) - 1);
    }

    if(branch->is_taken) {
        if(patternTable[pt_entry] < 3) {
            patternTable[pt_entry]++;
        }
    } else {
        if(patternTable[pt_entry] > 0) {
            patternTable[pt_entry]--;
        }
    }

}

yeh_patt::~yeh_patt()
{

}


// Perceptron Branch Predictor

void perceptron_predictor::init_predictor(branchsim_conf *sim_conf)
{
    G = sim_conf->G;
    P = sim_conf->P;
    N = sim_conf->N;

    ghr = 0;
    theta = (1.93 * (double)G) + 14;
    uint64_t pTable_size = 1 << P;
    perceptronTable.resize(pTable_size);

    for(int i = 0; i < pTable_size; i++) {
        perceptronTable[i].weights.resize(G + 1);
    }
}

bool perceptron_predictor::predict(branch *branch, branchsim_stats *sim_stats)
{
    uint64_t index = index_bits(P, branch->ip);
    uint64_t tag = branch_tag(P, branch->ip);

    struct entry perceptron = perceptronTable[index];

    if(perceptron.tag != tag) sim_stats->num_tag_conflicts++;

    last_y = perceptron.weights[0];

//    std::cout << "GHR: " << (uint64_t) ghr << "\n";

    for(int i = 0; i < G; i++) {
        int64_t x = (((ghr >> i) & 1) > 0) ? 1 : -1;
//        std::cout << "point: " << (uint64_t) ((ghr >> i) & 1) << "\n";
//        std::cout << "isolation point: " << (uint64_t) isolation_point << "\n";
//        std::cout << "X: " << (int64_t) x << " for i: " << i <<"\n";
        last_y += x * perceptron.weights[i + 1];
    }

//    std::cout << "Y: " << (int) last_y << "\n";

    return last_y >= 0;
}

void perceptron_predictor::update_predictor(branch *branch)
{
    uint64_t index = index_bits(P, branch->ip);
    uint64_t tag = branch_tag(P, branch->ip);

    struct entry *perceptron = &perceptronTable[index];

    perceptron->tag = tag;

    int64_t t = branch->is_taken ? 1 : -1;


    if(((last_y >= 0) != branch->is_taken)
        || std::labs(last_y) <= theta) {
        perceptron->weights[0] += t;

        int64_t lower_bound = -(theta + (int64_t)1);
        if(perceptron->weights[0] > theta) perceptron->weights[0] = theta;
        else if(perceptron->weights[0] < lower_bound) perceptron->weights[0] = lower_bound;

        for(int i = 0; i < G; i++) {
            int64_t x = (((ghr >> i) & 1) > 0) ? 1 : -1;
            perceptron->weights[i + 1] += t * x;
            if(perceptron->weights[i + 1] > theta) perceptron->weights[i + 1] = theta;
            else if(perceptron->weights[i + 1] < -(theta + 1)) perceptron->weights[i + 1] = -(theta + 1);
        }
    }

    ghr = ghr << 1;
    if(branch->is_taken) {
        ghr |= 1;
    }
//    if(G < 64) {
//        ghr &= ((((uint64_t) 1) << (G)) - 1);
//    }
}

perceptron_predictor::~perceptron_predictor()
{

}


// Common Functions to update statistics and final computations, etc.

/**
 *  Function to update the branchsim stats per prediction
 *
 *  @param perdiction The prediction returned from the predictor's predict function
 *  @param branch Pointer to the branch that is being predicted -- contains actual behavior
 *  @param stats Pointer to the simulation statistics -- update in this function
*/
void branchsim_update_stats(bool prediction, branch *branch, branchsim_stats *sim_stats)
{
    sim_stats->num_branch_instructions++;
    if(prediction == branch->is_taken) {
        sim_stats->num_branches_correctly_predicted++;
    } else {
        sim_stats->num_branches_miss_predicted++;
    }

    sim_stats->total_instructions = branch->inst_num;
}

/**
 *  Function to cleanup branchsim statistic computations such as prediction rate, etc.
 *
 *  @param stats Pointer to the simulation statistics -- update in this function
*/
void branchsim_cleanup_stats(branchsim_stats *sim_stats)
{
    sim_stats->fraction_branch_instructions = ((double)sim_stats->num_branch_instructions) / ((double)sim_stats->total_instructions);
    sim_stats->prediction_accuracy = ((double) sim_stats->num_branches_correctly_predicted) / ((double)sim_stats->num_branch_instructions);
    sim_stats->misses_per_kilo_instructions = ((double)sim_stats->num_branches_miss_predicted) / ((double)sim_stats->total_instructions) * 1000.;
    sim_stats->N = N;
    sim_stats->stalls_per_miss_predicted_branch = N <= 7 ? 2 : (N / 2) - 1;
    sim_stats->average_CPI = 1 +
            ((double)sim_stats->num_branches_miss_predicted /
            sim_stats->total_instructions *
            sim_stats->stalls_per_miss_predicted_branch);
}
