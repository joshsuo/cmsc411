#include "MESI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MESI_protocol::MESI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
  // Initialize lines to not have data yet
  this->state = MESI_CACHE_I;
}

MESI_protocol::~MESI_protocol ()
{
}

void MESI_protocol::dump (void)
{
    const char *block_states[8] = {"X","I","S","E","M", "IM", "IS", "IE"};
    fprintf (stderr, "MESI_protocol - state: %s\n", block_states[state]);
}

void MESI_protocol::process_cache_request (Mreq *request)
{
	switch (state) {
    case MESI_CACHE_I:   do_cache_I (request); break;
    case MESI_CACHE_S:   do_cache_S (request); break;
    case MESI_CACHE_E:   do_cache_E (request); break;
    case MESI_CACHE_M:   do_cache_M (request); break;
    case MESI_CACHE_IM:  do_cache_IM (request); break;
    case MESI_CACHE_ISE: do_cache_ISE (request); break;
    case MESI_CACHE_SM:  do_cache_SM (request); break;
    default:
        fatal_error ("Invalid Cache State for MESI Protocol\n");
    }
}

void MESI_protocol::process_snoop_request (Mreq *request)
{
	switch (state) {
    case MESI_CACHE_I:   do_snoop_I (request); break;
    case MESI_CACHE_S:   do_snoop_S (request); break;
    case MESI_CACHE_E:   do_snoop_E (request); break;
    case MESI_CACHE_M:   do_snoop_M (request); break;
    case MESI_CACHE_IM:  do_snoop_IM (request); break;
    case MESI_CACHE_ISE: do_snoop_ISE (request); break;
    case MESI_CACHE_SM:  do_snoop_SM (request); break;

    default:
    	fatal_error ("Invalid Cache State for MESI Protocol\n");
    }
}

inline void MESI_protocol::do_cache_I (Mreq *request)
{
  switch (request->msg) {
    case LOAD:
      // (1.1.) Add your code here
      // req to share
      send_GETS(request->addr);
      // state change to ISE
      state = MESI_CACHE_ISE;
      Sim->cache_misses++; // cache miss
      break;
    case STORE:
      // (1.2.) Add your code here
      // req to modify
      send_GETM(request->addr);
      // state change to IM
      state = MESI_CACHE_IM;
      Sim->cache_misses++; // cache miss
      break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: I state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_cache_S (Mreq *request)
{
  switch (request->msg) {
    case LOAD:
      // (2.1.) Add your code here
      // send to process
      send_DATA_to_proc(request->addr);
      break;
    case STORE:
      // (2.2.) Add your code here
      // req to modify
      send_GETM(request->addr);
      //state change to SM
      state = MESI_CACHE_SM;
      Sim->cache_misses++; // cache miss
      break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: S state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_cache_E (Mreq *request)
{
  switch (request->msg) {
    case LOAD:
      // (3.1.) Add your code here
      // send to process
      send_DATA_to_proc(request->addr);
      break;
    case STORE:
      // (3.2.) Add your code here
      // send to process
      send_DATA_to_proc(request->addr);
      // state change to M
      state = MESI_CACHE_M;
      Sim->silent_upgrades++; // silent ups
      break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: S state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_cache_M (Mreq *request)
{
  switch (request->msg) {
    case LOAD:
    case STORE:
      // (4.) Add your code here
      // send to process
      send_DATA_to_proc(request->addr);
      break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: M state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_cache_IM (Mreq *request)
{
  switch (request->msg) {
    case LOAD:
    case STORE:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error("Should only have one outstanding request per processor!");
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: IM state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_cache_ISE (Mreq *request)
{
  switch (request->msg) {
    case LOAD:
    case STORE:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error("Should only have one outstanding request per processor!");
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: IS state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_cache_SM (Mreq *request)
{
  switch (request->msg) {
    case LOAD:
    case STORE:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error("Should only have one outstanding request per processor!");
    break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: I state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_snoop_I (Mreq *request)
{
  switch (request->msg) {
    case GETS:
    case GETM:
    case DATA:
      break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: I state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_snoop_S (Mreq *request)
{
  switch (request->msg) {
    case GETS:
      // (5.1.) Add your code here
      set_shared_line();
      break;
    case GETM:
      // (5.2.) Add your code here
      set_shared_line();
      // state change to I
      state = MESI_CACHE_I;
      break;
    case DATA:
      fatal_error ("Should not see data for this line!  I have the line!");
      break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: S state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_snoop_E (Mreq *request)
{
  switch (request->msg) {
    case GETS:
      // (6.1.) Add your code here
      set_shared_line();
      // send on bus
      send_DATA_on_bus(request->addr, request->src_mid);
      // state change to S
      state = MESI_CACHE_S;
      break;
    case GETM:
      // (6.2.) Add your code here
      // send on bus
      send_DATA_on_bus(request->addr, request->src_mid);
      set_shared_line();
      // state change to I
      state = MESI_CACHE_I;
      break;
    case DATA:
      fatal_error ("Should not see data for this line!  I have the line!");
      break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: M state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_snoop_M (Mreq *request)
{
  switch (request->msg) {
    case GETS:
      // (7.1.) Add your code here
      set_shared_line();
      // send to bus
      send_DATA_on_bus(request->addr, request->src_mid);
      // state change to S
      state = MESI_CACHE_S;
      break;
    case GETM:
      // (7.2.) Add your code here
      set_shared_line();
      // send on bus
      send_DATA_on_bus(request->addr, request->src_mid);
      // state change to I
      state = MESI_CACHE_I;
      break;
    case DATA:
      fatal_error ("Should not see data for this line!  I have the line!");
      break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: M state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_snoop_IM (Mreq *request)
{
  switch (request->msg) {
    case GETS:
    case GETM:
      break;
    case DATA:
      // (8.) Add your code here
      // send to process
      send_DATA_to_proc(request->addr);
      // state change to M
      state = MESI_CACHE_M;
    break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: I state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_snoop_ISE (Mreq *request)
{
  switch (request->msg) {
    case GETS:
    case GETM:
      break;
    case DATA:
      // (9.) Add your code here
      if(get_shared_line()){ // if share line
        set_shared_line();
        // send to process
        send_DATA_to_proc(request->addr);
        // state change to S
        state = MESI_CACHE_S;
      }else{
        // send to process
        send_DATA_to_proc(request->addr);
        // state change to E
        state = MESI_CACHE_E;
      }
      break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: IS state shouldn't see this message\n");
  }
}

inline void MESI_protocol::do_snoop_SM (Mreq *request)
{
  switch (request->msg) {
    case GETS:
      // (10.1.) Add your code here
      set_shared_line();
      break;
    case GETM:
      // (10.2.) Add your code here
      set_shared_line();
      break;
    case DATA:
      // (10.3.) Add your code here
      // send to process
      send_DATA_to_proc(request->addr);
      // state change to M
      state = MESI_CACHE_M;
      break;
    default:
      request->print_msg (my_table->moduleID, "ERROR");
      fatal_error ("Client: SM state shouldn't see this message\n");
  }
}
