/*
Read-invalid-line predictor

Author: Xiaoyang Gong 
*/

#include "mem/ruby/structures/RILPredictor.hh"
#include "sim/sim_object.hh"
#include "debug/RILPredictor.hh"

RILPredictor::RILPredictor(std::string& predictor_type) :
predictor_type(predictor_type),
local_hist_size(128),
global_hist_size(128),
state(0),
curr_predict(false)
{
    DPRINTF(RILPredictor, "Created the RIL Predictor\n");
}


bool
RILPredictor::predict(Addr address)
{
    return curr_predict;
}

void
RILPredictor::update_predict(bool actual_taken)
{
    if(predictor_type.compare("LT") == 0)
        predict_LT(actual_taken);
    else if(predictor_type.compare("A1") == 0)
        predict_A1(actual_taken);
    else if(predictor_type.compare("A2") == 0)
        predict_A2(actual_taken);
    else if(predictor_type.compare("A3") == 0)
        predict_A3(actual_taken);
    else if(predictor_type.compare("A4") == 0)
        predict_A4(actual_taken);
    else{
        std::cout << "Wrong predictor type / predictor undefined. Exit..." << std::endl;
        exit(1);
    }

}


// Static predictors
// FSM reference: Page 3 https://dl.acm.org/doi/pdf/10.1145/123465.123475
// "Two-Level Adaptive Training Branch Prediction"

void 
RILPredictor::predict_LT(bool taken)
{
    switch(state){
        case 0 :
        if(taken) state = 1;
        else state = 0;
        curr_predict = false;
        break;

        case 1 :
        if(taken) state = 1;
        else state = 0;
        curr_predict = true;
        break;

        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

void 
RILPredictor::predict_A1(bool taken)
{
    switch(state){
        case 0 :
        if(taken) state = 1;
        else state = 0;
        curr_predict = false;
        break;

        case 1 :
        if(taken) state = 3;
        else state = 2;
        curr_predict = true;
        break;

        case 2 :
        if(taken) state = 1;
        else state = 0;
        curr_predict = true;
        break;

        case 3 :
        if(taken) state = 3;
        else state = 2;
        curr_predict = true;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}


void 
RILPredictor::predict_A2(bool taken)
{
    switch(state){
        case 0 :
        if(taken) state = 1;
        else state = 0;
        curr_predict = false;
        break;

        case 1 :
        if(taken) state = 2;
        else state = 0;
        curr_predict = false;
        break;

        case 2 :
        if(taken) state = 3;
        else state = 1;
        curr_predict = true;
        break;

        case 3 :
        if(taken) state = 3;
        else state = 2;
        curr_predict = true;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

void 
RILPredictor::predict_A3(bool taken)
{
    switch(state){
        case 0 :
        if(taken) state = 1;
        else state = 0;
        curr_predict = false;
        break;

        case 1 :
        if(taken) state = 3;
        else state = 0;
        curr_predict = false;
        break;

        case 2 :
        if(taken) state = 3;
        else state = 0;
        curr_predict = true;
        break;

        case 3 :
        if(taken) state = 3;
        else state = 2;
        curr_predict = true;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

void 
RILPredictor::predict_A4(bool taken)
{
    switch(state){
        case 0 :
        if(taken) state = 1;
        else state = 0;
        curr_predict = false;
        break;

        case 1 :
        if(taken) state = 3;
        else state = 0;
        curr_predict = false;
        break;

        case 2 :
        if(taken) state = 3;
        else state = 1;
        curr_predict = true;
        break;

        case 3 :
        if(taken) state = 3;
        else state = 2;
        curr_predict = true;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

