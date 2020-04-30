/*
Read-invalid-line predictor

Author: Xiaoyang Gong 
*/

#include "mem/ruby/system/RILPredictor.hh"
#include "sim/sim_object.hh"
#include "debug/RILPredictor.hh"

RILPredictor::RILPredictor(int predictor_type) :
predictor_type(predictor_type),
local_hist_size(128),
global_hist_size(128)
//state(0)
{
    DPRINTF(RILPredictor, "Created the RIL Predictor\n");
}


bool
RILPredictor::predict(Addr address)
{
    if(predictor_type == 0)
        return true;
    auto curr_predict = curr_predict_map.find(address);
    // if found, return predict value
    // else return true by default
    if(curr_predict != curr_predict_map.end()){
        int state = curr_predict->second;
        // Get T/NT from state. Dependent on type of FSM used
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
    else{
        // Create initial state 0
        curr_predict_map[address] = 0;
        return false;
    }
    return false;
}

void
RILPredictor::update_predict(Addr address, bool actual_taken)
{
    if(predictor_type == 0)
        return;
    else if(predictor_type == 1)
        update_LT(address, actual_taken);
    else if(predictor_type == 2)
        update_A1(address, actual_taken);
    else if(predictor_type == 3)
        update_A2(address, actual_taken);
    else if(predictor_type == 4)
        update_A3(address, actual_taken);
    else if(predictor_type == 5)
        update_A4(address, actual_taken);
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
RILPredictor::update_LT(Addr address, bool taken)
{
    int state = curr_predict_map[address];
    switch(state){
        case 0 :
        if(taken){
            curr_predict_map[address] = 1;
        }
        else{
            curr_predict_map[address] = 0;
        }
        
        break;

        case 1 :
        if(taken){
            curr_predict_map[address] = 1;
        }
        else{
            curr_predict_map[address] = 0;
        }
        break;

        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}



void 
RILPredictor::update_A1(Addr address, bool taken)
{
    int state = curr_predict_map[address];
    switch(state){
        case 0 :
        if(taken) 
            curr_predict_map[address] = 1;
        else 
            curr_predict_map[address] = 0;
        break;

        case 1 :
        if(taken) 
            curr_predict_map[address] = 3;
        else 
            curr_predict_map[address] = 2;
        break;

        case 2 :
        if(taken) 
            curr_predict_map[address] = 1;
        else 
            curr_predict_map[address] = 0;
        break;

        case 3 :
        if(taken) 
            curr_predict_map[address] = 3;
        else 
            curr_predict_map[address] = 2;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}


void 
RILPredictor::update_A2(Addr address, bool taken)
{
    int state = curr_predict_map[address];
    switch(state){
        case 0 :
        if(taken) 
            curr_predict_map[address] = 1;
        else 
            curr_predict_map[address] = 0;
        break;

        case 1 :
        if(taken) 
            curr_predict_map[address] = 2;
        else 
            curr_predict_map[address] = 0;
        break;

        case 2 :
        if(taken) 
            curr_predict_map[address] = 3;
        else 
            curr_predict_map[address] = 1;
        break;

        case 3 :
        if(taken) 
            curr_predict_map[address] = 3;
        else 
            curr_predict_map[address] = 2;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

void 
RILPredictor::update_A3(Addr address, bool taken)
{
    int state = curr_predict_map[address];
    switch(state){
        case 0 :
        if(taken) 
            curr_predict_map[address] = 1;
        else 
            curr_predict_map[address] = 0;
        break;

        case 1 :
        if(taken) 
            curr_predict_map[address] = 3;
        else 
            curr_predict_map[address] = 0;
        break;

        case 2 :
        if(taken) 
            curr_predict_map[address] = 3;
        else 
            curr_predict_map[address] = 0;
        break;

        case 3 :
        if(taken) 
            curr_predict_map[address] = 3;
        else 
            curr_predict_map[address] = 2;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

void 
RILPredictor::update_A4(Addr address, bool taken)
{
    int state = curr_predict_map[address];
    switch(state){
        case 0 :
        if(taken) 
            curr_predict_map[address] = 1;
        else 
            curr_predict_map[address] = 0;
        break;

        case 1 :
        if(taken) 
            curr_predict_map[address] = 3;
        else 
            curr_predict_map[address] = 0;
        break;

        case 2 :
        if(taken) 
            curr_predict_map[address] = 3;
        else 
            curr_predict_map[address] = 1;
        break;

        case 3 :
        if(taken) 
            curr_predict_map[address] = 3;
        else 
            curr_predict_map[address] = 2;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}



