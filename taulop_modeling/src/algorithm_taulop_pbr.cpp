//
//  algorithm_taulop_pbr.cpp
//  TauLopCost
//
//  Created by jarico on 9/5/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#include "algorithm_taulop_pbr.hpp"

#include "taulop_kernel.hpp"

#include <cmath>
#include <iostream>
using namespace std;


// UTILITY FUNCTIONS



// PRIVATE methods

void TauLopPBR::fillColumn_P2P (Arrangement *r, int b_ini, Cost *c) {
    
    // 1. Iterate over the columns of the matrix:
    //     - A concurrent object for all columns
    //     - A sequence object for the processes inside a ciolumn
    TauLopConcurrent *conc = new TauLopConcurrent();
    
    for (int col = 0; col < r->getColNr(); col++) {
        
        // 1.1. Fill a column communications. It is a sequence object.
        TauLopSequence *seq = new TauLopSequence ();
        
        // Find the rectangle owning the PBR
        int curr_b_ini  = 0;
        int row_nr = r->getRowNr(col);
        int row = 0;
        Rectangle *p_src = nullptr;
        Rectangle *p_dst = nullptr;
        
        // 1.2. Where is the PBR?
        for (row = 0; row < row_nr; row++) {
            p_src = r->get(row, col);
            curr_b_ini = curr_b_ini + p_src->getHeight();
            if (curr_b_ini > b_ini) {
                break;
            }
        }
        
        // 1.3. From the active rectangle (process) to the rest, create transmissions
        //  Algorithm: pt2pt
        for (int dst = (row + 1) % row_nr; dst != row; dst = (dst + 1) % row_nr) {
            
            p_dst = r->get(dst, col);
            
            int chn = (p_src->getNode() == p_dst->getNode()) ? 0 : 1; // TBD: What the channel_nr means?
            int m = 1; // Only row of block length
            int b = r->getB();
            int n = p_src->getWidth() * b * b * DATATYPE_SIZE;
            
            Transmission *T = new Transmission(p_src, p_dst, chn, m, n, 1);
            seq->add(T);
        }
        
        // 3. Add sequences of the column to the concurrent object
        seq->compact();
        conc->add(seq);
    }
    
#if TLOP_DEBUG == 1
    conc->show();
#endif
    
    // 2. Apply the communication cost operation for only ONE pbr
    TauLopCost *tc = new TauLopCost();
    conc->evaluate(tc);
    delete conc;
    
    // 4. Add the cost
    c->add(tc);
    
#if TLOP_DEBUG == 1
    // DBG:
    tc->show();
    cout << endl << " PBR Time (Pt2Pt): " << tc->getTime() << endl << endl;
#endif
    
    delete tc;
}



void TauLopPBR::fillColumn_RING (Arrangement *r, int b_ini, Cost *c) {
        
    // 1. Iterate over the columns of the matrix
    //    - Columns in a concurrent object
    //    - Processes in a column to a sequence object
    TauLopConcurrent *conc = new TauLopConcurrent();
    
    for (int col = 0; col < r->getColNr(); col++) {
        
        // 1.1. Fill a column communications. It is a sequence object.
        TauLopSequence *seq = new TauLopSequence ();
        
        // 1.2. Find the rectangle owning the PBR
        int curr_b_ini  = 0;
        int row_nr = r->getRowNr(col);
        int row = 0;
        Rectangle *p_src = nullptr;
        Rectangle *p_dst = nullptr;
        
        // 1.3. Where is the PBR
        for (row = 0; row < row_nr; row++) {
            
            p_src = r->get(row, col);
            
            curr_b_ini = curr_b_ini + p_src->getHeight();
            if (curr_b_ini > b_ini) {
                break;
            }
        }
        
        // 1.4. Create transmissions following a Ring pattern
        int dst = row;
        for (int i = 0; i < row_nr - 1; i++) {
            
            dst = (dst + 1) % row_nr;
            
            p_dst = r->get(dst, col);
            
            int chn = (p_src->getNode() == p_dst->getNode()) ? 0 : 1; // TBD: What the channel_nr means?
            int m = 1; // Only one row of block length
            int b = r->getB();
            int n = p_src->getWidth() * b * b * DATATYPE_SIZE;
            
            Transmission *T = new Transmission(p_src, p_dst, chn, m, n, 1);
            seq->add(T);
            
            // DBG: cout << "Comm: " << p_src->getRank() << " -> " << p_dst->getRank() << endl;
            p_src = p_dst;
        }
        
        // 1.5. Add to concurrent object of columns
        seq->compact();
        conc->add(seq);
    }
    
#if TLOP_DEBUG == 1
    conc->show();
#endif
    
    // 2. Create Cost structure
    TauLopCost *tc = new TauLopCost();
    
    // 3. Apply the communication cost operation for only ONE pbr (one block)
    conc->evaluate(tc);
    delete conc;

    // 4. Set the cost
    c->add(tc);

#if TLOP_DEBUG == 1
    // DBG:
    tc->show();
    cout << endl << " PBR Time: " << tc->getTime() << endl << endl;
#endif
    
    delete tc;
}


