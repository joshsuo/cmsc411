#include "schedulersim.hpp"
#include <cstdio>
#include <tuple>

int current_cycle = 1;

/**
 * Subroutine that returns how long it takes to complete a specific operation type
 *
 * @return                      Instruction latency in cycles
 */
int get_inst_latency(op_type op) {
    if(op == OP_ADD) {
        return 2;
    } else if(op == OP_DIV) {
        return 15;
    } else if(op == OP_MEM) {
        return 20;
    }
    std::printf("Invalid OP type:%d\n", op);
    std::exit(1);
}

/**
 * You are welcome to (re)define and set any global classes and variables as needed.
 */

int num_regs;

class RAT {
    // register alias table
    private:
        int* table;
    public:
        void init_table() {
            table = (int*) calloc(num_regs, sizeof(int));
            for (int i = 0; i < num_regs; i++) {
                table[i] = -1;
            }
        };;
        void free_table() {
            free(table);
        };
        void set_reg(int reg_num, int val) {
            table[reg_num - 1] = val;
        };
        int get_reg(int reg_num) {
            return table[reg_num - 1];
        };
};

RAT rat;

class REST {
    // reservation station
    private:
        int size;
        struct REST_Entry* table;
    public:
        void init_table(int table_size, int index_start) {
            size = table_size;
            table = (struct REST_Entry*) calloc(size, sizeof(struct REST_Entry));
            for (int i = 0; i < size; i++) {
                table[i].valid = false;
                table[i].rat_index = index_start++;
            }
        };
        void free_table() {
            free(table);
        };
        int get_size() {
            return size;
        };
        bool add_entry(op_type op, int dest, int src1, int src2, scheduler_stats_t* p_stats) {
            // put the instruction in the free entry of REST
            for (int i = 1; i <= size; i++) {
                if (!is_valid(i)) {
                    /*
                     * (2.1.) Add your code here
                     */ //done!

                    //initialize new entry
                    struct REST_Entry *entry = &(table[i-1]);

                    //initialize entry with given parameters
                    entry->op = op;
                    entry->dest = dest;
                    entry->src1 = src1;
                    entry->src2 = src2;

                    //update RAT
                    //set register num and update REST rat index
                    rat.set_reg(dest, entry->rat_index);

                    //initialize entry to valid and start cycle = 0 (not started)
                    entry->valid = true;
                    entry->start_cycle = 0;

                    //update num inst
                    p_stats->num_insts++;
                    return true;
                }
            }
            p_stats->issue_stall++;
            return false;
        };
        void clear_entry(int entry_num) {
            table[entry_num - 1].valid = false;
        };
        bool is_valid(int entry_num) {
            return table[entry_num - 1].valid;
        };
        bool is_empty() {
            // check if there are any valid entries in the tabele
            /*
             * (3.1.) Add your code here
             */ // done!

            // if valid entry, ret false
            for(int i = 1; i <= size; i++) {
                if((is_valid(i))) {
                    return false;
                }
            }

            return true;
        };
        int fire_ready() {
            int num_fired = 0;
            bool a_fired = false;
            bool d_fired = false;
            bool m_fired = false;
            for (int i = 1; i <= size; i++) {
                struct REST_Entry* e = &(table[i - 1]);
                // start executing (fire) any valid instruction in the table with ready sources
                // note: we marked the ready sources with -1
                /*
                 * (4.1.) Add your code here
                 */ //done!

                // start executing inst if entry is: 
                // valid && both operands (src1, src2) are ready && start cycle == 0 (not started yet)
                if(e->valid && e->src1 == -1 && e->src2 == -1 && e->start_cycle == 0) {
                    switch(e->op) {
                        // if inst is add
                        case OP_ADD:
                            // if flag is false, then change to true
                            if(a_fired == false) {
                                //update when this entry started exec and update fired inst
                                e->start_cycle = current_cycle;
                                num_fired++;
                                a_fired = true;
                            }
                        break;

                        //if inst is div
                        case OP_DIV:
                            // if flag is false, then change to true
                            if(d_fired == false) {
                                //update when this entry started exec and update fired inst
                                e->start_cycle = current_cycle;
                                num_fired++;
                                d_fired = true;
                            }
                        break;

                        // if inst is mem
                        case OP_MEM:
                            // if flag is false, then change to true
                            if(m_fired == false) {
                                //update when this entry started exec and update fired inst
                                e->start_cycle = current_cycle;
                                num_fired++;
                                m_fired = true;
                            }
                        break;

                        default:
                        break;
                    }
                }

            }
            return num_fired;
        };
        int complete_insts() {
            int num_completed = 0;
            for (int i = 1; i <= size; i++) {
                // for every instruction in the table check if its execution is done
                // if yes, then write the results back
                // feel free to call the broadcast_complete here
                /*
                 * (5.1.) Add your code here
                 */ //done!

                // select entry on ith element
                struct REST_Entry *entry = &(table[i-1]);
                
                // if entry is valid && start cycle != 0 (started inst)
                if(entry->valid && entry->start_cycle != 0) {
                    
                    //if inst is completed
                    if((current_cycle - get_inst_latency(entry->op)) == entry->start_cycle) {
                        // free RAT
                        rat.set_reg(entry->dest, -1);

                        //invalidate REST entry
                        clear_entry(i);

                        //write back results
                        broadcast_complete(entry->rat_index);

                        //update num complete inst
                        num_completed++;
                    }
                }

            }
            return num_completed;
        };
        int count_active() {
            // Only to be used for a REST specific to a single FU - see below for unified
            int num_active = 0;
            for (int i = 1; i <= size; i++) {
                if (table[i - 1].valid && (table[i - 1].start_cycle > 0)) {
                    num_active++;
                }
            }
            return num_active;
        };
        std::tuple<int, int, int> count_active_u() {
            // To be used for a unified REST - see above for "per FU" REST
            int num_active_a = 0;
            int num_active_d = 0;
            int num_active_m = 0;
            for (int i = 1; i <= size; i++) {
                if (table[i - 1].valid && (table[i - 1].start_cycle > 0)) {
                    switch (table[i - 1].op) {
                        case OP_ADD:
                            num_active_a++;
                            break;
                        case OP_DIV:
                            num_active_d++;
                            break;
                        case OP_MEM:
                            num_active_m++;
                            break;
                        default:
                            printf("Error: Invalid op_type: %d", table[i - 1].op);
                            break;
                    }
                }
            }
            return std::make_tuple(num_active_a, num_active_d, num_active_m);
        }
        void update_sources(int rat_index) {
            for (int i = 1; i <= size; i++) {
                struct REST_Entry* e = &(table[i - 1]);
                if (e->valid) {
                    // we mark the match sources with -1
                    if (e->src1 == rat_index) {
                        e->src1 = -1;
                    }
                    if (e->src2 == rat_index) {
                        e->src2 = -1;
                    }
                }
            }
        }
};

