//
//  cost.hpp
//  Cost
//
//  Created by jarico on 21/4/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#ifndef cost_hpp
#define cost_hpp

#include "taulop_cost.hpp"

#include <iostream>
using namespace std;


class Cost {

private:
    
    // Total volume is a calculated value adding the following intra and inter volumes
    unsigned long int volIntra;
    unsigned long int volInter;
    
    double  time;
    
public:
    
     Cost ();
     Cost (Cost *t);
    ~Cost ();
    
    // Main methods returning total time and volume
    unsigned long int getVolume ();
    float             getTime   ();
    
    unsigned long int getVolIntra ();
    unsigned long int getVolInter ();

    void  add  (TauLopCost *tc, unsigned int size = 1);
    void  add  (Cost *c,        unsigned int size = 1);
    
    //bool  operator< (const Cost *c);
    
    void  show ();
};


#endif /* cost_hpp */
