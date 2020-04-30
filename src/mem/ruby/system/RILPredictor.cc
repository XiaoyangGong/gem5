/*
Read-invalid-line predictor

Author: Xiaoyang Gong 
*/

#include "mem/ruby/system/RILPredictor.hh"
#include "sim/sim_object.hh"
#include "debug/RILPredictor.hh"

RILPredictor::RILPredictor(int predictor_type) :
predictor_type(predictor_type),
hist_reg_size(8)
{
    DPRINTF(RILPredictor, "Created the RIL Predictor\n");
}


bool
RILPredictor::predict(Addr address)
{
    if(predictor_type == 0)
        return true;

    // Find history register from m_hist_reg_table indexed by address
    auto hist_reg_ite = m_hist_reg_table.find(address);
    // If hist_reg found, get 8 bit history
    // else assign all 0 to hist_reg
    if(hist_reg_ite != m_hist_reg_table.end()){
        uint8_t hist_reg = hist_reg_ite -> second;

        // Using hist_reg as index/key, find state in pattern table
        auto state_ite = m_pattern_table.find(hist_reg);
        // If state is found, get T/NT from state variable
        if(state_ite != m_pattern_table.end()){
            int state = state_ite->second;
            if(predictor_type == 1)
                return predict_LT(state);
            else if(predictor_type == 2)
                return predict_A1(state);
            else if(predictor_type == 3)
                return predict_A2(state);
            else if(predictor_type == 4)
                return predict_A3(state);
            else if(predictor_type == 5)
                return predict_A4(state);
        }
        // if not found, set init state value to 0
        else{
            m_pattern_table[hist_reg] = 0;
            return false;
        }
    }
    // If hist_reg not found, init it to all 0
    // Try get state when pattern is all 0
    // If not, also init state value for pattern 8'b0 to 0
    else{
        m_hist_reg_table[address] = 0;
        auto state_ite = m_pattern_table.find(0);
        // If state is found, get T/NT from state variable
        if(state_ite != m_pattern_table.end()){
            int state = state_ite->second;
            if(predictor_type == 1)
                return predict_LT(state);
            else if(predictor_type == 2)
                return predict_A1(state);
            else if(predictor_type == 3)
                return predict_A2(state);
            else if(predictor_type == 4)
                return predict_A3(state);
            else if(predictor_type == 5)
                return predict_A4(state);
        }
        // if not found, set init state value to 0
        else{
            m_pattern_table[0] = 0;
            return false;
        }        

    }
    return false;
}

void
RILPredictor::update_predict(Addr address, bool actual_taken)
{
    if(predictor_type == 0)
        return;
    // First update hist_reg
    uint8_t hist_reg = m_hist_reg_table[address];
    uint8_t hist_reg_tmp = hist_reg;
    if(actual_taken)
        hist_reg_tmp = (hist_reg << 1) + (uint8_t)1;
    else 
        hist_reg_tmp = hist_reg << 1;
    m_hist_reg_table[address] = hist_reg_tmp;

    // Next update pattern table
    if(predictor_type == 1)
        update_LT(hist_reg, actual_taken);
    else if(predictor_type == 2)
        update_A1(hist_reg, actual_taken);
    else if(predictor_type == 3)
        update_A2(hist_reg, actual_taken);
    else if(predictor_type == 4)
        update_A3(hist_reg, actual_taken);
    else if(predictor_type == 5)
        update_A4(hist_reg, actual_taken);
    else{
        std::cout << "Undefined predictor_type!" << std::endl;
        exit(1);
    }

}


// Static predictors
// FSM reference: Page 3 https://dl.acm.org/doi/pdf/10.1145/123465.123475
// "Two-Level Adaptive Training Branch Prediction"
bool 
RILPredictor::predict_LT(int state){
    if(state == 0)
        return false;
    else 
        return true;
}

bool 
RILPredictor::predict_A1(int state){
    if(state == 0)
        return false;
    else if(state == 1) 
        return true;
    else if(state == 2) 
        return true;
    else 
        return true;
}

