//
//  algorithm_taulop_pbr.hpp
//  TauLopCost
//
//  Created by jarico on 9/5/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#ifndef algorithm_taulop_pbr_hpp
#define algorithm_taulop_pbr_hpp

#include "ilist.h"
#include "arrangement.hpp"
#include "cost.hpp"
#include "taulop_cost.hpp"
#include "taulop_sequence.hpp"
#include "taulop_concurrent.hpp"

#include <iostream>


class TauLopPBR {
    
private:
    
    void  fillColumn_P2P   (Arrangement *r, int b_ini, Cost *c);
    void  fillColumn_RING  (Arrangement *r, int b_ini, Cost *c);
    void  fillColumn_BCAST (Arrangement *r, int b_ini, Cost *c);
    
    void  intervalCost     (Arrangement *r, int b_ini, Cost *c);
    
public:

     TauLopPBR ();
    ~TauLopPBR ();
    
    void getPBRCost (Arrangement *r, Cost *cpbr);
    
};

#endif /* algorithm_taulop_pbr_hpp */
