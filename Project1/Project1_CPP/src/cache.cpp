/**
 * @file cache.c
 * @brief CS{4/6}290 / ECE{4/6}100 Spring 2021 Project 1 cache simulator
 *
 * Fill in the functions to complete the cache simulator
 *
 * @author Ishan Arya
 */

#include <cstdarg>
#include <cinttypes>
#include <cstdio>
#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <list>
#include <iostream>

#include "cache.hpp"

// Use this for printing errors while debugging your code
// Most compilers support the __LINE__ argument with a %d argument type
// Example: print_error_exit("Error: Could not allocate memory %d", __LINE__);
static inline void print_error_exit(const char *msg, ...)
{
    va_list argptr;
    va_start(argptr, msg);
    fprintf(stderr, msg, argptr);
    va_end(argptr);
    exit(EXIT_FAILURE);
}

// Define data structures and globals you might need for simulating the cache hierarchy below

/**
 * Data structure to store cache entry data. Accesses is used for LFU
 * and time is used for both LFU and LRU
 */
struct cache_entry {
    uint64_t addr;
    uint32_t time;
    uint32_t accesses;
    bool valid;
    bool dirty;
};

std::vector<std::vector<struct cache_entry>> l1; //l1 cache
std::list<struct cache_entry> victim; //victim cache

int indexBits; //storage of number of index bits for easy access
uint32_t timePass; //increased by 1 for each cache_access


// TODO

/**
 * Function to initialize any data structures you might need for simulating the cache hierarchy. Use
 * the sim_conf structure for initializing dynamically allocated memory.
 *
 * @param sim_conf Pointer to simulation configuration structure
 *
 */
void sim_init(struct sim_config_t *sim_conf)
{
    indexBits = sim_conf->l1data.c - sim_conf->l1data.s - sim_conf->l1data.b;
    int numberOfSets = 1 << indexBits;
    l1.resize(numberOfSets);
}

/**
 * Extract tag from address
 * @param addr
 * @param sim_conf
 * @return
 */
uint64_t get_tag(uint64_t addr, struct sim_config_t *sim_conf) {
    return addr >> (sim_conf->l1data.c - sim_conf->l1data.s);
}

/**
 * Extract index from address
 * @param addr
 * @param sim_conf
 * @return
 */
uint64_t get_index(uint64_t addr, struct sim_config_t *sim_conf) {
    uint64_t isolation = addr >> sim_conf->l1data.b;

    return isolation & ((1 << indexBits) - 1);
}

/**
 * Extract tag and index from address
 * @param addr
 * @param sim_conf
 * @return
 */
uint64_t get_tag_index(uint64_t addr, struct sim_config_t *sim_conf) {
    return addr >> (sim_conf->l1data.b);
}

/**
 * Contains logic for l1 eviction
 * @param index
 * @param sim_conf
 * @return
 */
struct cache_entry pop_l1(int index, struct sim_config_t *sim_conf) {
    int toRemove = 0;
    std::vector<struct cache_entry>& cacheSet = l1[index];
    switch (sim_conf->rp) {
        case LRU: { //Find cache entry with lowest time
            int time = INT_MAX;
            for (int i = 0; i < cacheSet.size(); i++) {
                if (cacheSet[i].time < time) {
                    toRemove = i;
                    time = cacheSet[i].time;
                }
            }
            break;
        }
        case FIFO: { //FIFO works like a queue
            toRemove = 0;
            break;
        }
        case LFU: {
            uint32_t time = 0;
            int mru = 0;
            for (int i = 0; i < cacheSet.size(); i++) { //Find most recently used cache
                if (cacheSet[i].time > time) {
                    mru = i;
                    time = cacheSet[i].time;
                }
            }

            uint32_t accesses = UINT32_MAX;
            for (int i = 0; i < cacheSet.size(); i++) { // Find least frequently used cache that is not MRU
                if(i == mru) continue;
                if (cacheSet[i].accesses < accesses) {
                    toRemove = i;
                    accesses = cacheSet[i].accesses;
                } else if(cacheSet[i].accesses == accesses) {
                    if(get_tag(cacheSet[i].addr, sim_conf) > get_tag(cacheSet[toRemove].addr, sim_conf)) {
                        toRemove = i;
                    }
                }
            }
            break;
        }
    }
//    std::cout << "To Remove: " << toRemove << "\n";
    struct cache_entry removed = cacheSet[toRemove];
    cacheSet.erase(cacheSet.begin() + toRemove);
    return removed;
}

/**
 * contains logic for victim eviction
 * @return
 */
struct cache_entry pop_victim() {
    struct cache_entry removed = victim.front();
    victim.pop_front();
    return removed;
}

/**
 * Erase and return given block from victim cache
 * @param addr
 * @param sim_conf
 * @return
 */
struct cache_entry retrieve_victim(uint64_t addr, struct sim_config_t *sim_conf) {
    struct cache_entry removed;
    uint64_t tagIndex = get_tag_index(addr, sim_conf);

    for(auto i = victim.begin(); i != victim.end(); i++) {
        if(get_tag_index(i->addr, sim_conf) == tagIndex) {
            removed = *i;
            victim.erase(i);
            break;
        }
    }

    return removed;
}

/**
 * Add a new entry to victim cache
 * @param entry
 * @param sim_stats
 * @param sim_conf
 */