bool 
RILPredictor::predict_A2(int state){
    if(state == 0)
        return false;
    else if(state == 1) 
        return false;
    else if(state == 2) 
        return true;
    else 
        return true;
}

bool 
RILPredictor::predict_A3(int state){
    if(state == 0)
        return false;
    else if(state == 1) 
        return false;
    else if(state == 2) 
        return true;
    else 
        return true;
}

bool 
RILPredictor::predict_A4(int state){
    if(state == 0)
        return false;
    else if(state == 1) 
        return false;
    else if(state == 2) 
        return true;
    else 
        return true;
}

void 
RILPredictor::update_LT(uint8_t hist_reg, bool taken)
{
    int state = m_pattern_table[hist_reg];
    switch(state){
        case 0 :
        if(taken){
            m_pattern_table[hist_reg] = 1;
        }
        else{
            m_pattern_table[hist_reg] = 0;
        }
        
        break;

        case 1 :
        if(taken){
            m_pattern_table[hist_reg] = 1;
        }
        else{
            m_pattern_table[hist_reg] = 0;
        }
        break;

        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}



void 
RILPredictor::update_A1(uint8_t hist_reg, bool taken)
{
    int state = m_pattern_table[hist_reg];
    switch(state){
        case 0 :
        if(taken) 
            m_pattern_table[hist_reg] = 1;
        else 
            m_pattern_table[hist_reg] = 0;
        break;

        case 1 :
        if(taken) 
            m_pattern_table[hist_reg] = 3;
        else 
            m_pattern_table[hist_reg] = 2;
        break;

        case 2 :
        if(taken) 
            m_pattern_table[hist_reg] = 1;
        else 
            m_pattern_table[hist_reg] = 0;
        break;

        case 3 :
        if(taken) 
            m_pattern_table[hist_reg] = 3;
        else 
            m_pattern_table[hist_reg] = 2;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}


void 
RILPredictor::update_A2(uint8_t hist_reg, bool taken)
{
    int state = m_pattern_table[hist_reg];
    switch(state){
        case 0 :
        if(taken) 
            m_pattern_table[hist_reg] = 1;
        else 
            m_pattern_table[hist_reg] = 0;
        break;

        case 1 :
        if(taken) 
            m_pattern_table[hist_reg] = 2;
        else 
            m_pattern_table[hist_reg] = 0;
        break;

        case 2 :
        if(taken) 
            m_pattern_table[hist_reg] = 3;
        else 
            m_pattern_table[hist_reg] = 1;
        break;

        case 3 :
        if(taken) 
            m_pattern_table[hist_reg] = 3;
        else 
            m_pattern_table[hist_reg] = 2;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

void 
RILPredictor::update_A3(uint8_t hist_reg, bool taken)
{
    int state = m_pattern_table[hist_reg];
    switch(state){
        case 0 :
        if(taken) 
            m_pattern_table[hist_reg] = 1;
        else 
            m_pattern_table[hist_reg] = 0;
        break;

        case 1 :
        if(taken) 
            m_pattern_table[hist_reg] = 3;
        else 
            m_pattern_table[hist_reg] = 0;
        break;

        case 2 :
        if(taken) 
            m_pattern_table[hist_reg] = 3;
        else 
            m_pattern_table[hist_reg] = 0;
        break;

        case 3 :
        if(taken) 
            m_pattern_table[hist_reg] = 3;
        else 
            m_pattern_table[hist_reg] = 2;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

void 
RILPredictor::update_A4(uint8_t hist_reg, bool taken)
{
    int state = m_pattern_table[hist_reg];
    switch(state){
        case 0 :
        if(taken) 
            m_pattern_table[hist_reg] = 1;
        else 
            m_pattern_table[hist_reg] = 0;
        break;

        case 1 :
        if(taken) 
            m_pattern_table[hist_reg] = 3;
        else 
            m_pattern_table[hist_reg] = 0;
        break;

        case 2 :
        if(taken) 
            m_pattern_table[hist_reg] = 3;
        else 
            m_pattern_table[hist_reg] = 1;
        break;

        case 3 :
        if(taken) 
            m_pattern_table[hist_reg] = 3;
        else 
            m_pattern_table[hist_reg] = 2;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}



