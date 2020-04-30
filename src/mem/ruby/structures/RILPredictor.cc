#ifndef __MEM_RUBY_STRUCTURES_RILPREDICTOR_HH__
#define __MEM_RUBY_STRUCTURES_RILPREDICTOR_HH__


#include "params/RILPredictor.hh"
#include "sim/sim_object.hh"
#include "debug/RILPredictor.hh"

RILPredictor*
RILPredictorParams::create()
{
    return new RILPredictor(this);
}

RILPredictor::RILPredictor(RILPredictorParams *params, string predictor_type) :
SimObject(params),
predictor_type(predictor_type),
local_hist_size(128),
global_hist_size(128),
state(0),
predict(false),
{
    DPRINTF(RILPredictor, "Created the RIL Predictor\n");
}


int
RILPredictor::predict(Addr address)
{
    return predict;
}

void
RILPredictor::update_predict(bool actual_taken)
{
    predict_LT(actual_taken);
}


// Static predictors
// FSM reference: Page 3 https://dl.acm.org/doi/pdf/10.1145/123465.123475
// "Two-Level Adaptive Training Branch Prediction"

bool 
RILPredictor::predict_LT(bool taken)
{
    switch(state){
        case 0 :
        if(taken) state = 1;
        else state = 0;
        return false;
        break;

        case 1 :
        if(taken) state = 1;
        else state = 0;
        return true;
        break;

        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

bool 
RILPredictor::predict_A1(bool taken)
{
    switch(state){
        case 0 :
        if(taken) state = 1;
        else state = 0;
        return false;
        break;

        case 1 :
        if(taken) state = 3;
        else state = 2;
        return true;
        break;

        case 2 :
        if(taken) state = 1;
        else state = 0;
        return true;
        break;

        case 3 :
        if(taken) state = 3;
        else state = 2;
        return true;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}


bool 
RILPredictor::predict_A2(bool taken)
{
    switch(state){
        case 0 :
        if(taken) state = 1;
        else state = 0;
        return false;
        break;

        case 1 :
        if(taken) state = 2;
        else state = 0;
        return false;
        break;

        case 2 :
        if(taken) state = 3;
        else state = 1;
        return true;
        break;

        case 3 :
        if(taken) state = 3;
        else state = 2;
        return true;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

bool 
RILPredictor::predict_A3(bool taken)
{
    switch(state){
        case 0 :
        if(taken) state = 1;
        else state = 0;
        return false;
        break;

        case 1 :
        if(taken) state = 3;
        else state = 0;
        return false;
        break;

        case 2 :
        if(taken) state = 3;
        else state = 0;
        return true;
        break;

        case 3 :
        if(taken) state = 3;
        else state = 2;
        return true;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

bool 
RILPredictor::predict_A4(bool taken)
{
    switch(state){
        case 0 :
        if(taken) state = 1;
        else state = 0;
        return false;
        break;

        case 1 :
        if(taken) state = 3;
        else state = 0;
        return false;
        break;

        case 2 :
        if(taken) state = 3;
        else state = 1;
        return true;
        break;

        case 3 :
        if(taken) state = 3;
        else state = 2;
        return true;
        break;
        default : 
        DPRINTF(RILPredictor, "Predictor FSM error!\n");
    }
}