// Reservation stations - unified, add, div, and mem
// Note some of these will be unused depending on unified vs per FU
REST urest;
REST arest;
REST drest;
REST mrest;

rs_type rs;

void broadcast_complete(int rat_index) {
    if (rs == 'U') {
        urest.update_sources(rat_index);
    } else {
        arest.update_sources(rat_index);
        drest.update_sources(rat_index);
        mrest.update_sources(rat_index);
    }
}


/**
 * Subroutine for initializing the scheduler (unified reservation station type).
 * You may initalize any global or heap variables as needed.
 *
 * @param[in]   num_registers   The number of registers in the instructions
 * @param[in]   rs_size         The number of entries for the unified RS
 */
void scheduler_unified_init(int num_registers, int rs_size) {
    num_regs = num_registers;
    rat.init_table();
    rs = RSTYPE_UNIFIED;
    urest.init_table(rs_size, 1);
}

/**
 * Subroutine for initializing the scheduler (per-functional unit reservation station type).
 * You may initalize any global or heap variables as needed.
 *
 * (1) You're responsible for completing this routine
 *
 * @param[in]   num_registers   The number of registers in the instructions
 * @param[in]   rs_sizes        An array of size 3 that contains the number of entries for each
 *                              op_type
 *                              rs_sizes = [4,2,1] means 4 ADD RS, 2 DIV RS, 1 MEM RS
 */
void scheduler_per_fu_init(int num_registers, int rs_sizes[]) {
  /*
   * (1.1.) Add your code here
   */ //done!
    
    //initialize scheduler

    //update global num_regs var = num of resisters
    num_regs = num_registers;

    //initialize table with num_regs
    rat.init_table();

    // update reservation station type to per fu, not unified
    rs = RSTYPE_PER_FU;

    // initialize table for each inst and how many spaces they need
    arest.init_table(rs_sizes[0], 1);
    drest.init_table(rs_sizes[1], rs_sizes[0] + 1);
    mrest.init_table(rs_sizes[2], rs_sizes[0] + rs_sizes[1] + 1);

}

/**
 * Subroutine that tries to issue an instruction to the reservation station. You need to
 * choose the appropriate RS depending on the RS type and op_type and update it.
 *
 * (2) You're responsible for completing this routine and
 * the add_entry routine marked with (2.1.) above
 *
 * @param[in]   op_type         The FU to use
 * @param[in]   dest            The destination register
 * @param[in]   src1            The first source register (-1 if unused)
 * @param[in]   src2            The seconde source register (-1 if unused)
 * @param[out]  p_stats         Pointer to the stats structure
 *
 * @return                      true if successful, false if we failed
 */
