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
    const int local_hist_size;
    const int global_hist_size; 
    //int state;
    std::unordered_map<Addr, int> curr_predict_map;
    // Local history buffer. Record history of T/TN for an address
    std::unordered_map<Addr, std::list<bool>> m_local_hist;

    // Global history buffer.
    std::list<bool> m_global_hist;
  
  private:
    bool predict_LT(int state);
    bool predict_A1(int state);
    bool predict_A2(int state);
    bool predict_A3(int state);
    bool predict_A4(int state);
    
    void update_LT(Addr address, bool taken);
    void update_A1(Addr address, bool taken);
    void update_A2(Addr address, bool taken);
    void update_A3(Addr address, bool taken);
    void update_A4(Addr address, bool taken);
    
};


#endif // __MEM_RUBY_SYSTEM_RILPREDICTOR_HH__