void TauLopPBR::fillColumn_BCAST (Arrangement *r, int b_ini, Cost *c) {

    int root = -1;
    TauLopCost *tc_max = nullptr;
    
    // 1. Iterate over the columns of the matrix
    //    - Each column has concurrent transmissions (binomial tree levels)
    //    - We need some mechanism to make concurrent the binomial trees in
    //      the columns (TBD)
    //  TBD:  We return the maximum of the columns without concurrency between them
    //        Solve this (?)
    
    for (int col = 0; col < r->getColNr(); col++) {
        
        // 1.1. Find the rectangle owning the PBR
        int curr_b_ini = 0;
        int P = r->getRowNr(col);  // P
        
        // 1.2. get Width size
        int b    = r->getB();
        int size = r->getWidth(col) * b * b * DATATYPE_SIZE;
        
        // 1.3. Create communicator and mapping
        Communicator *comm_col = new Communicator (P);
        int nodes[P];
        
        // 1.4. Create a Column communicator with the mapping and root
        for (int row = 0; row < P; row++) {
            
            Rectangle *p_src = r->get(row, col);
            nodes[row] = p_src->getNode();
            curr_b_ini = curr_b_ini + p_src->getHeight();
            if ((curr_b_ini > b_ini) && (root < 0)) {
                root = row;
            }
        }
        
        Mapping *map = new Mapping (P, nodes);
        comm_col->map(map);
#if TLOP_DEBUG == 1
        comm_col->show();
#endif
        
        // 1.5. Execute collective and get cost
        Collective *bin = nullptr;
        
        if (column_algorithm == BCAST_BIN_MPICH) {
            bin  = new BcastBinomial();
        } else if (column_algorithm == BCAST_BIN_OPENMPI) {
            bin  = new BcastBinomialOpenMPI();
        } else {
            cout << "DBG: Error, Broadcast algorithm not found or defined " << endl;
        }
        
        // 2. Apply the communication cost operation for only ONE pbr (one block)
        TauLopCost *tc = bin->evaluate (comm_col, &size, root);

        // 3. Use time to compare columns. MAX time is the final cost
        if (tc_max == nullptr) {
            tc_max = new TauLopCost(tc);
        } else {
            if (tc->getTime() > tc_max->getTime()) {
                delete tc_max;
                tc_max = new TauLopCost(tc);
            }
        }

#if TLOP_DEBUG == 1
        cout << "DBG: Column: " << col << " cost using Broadcast." << endl;
        tc->show();
#endif
        
        delete tc;
    }
    
    // 4. Set the cost
    c->add(tc_max);
    delete tc_max;
}



// Get the cost of an interval following an specific communication pattern (algorithm)
void TauLopPBR::intervalCost (Arrangement *r, int b_ini, Cost *c) {
    
    if (column_algorithm == PT2PT) { // P2P (1 to all)
        
        this->fillColumn_P2P (r, b_ini, c);
        
    } else if (column_algorithm == RING) { // Ring
        
        this->fillColumn_RING (r, b_ini, c);
        
    } else if ((column_algorithm == BCAST_BIN_MPICH) ||
               (column_algorithm == BCAST_BIN_OPENMPI) )  { // Broadcast Binomial
        this->fillColumn_BCAST (r, b_ini, c);
        
    } else {
        cout << "DBG: Error, PBR algorithm not found or defined " << endl;
    }
    
}







// PUBLIC interface


TauLopPBR::TauLopPBR () {
}


TauLopPBR::~TauLopPBR () {
}


void TauLopPBR::getPBRCost (Arrangement *r, Cost *cpbr) {
    
    int b_size = 0;
    int b_ini  = 0;
    
    // Loop over all the vertical intervals of the matrix arrangements
    do {
        // 1. Find the next range init and size
        r->getNextVInterval(b_ini, b_size);
        
#if TLOP_DEBUG == 1
        cout << "b_ini: " << b_ini << "  size: " << b_size << endl;
#endif
        
        // 2. Build a concurrent object with the per column communications and get its cost
        Cost *c = new Cost();
        this->intervalCost(r, b_ini, c);
        
        // 3. The cost is multiplied for the range of rows evaluated.
        cpbr->add(c, b_size);
        delete c;
        
        // 4. Next interval
        b_ini += b_size;
        
    } while (b_ini < r->getHeight());
    
}

