//
//  algorithm_MxM.cpp
//
//  Created by jarico on 5/5/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#include "algorithm_MxM.hpp"

#include "cost.hpp"
#include "algorithm.hpp"
#include "arrangement.hpp"
#include "taulop_cost.hpp"
#include "taulop_operator.hpp"
#include "algorithm_taulop_pbr.hpp"
#include "algorithm_taulop_pbc.hpp"

#include <iostream>
using namespace std;




MxM_Algorithm::MxM_Algorithm () {
    
}

MxM_Algorithm::~MxM_Algorithm () {
    
    
}


Cost * MxM_Algorithm::execute (Arrangement *r) {
    
    Cost *c = new Cost();
    
    // Column (vertical) communications (Matrix B)
    TauLopPBR *pbr = new TauLopPBR ();
    pbr->getPBRCost(r, c);
    delete pbr;
    
    // Row (horizontal) communications (Matrix A)
    TauLopPBC *pbc = new TauLopPBC ();
    pbc->getPBCCost(r, c);
    delete pbc;
    
    return c;
}

