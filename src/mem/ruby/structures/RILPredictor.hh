#ifndef __MEM_RUBY_STRUCTURES_RILPREDICTOR_HH__
#define __MEM_RUBY_STRUCTURES_RILPREDICTOR_HH__

#include <string>
#include <unordered_map>
#include <vector>

// #include "params/RILPredictor.hh"
#include "sim/sim_object.hh"
#include "base/types.hh"

class RILPredictor : public SimObject
{
  public:
    RILPredictor(string predictor_type);
    ~RILPredictor();

    bool predict(Addr address);

    // Check predicted value with actual value
    void update_predict(bool taken);

  private:
    const string predictor_type;
    const int local_hist_size;
    const int global_hist_size; 
    int state;
    bool predict;
    // Local history buffer. Record history of T/TN for an address
    std::unordered_map<Addr, std::list<bool>> m_local_hist;

    // Global history buffer.
    std::std::list<bool> m_global_hist;
  
  private:
    bool predict_LT(bool taken);
    bool predict_A1(bool taken);
    bool predict_A2(bool taken);
    bool predict_A3(bool taken);
    bool predict_A4(bool taken);
};

std::ostream& operator<<(std::ostream& out, const CacheMemory& obj);

#endif // __MEM_RUBY_STRUCTURES_RILPREDICTOR_HH__
