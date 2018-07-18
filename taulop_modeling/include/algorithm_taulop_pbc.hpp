//
//  algorithm_taulop_pbc.hpp
//  TauLopCost
//
//  Created by jarico on 9/5/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#ifndef algorithm_taulop_pbc_hpp
#define algorithm_taulop_pbc_hpp

#include "taulop_cost.hpp"
#include "arrangement.hpp"
#include "taulop_sequence.hpp"
#include "cost.hpp"

#include <iostream>

class TauLopPBC {
    
private:
        
    int  overlap (Rectangle *p_src, Rectangle *p_dst);
    
    void  fillRow_P2P   (Arrangement *r, int b_ini, Cost *c);
    void  fillRow_RING  (Arrangement *r, int b_ini, Cost *c);

    void  intervalCost  (Arrangement *r, int b_ini, Cost *c);
    
public:
    
    TauLopPBC  ();
    ~TauLopPBC ();
    
    void  getPBCCost (Arrangement *r, Cost *cpbc);
    
};

#endif /* algorithm_taulop_pbc_hpp */
