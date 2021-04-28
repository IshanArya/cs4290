#include "MOSI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MOSI_protocol::MOSI_protocol (Hash_table *my_table, Hash_entry *my_entry)
        : Protocol (my_table, my_entry)
{
    this->state = MOSI_CACHE_I;
}

MOSI_protocol::~MOSI_protocol ()
{
}

void MOSI_protocol::dump (void)
{
    const char *block_states[8] = {"X","I","IS","S","O","OM","M","PM"};
    fprintf (stderr, "MOSI_protocol - state: %s\n", block_states[state]);
}

void MOSI_protocol::process_cache_request (Mreq *request)
{
//    const char *block_states[7] = {"X","I","IS","S","O","M","PM"};
//    fprintf (stderr, "State: %s\n", block_states[state]);
    switch (state) {
        case MOSI_CACHE_I:
            do_cache_I(request);
            break;
        case MOSI_CACHE_S:
            do_cache_S(request);
            break;
        case MOSI_CACHE_O:
            do_cache_O(request);
            break;
        case MOSI_CACHE_M:
            do_cache_M(request);
            break;

        default:
            fatal_error ("Invalid Cache State for MOSI Protocol\n");
    }
}

void MOSI_protocol::process_snoop_request (Mreq *request)
{
//    const char *block_states[7] = {"X","I","IS","S","O","M","PM"};
//    fprintf (stderr, "State: %s\n", block_states[state]);
    switch (state) {
        case MOSI_CACHE_I:
            do_snoop_I(request);
            break;
        case MOSI_CACHE_IS:
            do_snoop_IS(request);
            break;
        case MOSI_CACHE_S:
            do_snoop_S(request);
            break;
        case MOSI_CACHE_O:
            do_snoop_O(request);
            break;
        case MOSI_CACHE_OM:
            do_snoop_OM(request);
            break;
        case MOSI_CACHE_M:
            do_snoop_M(request);
            break;
        case MOSI_CACHE_PM:
            do_snoop_PM(request);
            break;
        default:
            fatal_error ("Invalid Cache State for MOSI Protocol\n");
    }
}

inline void MOSI_protocol::do_cache_I (Mreq *request)
{
    switch(request->msg) {
        case LOAD:
            send_GETS(request->addr);
            state = MOSI_CACHE_IS;
            Sim->cache_misses++;
            break;
        case STORE:
            send_GETM(request->addr);
            state = MOSI_CACHE_PM;
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }

}


inline void MOSI_protocol::do_cache_S (Mreq *request)
{
    switch(request->msg) {
        case LOAD:
            send_DATA_to_proc(request->addr);
            break;
        case STORE:
            send_GETM(request->addr);
            Sim->cache_misses++;
            state = MOSI_CACHE_PM;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: S state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_O (Mreq *request)
{
    switch(request->msg) {
        case LOAD:
            send_DATA_to_proc(request->addr);
            break;
        case STORE:
            send_GETM(request->addr);
            Sim->cache_misses++;
            state = MOSI_CACHE_OM;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: S state shouldn't see this message\n");
    }

}

inline void MOSI_protocol::do_cache_M (Mreq *request)
{

    switch (request->msg) {
        /* The M state means we have the data and we can modify it.  Therefore any request
         * from the processor (read or write) can be immediately satisfied.
         */
        case LOAD:
        case STORE:
            // This is how you send data back to the processor to finish the request
            // Note: There was no need to send anything on the bus on a hit.
            send_DATA_to_proc(request->addr);
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: M state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_I (Mreq *request)
{

    return;

}

inline void MOSI_protocol::do_snoop_IS (Mreq *request)
{
    switch (request->msg) {
        case DATA:
            /**
             * If we snoop a message from another cache and we are in I, then we don't
             * need to do anything!  We obviously cannot supply data since we don't have
             * it, and we don't need to downgrade our state since we are already in I.
             */
            send_DATA_to_proc(request->addr);
            state = MOSI_CACHE_S;
            break;
        case GETM:
        case GETS:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: IS state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_S (Mreq *request)
{
    switch(request->msg) {
        case GETS:
//        case DATA:
            break;
        case GETM:
            state = MOSI_CACHE_I;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: S state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_O (Mreq *request)
{

    switch(request->msg) {
        case GETS:
//            set_shared_line();
            send_DATA_on_bus(request->addr, request->src_mid);
            break;
        case GETM:
//            set_shared_line();
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOSI_CACHE_I;
            break;
        case DATA:
            fatal_error ("STATE SNOOP M: Should not see data for this line!  I have the line!");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: M state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_OM (Mreq *request)
{

    switch(request->msg) {
        case GETS:
            send_DATA_on_bus(request->addr, request->src_mid);
            break;
        case GETM:
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOSI_CACHE_PM;
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state = MOSI_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: OM state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_M (Mreq *request)
{

    switch(request->msg) {
        case GETS:
//            set_shared_line();
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOSI_CACHE_O;
            break;
        case GETM:
//            set_shared_line();
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOSI_CACHE_I;
            break;
        case DATA:
            fatal_error ("STATE SNOOP M: Should not see data for this line!  I have the line!");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: M state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_PM (Mreq *request)
{
    switch(request->msg) {
        case GETM:
        case GETS:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state = MOSI_CACHE_M;
//            if(get_shared_line()) {
//
//            }
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Snoop: PM state shouldn't see this message\n");

    }

}

