//
//  heuristic_malik.hpp
//  TauLopCost
//
//  Created by jarico on 26/4/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#ifndef heuristic_malik_hpp
#define heuristic_malik_hpp

#include "heuristic.hpp"
#include "algorithm.hpp"
#include "arrangement.hpp"

#include <iostream>
using namespace std;

class MalikHeuristic : public Heuristic {

private:
    
    Algorithm *alg;

    // Given a Column combination, combine processes in the arrangement
    //  Parameters:
    //    a) r: IN/OUT. Get the arrangement and return the optimal one
    //    b) c: OUT. Return the cost of the optimal arrangment in r.
    void createCombinations (Arrangement* &r, Cost* &c);
    
    // TEMPORAL
    string network;
    string comm_mode;

public:

     MalikHeuristic (Algorithm *alg, string comm_mode, string network);
    ~MalikHeuristic ();

    virtual  Arrangement ** apply        (Arrangement *r);
    virtual  long int       combinations (Arrangement *r);

};

#endif /* heuristic_malik_hpp */
