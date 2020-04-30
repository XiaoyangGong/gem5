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
    void update_predict(bool taken);

  private:
    const int predictor_type;
    const int local_hist_size;
    const int global_hist_size; 
    int state;
    bool curr_predict;
    // Local history buffer. Record history of T/TN for an address
    std::unordered_map<Addr, std::list<bool>> m_local_hist;

    // Global history buffer.
    std::list<bool> m_global_hist;
  
  private:
    void predict_LT(bool taken);
    void predict_A1(bool taken);
    void predict_A2(bool taken);
    void predict_A3(bool taken);
    void predict_A4(bool taken);
};


#endif // __MEM_RUBY_SYSTEM_RILPREDICTOR_HH__