bool scheduler_try_issue(op_type op, int dest, int src1, int src2, scheduler_stats_t* p_stats) {
    int alias1 = src1;
    if (src1 != -1) {
        alias1 = rat.get_reg(src1);
    }
    int alias2 = src2;
    if (src2 != -1) {
        alias2 = rat.get_reg(src2);
    }
    if (rs == 'U') {
      return urest.add_entry(op, dest, alias1, alias2, p_stats);
    } else {
      /*
       * (2.2.) Add your code here
       */ // done!

    // if rs == 'FU'
        
        // switch based on operation
        switch(op) {
            //if op == add
            case OP_ADD:
                //return add entry with given parameters
                return arest.add_entry(op, dest, alias1, alias2, p_stats);
            //if op == div
            case OP_DIV:
                //return div entry with given parameters
                return drest.add_entry(op, dest, alias1, alias2, p_stats);
            //if op == mem
            case OP_MEM:
                //return mem entry with given parameters
                return mrest.add_entry(op, dest, alias1, alias2, p_stats);

            default:
            break;
        }
        
    }
    return false;
}

/**
 * Subroutine that checks if all instructions have been drained from the pipeline
 * (3) You're responsible for completing this routine and
 * the is_empty routine marked with (3.1.) above
 *
 * @return                      true if no instructions are left
 */
bool scheduler_completed() {
    if (rs == 'U') {
      return urest.is_empty();
    } else {
      /*
       * (3.2.) Add your code here
       */ //done!

    // if rs == 'FU'
    
        // return true if a,d,m REST are all empty
        return arest.is_empty() && drest.is_empty() && mrest.is_empty();

    }
    return false;
}

/**
 * Subroutine that increments the clock cycle and updates any system state
 *
 * @param[out]  p_stats         Pointer to the stats structure
 */
void scheduler_step(scheduler_stats_t* p_stats) {
    current_cycle++;
    scheduler_clear_completed(p_stats);
    scheduler_start_ready(p_stats);
}

/**
 * Subroutine for firing (start executing) any ready instructions.
 *
 * (4) You're responsible for completing this routine and
 * the fire_ready routine marked with (4.1.) above
 *
 * @param[out]  p_stats         Pointer to the stats structure
 */
void scheduler_start_ready(scheduler_stats_t* p_stats) {
    unsigned int num_fired = 0;
    if (rs == 'U') {
      num_fired += urest.fire_ready();
    } else {
      /*
       * (4.2.) Add your code here
       */ //done!

    //  if rs == 'FU'

        // update num fired for each op based on num of fired inst
        num_fired += arest.fire_ready();
        num_fired += drest.fire_ready();
        num_fired += mrest.fire_ready();

    }
    // Update p_stats with number of fired instructions
    /*
     * (4.3.) Add your code here
     */ // done!

    //if num fired > max fired
    if(num_fired > (p_stats->max_fired)) {
        //max fired = num fired
        p_stats->max_fired = num_fired;
    }

    // Count num active per FU
    unsigned int num_active_a;
    unsigned int num_active_d;
    unsigned int num_active_m;

    if (rs == 'U') {
        auto t = urest.count_active_u();
        num_active_a = std::get<0>(t);
        num_active_d = std::get<1>(t);
        num_active_m = std::get<2>(t);
    } else {
      num_active_a = arest.count_active();
      num_active_d = drest.count_active();
      num_active_m = mrest.count_active();
    }

    // Update pstats with number of active FUs
    /*
     * (4.4.) Add your code here
     */ //done!

    // if num active > max active for op add
    if(num_active_a > (p_stats->max_active[0])) {
        // max active = num active for op add
        p_stats->max_active[0] = num_active_a;
    }
    // if num active > max active for op div
    if(num_active_d > (p_stats->max_active[1])) {
        // max active = num active for op div
        p_stats->max_active[1] = num_active_d;
    }
    // if num active > max active for op mem
    if(num_active_m > (p_stats->max_active[2])) {
        // max active = num active for op mem
        p_stats->max_active[2] = num_active_m;
    }

}

/**
 * Subroutine for clearing any completed instructions.
 * (5) You're responsible for completing this routine and
 * the complete_insts routine marked with (5.1.) above
 *
 * @param[out]  p_stats         Pointer to the stats structure
 */
void scheduler_clear_completed(scheduler_stats_t* p_stats) {
    unsigned int num_completed = 0;

    if (rs == 'U') {
      num_completed += urest.complete_insts();
    } else {
      /*
       * (5.2.) Add your code here
       */ // done!

    //if rs == 'FU'

        //update num completed with each num of completed inst from from op
        num_completed += arest.complete_insts();
        num_completed += drest.complete_insts();
        num_completed += mrest.complete_insts();

    }

    // Update p_stats with number of completed instructions
    /*
     * (5.3.) Add your code here
     */ //done!

    //if num complete > max completed
    if(num_completed > p_stats->max_completed) {
        // max completed = num completed
        p_stats->max_completed = num_completed;
    }

}

/**
 * Subroutine for completing the scheduler and getting any final stats
 *
 * @param[out]  p_stats         Pointer to the stats structure
 */
void scheduler_complete(scheduler_stats_t* p_stats) {
    // Free memory
    if (rs == 'U') {
        urest.free_table();
    } else {
        arest.free_table();
        drest.free_table();
        mrest.free_table();
    }
    rat.free_table();

    // Update number of cycles and ipc in p_stats
    p_stats->num_cycles = current_cycle;
    p_stats->ipc = (double) p_stats->num_insts / p_stats->num_cycles;
}
