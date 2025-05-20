#include "branchsim.hpp"

/**
 * A set of global variables
 * You may define and set any global classes and variables here as needed.
 */
static unsigned int* pht;
static std::uint64_t ghr;
static unsigned int ctr_max;
static unsigned int ctr_weakly_taken;
static std::uint64_t index_mask;
static std::uint64_t  history_shift;
static std::uint64_t history_mask;
static predictor_type pred_type;
static std::uint64_t* hist_table;

/**
 * This subroutine initializes the branch predictors.
 * You many add and initialize any global or heap
 * variables as needed.
 * (1) You're responsible for completing this routine
 *
 * Inputs and outputs:
 * @param[in]   ptype       The type of branch predictor to simulate
 * @param[in]   num_entries The number of entries a PC is hashed into
 * @param[in]   counter_bits The number of bits per counter
 * @param[in]   history_bits The number of bits per history
 * @param[out]  p_stats     Pointer to the stats structure
 */
void setup_predictor(predictor_type ptype,
                     int num_entries,
                     int counter_bits,
                     int history_bits,
                     branch_stats_t* p_stats) {
  ctr_max = (unsigned int)(1 << counter_bits) - 1;
  ctr_weakly_taken = (unsigned int) (1 << (counter_bits - 1));

  index_mask = (std::uint64_t)num_entries - 1;
  history_mask = (std::uint64_t)(1 << history_bits) - 1;
  history_shift = history_bits;

  int pht_size = num_entries;
  pred_type = ptype;
  /*
   * For each type of branch prediction, complete the following switch
   * to define pht_size and p_stats based on num_entries, counter_bits, and history_bits.
   * The code for Bimodal branch predictor is provided as an example.
   */
  switch (ptype) {
    case PTYPE_BIMODAL: {
      // given code
      // pht size = entries
      pht_size = num_entries;

      // total pht = pht size * cnt bits
      p_stats->storage_overhead = ((std::uint64_t)pht_size) * ((std::uint64_t)counter_bits);
      break;
    }
    case PTYPE_TWO_LEVEL_ADAPTIVE: {
      /*
       * 1.1. Add your code here
       */

      // size of PHT is: 2^N x cnt bits
      // pht size = 2^hist bits * cnt bits
      pht_size = (1<<(history_bits)) * counter_bits;

      // total size = entries + hist bits + pht size
      // Total size = 2^K x N + 2^N * cnt bits
      p_stats->storage_overhead = (((std::uint64_t)history_bits) * ((std::uint64_t)num_entries)) + ((std::uint64_t)pht_size);
      break;
    }
    case PTYPE_LOCAL_HISTORY: {
      /*
       * 1.2. Add your code here
       */

      // size of one PHT is: 2^N x counter bits
      // pht size = 2^hist bits * cnt bits * entries
      pht_size = (1<<history_bits) *  counter_bits * num_entries;

      // total size = hist bits * num entries + pht size
      // Total size: 2^K * N + 2^N * cnt bits * 2^K
      p_stats->storage_overhead = (((std::uint64_t)history_bits) * ((std::uint64_t)num_entries)) + ((std::uint64_t)pht_size);
      break;
    }
    case PTYPE_GSHARE: {
      /*
       * 1.3. Add your code here
       */

      // pht size = num entries * cnt bits
      pht_size = num_entries * counter_bits;

      // total size = num entries * counter bits + hist bits
      p_stats->storage_overhead = (((std::uint64_t)pht_size) + ((std::uint64_t)history_bits));;
      break;
    }
  }

  /*
   * We initial the pht, ghr, and hist_table in the following:
   */
  pht = new unsigned int[pht_size];
  for (int i = 0; i < pht_size; i++) {
    pht[i] = ctr_weakly_taken;
  }
  ghr = 0;
  if (pred_type == PTYPE_LOCAL_HISTORY || pred_type == PTYPE_TWO_LEVEL_ADAPTIVE) {
    hist_table = new std::uint64_t[num_entries];
    for (int i = 0; i < num_entries; i++) {
      hist_table[i] = 0;
    }
  }
}

/**
 * This subroutine calculates the index for accessing the pht
 * (2) You're responsible for completing this routine
 *
 * Inputs and outputs:
 * @param[in]   pc       The program counter
 * @param[out]  The index for accessing the pht
 * The code for bimodal branch predictor is provided as an example.
 */