void add_victim(struct cache_entry entry, sim_stats_t *sim_stats, struct sim_config_t *sim_conf) {
    if(victim.size() >= (sim_conf->V)) {
        struct cache_entry removed = pop_victim();
        if(removed.dirty) {
            sim_stats->num_writebacks += 1;
        }
    }

    victim.push_back(entry);
}

/**
 * Add new entry to l1 cache. Sets accesses to 1 and sets dirty bit if
 * needed
 * @param addr
 * @param sim_stats
 * @param sim_conf
 * @param make_dirty
 */
void add_l1(uint64_t addr, sim_stats_t *sim_stats, struct sim_config_t *sim_conf, bool make_dirty) {
    uint64_t index = get_index(addr, sim_conf);
    std::vector<struct cache_entry>& cacheSet = l1[index];

    struct cache_entry entry = {
            .addr = addr,
            .time = timePass,
            .accesses = 1,
            .dirty = make_dirty
    };

    if(cacheSet.size() >= (1 << sim_conf->l1data.s)) {
        struct cache_entry removed = pop_l1(index, sim_conf);
        sim_stats->l1data_num_evictions += 1;

        if(sim_conf->has_victim_cache) {
            add_victim(removed, sim_stats, sim_conf);
        } else {
            if(removed.dirty) {
                sim_stats->num_writebacks += 1;
            }
        }
    }

    cacheSet.push_back(entry);
}


/**
 * Check if address exists in cache
 * Increase accesses and time flags. Also make dirty if needed
 * @param addr
 * @param sim_conf
 * @param make_dirty
 * @return
 */
bool cache_exists_l1(uint64_t addr, struct sim_config_t *sim_conf, bool make_dirty) {
    uint64_t index = get_index(addr, sim_conf);
    uint64_t tag = get_tag(addr, sim_conf);

    std::vector<struct cache_entry> &cacheSet = l1[index];
    for(auto i = cacheSet.begin(); i != cacheSet.end(); i++) {
        if(get_tag(i->addr, sim_conf) == tag) {
            if(make_dirty) i->dirty = true;
            i->accesses += 1;
            i->time = timePass;
            return true;
        }
    }
    return false;
}

/**
 * check if address exists in victim cache
 * @param addr
 * @param sim_conf
 * @return
 */
bool cache_exists_victim(uint64_t addr, struct sim_config_t *sim_conf) {
    uint64_t tagIndex = get_tag_index(addr, sim_conf);

    for(auto i = victim.begin(); i != victim.end(); i++) {
        if(get_tag_index(i->addr, sim_conf) == tagIndex) return true;
    }
    return false;
}



/**
 * Function to perform cache accesses, one access at a time. The print_debug function should be called
 * if the debug flag is true
 *
 * @param addr The address being accessed by the processor
 * @param type The type of access - Load (L) or Store (S)
 * @param sim_stats Pointer to simulation statistics structure - Should be populated here
 * @param sim_conf Pointer to the simulation configuration structure - Don't modify it in this function
 */
void cache_access(uint64_t addr, char type, struct sim_stats_t *sim_stats, struct sim_config_t *sim_conf)
{
    timePass++;
    sim_stats->l1data_num_accesses += 1;
    if(type == LOAD) sim_stats->l1data_num_accesses_loads += 1;
    else if(type == STORE) sim_stats->l1data_num_accesses_stores += 1;

    if(cache_exists_l1(addr, sim_conf, type == STORE)) {
        return;
    }

    sim_stats->l1data_num_misses += 1;
    if(type == LOAD) sim_stats->l1data_num_misses_loads += 1;
    else if(type == STORE) sim_stats->l1data_num_misses_stores += 1;

    if(sim_conf->has_victim_cache) {
        sim_stats->victim_cache_accesses += 1;
        if(cache_exists_victim(addr, sim_conf)) {
            sim_stats->victim_cache_hits += 1;
            struct cache_entry entry = retrieve_victim(addr, sim_conf);
            add_l1(addr, sim_stats, sim_conf, type == STORE || entry.dirty);
            return;
        }
        sim_stats->victim_cache_misses += 1;
    }

    sim_stats->bytes_transferred_from_mem += (1 << sim_conf->l1data.b);
    add_l1(addr, sim_stats, sim_conf, type == STORE);

}


/**
 * Function to cleanup dynamically allocated simulation memory, and perform any calculations
 * that might be required
 *
 * @param stats Pointer to the simulation structure - Final calculations should be performed here
 */
void sim_cleanup(struct sim_stats_t *sim_stats, struct sim_config_t *sim_conf)
{
    // TODO
    sim_stats->bytes_transferred_to_mem = sim_stats->num_writebacks * (1 << sim_conf->l1data.b);
    sim_stats->l1data_miss_penalty = 60;
    sim_stats->l1data_miss_rate = sim_stats->l1data_num_accesses == 0 ? 0 : ((double) sim_stats->l1data_num_misses) / ((double) sim_stats->l1data_num_accesses);


    if(sim_conf->has_victim_cache) {
        sim_stats->victim_cache_miss_rate = ((double) sim_stats->victim_cache_misses) / ((double) sim_stats->victim_cache_accesses);
        sim_stats->avg_access_time = sim_stats->l1data_miss_rate * sim_stats->victim_cache_miss_rate * sim_stats->l1data_miss_penalty + sim_stats->l1data_hit_time;
    } else {
        sim_stats->avg_access_time = sim_stats->l1data_miss_rate * sim_stats->l1data_miss_penalty + sim_stats->l1data_hit_time;
    }







}
