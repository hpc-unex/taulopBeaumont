//
//  main.cpp
//  TauLopCost
//
//  Created by jarico on 18/4/16.
//  Copyright Â© 2016 Juan A. Rico. All rights reserved.
//

#include "matrix.hpp"
#include "algorithm.hpp"
#include "arrangement.hpp"
#include "cost.hpp"

#include "algorithm_basic.hpp"
#include "algorithm_malik.hpp"
#include "algorithm_MxM.hpp"


#include "heuristic_malik.hpp"
#include "heuristic_combinatorial.hpp"
#include "heuristic_beaumont.hpp"


#include "taulop_operator.hpp"
#include "taulop_cost.hpp"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <fstream>
#include "ilist.h"
#include <string>
#include <sstream>
#include <limits>
#include <iomanip>
using namespace std;


int column_algorithm = PT2PT;
int row_algorithm    = PT2PT;


int main(int argc, const char * argv[]) {
    
    if (argc != 5) {
        cerr << "ERROR: invalid number of parameters: " << argc << "  Expected: 5." << endl;
        cerr << "delta  folder  network  comm_mode  kernel " << endl;
        return -1;
    }
    
    string  folder    = argv[1];
    string  network   = argv[2];
    string  comm_mode = argv[3];
    string  kernel    = argv[4]; // Kernel to run
    
    string part_file             = folder + "part_delta.2dist";
    string out_square_delta_file = folder + "part_delta_squares.gpl";
    
    
    cout << "Part file: " << part_file << endl;
    
    
    TauLopParam::setInstance(network);
    
    Matrix *m = new Matrix ();
    //m->load(); // Synthetic matrix for testing
    m->load(part_file);
    
    // 1. Algorithms
    Arrangement *r_orig = m->getArrangement(0);
    
    r_orig->show();
    r_orig->plot(out_square_delta_file);
        
    return 0;
}