std::uint64_t get_index(std::uint64_t pc) {
  /*
   * For each type of branch prediction, complete the following switch
   * to calculate the index. You may combine pc with index_mask and use
   * history_shift, hist_table, and history_mask in your calculation.
   * The code for the Local History branch predictor is provided as an example.
   */
  switch (pred_type) {
    case PTYPE_BIMODAL: {
      /*
       * 2.1. Add your code here
       */ 

      // index = program counter & index mask
      return pc & index_mask;
    }
    case PTYPE_TWO_LEVEL_ADAPTIVE: {
      /*
       * 2.2. Add your code here
       */

      // find hist index = pc & index mask
      std::uint64_t hist_index = pc & index_mask;

      // index found by getting offset in pht table by doing:
      // index = hist table index & hist mask
      return (hist_table[hist_index] & history_mask);
    }
    case PTYPE_LOCAL_HISTORY: {
      // given code

      // find hist index = pc & index mask
      std::uint64_t hist_index = pc & index_mask;

      // index found by get hist index (above) and mult with 2^hist bits to get corresponding pht table index
      // then add hist table[hist index] & hist mask to get offset in pht table
      // index = hist index * 2^hist bits + hist table[hist index] & hist mask
      return (hist_index * (1 << history_shift)) + (hist_table[hist_index] & history_mask);
    }
    case PTYPE_GSHARE: {
      /*
       * 2.3. Add your code here
       */

      // Generate PHT index by xor-ing branch pc with global history
      // branch pc = pc & index mask
      // global hist = ghr & index mask
      // index = pc & index mask ^ ghr & index mask
      return (pc & index_mask) ^ (ghr & index_mask);
    }
    default:
      return 0;
  }
}

/**
 * This subroutine run the branch prediction for a PC, returns either TAKEN or
 * NOT_TAKEN and accordingly increaments the pred_taken or pred_not_taken++.
 * (3) You're responsible for completing this routine
 *
 * @param[in]   pc          The PC value of the branch instruction.
 * @param[out]  p_stats     Pointer to the stats structure
 *
 * @return                  Either TAKEN ('T'), or NOT_TAKEN ('N')
 */
branch_dir predict_branch(std::uint64_t pc, branch_stats_t* p_stats) {
  // Increment branch count
  p_stats->num_branches++;

  // Identify index
  std::uint64_t index = get_index(pc);

  // Predict the branch by accessing the pht
  /*
   * 3.1. Add your code here
   * The following code (i.e., return TAKEN) is added just as an example to return TAKEN by default.
   */

  // if pht[index] >= ctr_weakly_taken, then increment prediction taken
  if(pht[index] >= ctr_weakly_taken) {
    p_stats->pred_taken++;
    return TAKEN;
  }else{ // else increment prediction not taken
    p_stats->pred_not_taken++;
    return NOT_TAKEN;
  }

}

/**
 * This subroutine updates the branch predictor.
 * The branch predictor needs to be notified whether
 * the prediction was right or wrong.
 * To do so, update pht, ghr, hist_table, and any other variables as needed.
 * (4) You're responsible for completing this routine
 *
 * @param[in]   pc          The PC value of the branch instruction.
 * @param[in]   actual      The actual direction of the branch
 * @param[in]   predicted   The predicted direction of the branch (from predict_branch)
 * @param[out]  p_stats     Pointer to the stats structure
 */
void update_predictor(std::uint64_t pc,
                      branch_dir actual,
                      branch_dir predicted,
                      branch_stats_t* p_stats) {
  std::uint64_t index = get_index(pc);

  if (actual == predicted) {
    p_stats->correct++;
  }

  if (actual == TAKEN) {
    ghr = (ghr << 1) | 1;
    /*
     * 4.1. Add your code here
     */

    // update pht[index] to increment if < strongly taken
    if(pht[index] < (2<<(ctr_weakly_taken-1)) - 1) pht[index]++;

    // if pred is two level adaptive OR local hist
    if(pred_type == PTYPE_TWO_LEVEL_ADAPTIVE || pred_type == PTYPE_LOCAL_HISTORY) { 
      // update hist table by shift hist bits << 1 and + 1 for taken
      std::uint64_t hist_index = pc & index_mask;
      hist_table[hist_index] = (hist_table[hist_index]<<1) + 1;

    }
  } else if (actual == NOT_TAKEN) {
    ghr = (ghr << 1) | 0;
    /*
     * 4.2. Add your code here
     */

    // update pht[index] to decrement if > 0
    if(pht[index] > 0) pht[index]--;

    // if pred if two level adaptive || local history
    if (pred_type == PTYPE_TWO_LEVEL_ADAPTIVE || pred_type == PTYPE_LOCAL_HISTORY) {
      // update hist table by shift hist bits << 1 and + 0 (nothing) for not taken
      std::uint64_t hist_index = pc & index_mask;
      hist_table[hist_index] = (hist_table[hist_index]<<1);

    }
  }
}

/**
 * This subroutine cleans up any outstanding memory operations and calculating overall statistics.
 *
 * @param[out]  p_stats     Pointer to the statistics structure
 */
void complete_predictor(branch_stats_t *p_stats) {
  p_stats->misprediction_rate = (double)(p_stats->num_branches - p_stats->correct) / (double)p_stats->num_branches;
  delete[] pht;
  if (pred_type == PTYPE_LOCAL_HISTORY) {
    delete[] hist_table;
  }
}
