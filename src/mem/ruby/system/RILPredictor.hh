#ifndef __MEM_RUBY_SYSTEM_RILPREDICTOR_HH__
#define __MEM_RUBY_SYSTEM_RILPREDICTOR_HH__

#include <string>
#include <unordered_map>
#include <vector>

// #include "params/RILPredictor.hh"
#include "sim/sim_object.hh"
#include "base/types.hh"

class RILPredictor
{
  public:
    RILPredictor(int predictor_type);
    ~RILPredictor();

    bool predict(Addr address);

    // Check predicted value with actual value
    void update_predict(Addr address, bool taken);

  private:    
    const int predictor_type;
    const int hist_reg_size;

    //Assume 8 bit long history
    std::unordered_map<Addr, uint8_t> m_hist_reg_table;
    std::unordered_map<uint8_t, int> m_pattern_table;
  
  private:
    bool predict_LT(int state);
    bool predict_A1(int state);
    bool predict_A2(int state);
    bool predict_A3(int state);
    bool predict_A4(int state);
    
    void update_LT(uint8_t hist_reg, bool taken);
    void update_A1(uint8_t hist_reg, bool taken);
    void update_A2(uint8_t hist_reg, bool taken);
    void update_A3(uint8_t hist_reg, bool taken);
    void update_A4(uint8_t hist_reg, bool taken);
    
};


#endif // __MEM_RUBY_SYSTEM_RILPREDICTOR_HH__
