#include "MSI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MSI_protocol::MSI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
    this->state = MSI_CACHE_I;
}

MSI_protocol::~MSI_protocol ()
{
}

void MSI_protocol::dump (void)
{
    const char *block_states[6] = {"X","I","IS","S","M","PM"};
    fprintf (stderr, "MSI_protocol - state: %s\n", block_states[state]);
}

void MSI_protocol::process_cache_request (Mreq *request)
{
	switch (state) {
	    case MSI_CACHE_I:
            do_cache_I(request);
            break;
	    case MSI_CACHE_IS:
            do_cache_IS(request);
            break;
        case MSI_CACHE_S:
            do_cache_S(request);
            break;
	    case MSI_CACHE_M:
            do_cache_M(request);
            break;
        case MSI_CACHE_PM:
            do_cache_PM(request);
            break;

    default:
        fatal_error ("Invalid Cache State for MSI Protocol\n");
    }
}

void MSI_protocol::process_snoop_request (Mreq *request)
{
	switch (state) {
        case MSI_CACHE_I:
            do_snoop_I(request);
            break;
        case MSI_CACHE_IS:
            do_snoop_IS(request);
            break;
        case MSI_CACHE_S:
            do_snoop_S(request);
            break;
        case MSI_CACHE_M:
            do_snoop_M(request);
            break;
        case MSI_CACHE_PM:
            do_snoop_PM(request);
            break;

    default:
    	fatal_error ("Invalid Cache State for MSI Protocol\n");
    }
}

inline void MSI_protocol::do_cache_I (Mreq *request)
{

    switch(request->msg) {
        case LOAD:
            send_GETS(request->addr);
            state = MSI_CACHE_IS;
            Sim->cache_misses++;
            break;
        case STORE:
            send_GETM(request->addr);
            state = MSI_CACHE_PM;
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_cache_IS (Mreq *request)
{

    switch (request->msg) {
        /* If the block is in the IM state that means it sent out a GET message
         * and is waiting on DATA.  Therefore the processor should be waiting
         * on a pending request. Therefore we should not be getting any requests from
         * the processor.
         */
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Should only have one outstanding request per processor!");
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: IS state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_cache_S (Mreq *request)
{

    switch(request->msg) {
        case LOAD:
            send_DATA_to_proc(request->addr);
            break;
        case STORE:
            send_GETM(request->addr);
            Sim->cache_misses++;
            state = MSI_CACHE_PM;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: S state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_cache_M (Mreq *request)
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

inline void MSI_protocol::do_cache_PM (Mreq *request)
{
    switch (request->msg) {
        /* If the block is in the IM state that means it sent out a GET message
         * and is waiting on DATA.  Therefore the processor should be waiting
         * on a pending request. Therefore we should not be getting any requests from
         * the processor.
         */
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Should only have one outstanding request per processor!");
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: PM state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_snoop_I (Mreq *request)
{
    switch (request->msg) {
        case GETS:
        case GETM:
        case DATA:
            /**
             * If we snoop a message from another cache and we are in I, then we don't
             * need to do anything!  We obviously cannot supply data since we don't have
             * it, and we don't need to downgrade our state since we are already in I.
             */
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_snoop_IS (Mreq *request)
{
    switch (request->msg) {
        case DATA:
            /**
             * If we snoop a message from another cache and we are in I, then we don't
             * need to do anything!  We obviously cannot supply data since we don't have
             * it, and we don't need to downgrade our state since we are already in I.
             */
            send_DATA_to_proc(request->addr);
            state = MSI_CACHE_S;
            if(get_shared_line()) {

            }
            break;
        case GETS:
        case GETM:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: IS state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_snoop_S (Mreq *request)
{
    switch(request->msg) {
        case GETS:
        case DATA:
            break;
        case GETM:
            state = MSI_CACHE_I;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: S state shouldn't see this message\n");
    }

}

inline void MSI_protocol::do_snoop_M (Mreq *request)
{
    switch(request->msg) {
        case GETS:
            set_shared_line();
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MSI_CACHE_S;
            break;
        case GETM:
            set_shared_line();
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MSI_CACHE_I;
            break;
        case DATA:
            fatal_error ("STATE SNOOP M: Should not see data for this line!  I have the line!");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: M state shouldn't see this message\n");
    }

}

inline void MSI_protocol::do_snoop_PM (Mreq *request)
{
    switch(request->msg) {
        case GETM:
        case GETS:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state = MSI_CACHE_M;
            if(get_shared_line()) {

            }
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Snoop: PM state shouldn't see this message\n");

    }

}
