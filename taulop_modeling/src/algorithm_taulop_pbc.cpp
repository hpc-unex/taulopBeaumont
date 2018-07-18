//
//  algorithm_taulop_pbc.cpp
//  TauLopCost
//
//  Created by jarico on 9/5/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#include "algorithm_taulop_pbc.hpp"

#include "matrix_config.h"
#include "taulop_cost.hpp"
#include "taulop_operator.hpp"
#include "arrangement.hpp"
#include "taulop_concurrent.hpp"
#include "taulop_sequence.hpp"

#include <iostream>
using namespace std;



// PRIVATE Methods

int TauLopPBC::overlap (Rectangle *p_src, Rectangle *p_dst) {
    
    int blocks = 0;
    
    // Do they Overlap ??
    int y_src = p_src->getCoordY();
    int h_src = p_src->getHeight();
    int y_dst = p_dst->getCoordY();
    int h_dst = p_dst->getHeight();
    if ((y_src < y_dst + h_dst) && (y_src + h_src > y_dst)) {
        
        int start = y_dst - y_src;
        if (start < 0) start = 0;
        
        if (y_dst + h_dst > y_src + h_src) {
            blocks = h_src - start;
        } else {
            blocks = y_dst + h_dst - y_src - start;
        }
        
        // DBG:        cout << "Overlap: " << p_src->getRank() << "|" << p_dst->getRank()
        // << " from: " << start << " in: " << length << endl;
        
        //int b = r->getB();
        
        // Blocks to bytes
        //length = length * b * b * DATATYPE_SIZE;
    }
    
    return blocks;
}


void TauLopPBC::fillRow_P2P (Arrangement *r, int b_ini, Cost *c) {

    // 1. In what column is this block
    int col_nr = r->getColNr(b_ini);
    
    // 2. Process number in current column
    int nprocs = r->getRowNr(col_nr);
    
    
    // 3. Row communications for this interval (concurrents)
    TauLopConcurrent *conc = new TauLopConcurrent();
    for (int p = 0; p < nprocs; p++) {
        
        // 2.1. Fill a row communications. It is a sequence object.
        TauLopSequence *seq = new TauLopSequence ();
        
        Rectangle *p_src = r->get(p, col_nr);
        for (int col = 0; col < r->getColNr(); col++) {
            
            if (col == col_nr) continue;
            
            for (int row_nr = 0; row_nr < r->getRowNr(col); row_nr++) {
                
                Rectangle *p_dst = r->get(row_nr, col);
                
                // Do they Overlap ??
                int blocks = this->overlap (p_src, p_dst);
                
                if (blocks > 0) {
                    int m = 1;
                    int chn = (p_src->getNode() == p_dst->getNode()) ? 0 : 1;
                    int b = r->getB();
                    int length = blocks * b * b * DATATYPE_SIZE;
                    Transmission *T = new Transmission(p_src, p_dst, chn, m, length, 1);
                    seq->add(T);
                }
            }
        }
        
        seq->compact();
        conc->add(seq);
    }
    
#if TLOP_DEBUG == 1
    conc->show();
#endif
    
    // 3. Create Cost structure
    TauLopCost *tc = new TauLopCost();
    
    // 4. Apply the communication cost operation for only ONE pbc
    conc->evaluate(tc);
    
#if TLOP_DEBUG == 1
    tc->show();
    cout << endl << " PBC Time: " << tc->getTime() << endl << endl;
#endif
    
    c->add(tc);
    
    delete tc;
    delete conc;
}


void TauLopPBC::fillRow_RING (Arrangement *r, int b_ini, Cost *c) {
    
    // 1. Column where we are
    int col_nr = r->getColNr(b_ini);
    
    int col_src = col_nr;
    int col_dst = 0;
        
    for (int col = 0; col < r->getColNr() - 1; col++) {
        
        col_dst = (col_src + 1) % r->getColNr();
        
        int nprocs = r->getRowNr(col_src);
        
        // 2. Row communications for this interval (concurrents)
        TauLopConcurrent *conc = new TauLopConcurrent();
        for (int p = 0; p < nprocs; p++) {
            
            // 2.1. Fill a row communications. It is a sequence object.
            TauLopSequence *seq = new TauLopSequence ();
            
            Rectangle *p_src = r->get(p, col_src);
            for (int row_nr = 0; row_nr < r->getRowNr(col_dst); row_nr++) {
                
                Rectangle *p_dst = r->get(row_nr, col_dst);
                
                // Do they Overlap ??
                int blocks = this->overlap (p_src, p_dst);
                
                if (blocks > 0) {
                    int m = 1;
                    int chn = (p_src->getNode() == p_dst->getNode()) ? 0 : 1;
                    int b = r->getB();
                    int length = blocks * b * b * DATATYPE_SIZE;
                    Transmission *T = new Transmission(p_src, p_dst, chn, m, length, 1);
                    seq->add(T);
                }
            }
            
            seq->compact();
            conc->add(seq);
        }
        
#if TLOP_DEBUG == 1
        conc->show();
#endif
        
        // 3. Create Cost structure
        TauLopCost *tc = new TauLopCost();
        
        // 4. Apply the communication cost operation for only ONE pbc
        conc->evaluate(tc);
        
#if TLOP_DEBUG == 1
        // DBG:
        tc->show();
        cout << endl << " PBC Time: " << tc->getTime() << endl << endl;
#endif
        
        // 4.1. The cost is multiplied for the range of rows evaluated.
        c->add(tc);
        
        delete tc;
        delete conc;
        
        // 5. Next interval
        col_src = (col_src + 1) % r->getColNr();
    }
}



// Get the cost of an interval following an specific communication pattern (algorithm)
void TauLopPBC::intervalCost (Arrangement *r, int b_ini, Cost *c) {
    
    if (row_algorithm == PT2PT) { // P2P (1 to all)
        
        this->fillRow_P2P  (r, b_ini, c);
        
    } else if (row_algorithm == RING) { // Ring
        
        this->fillRow_RING (r, b_ini, c);
        
    } else {
        cout << "DBG: Error, PBC row algorithm not found or defined " << endl;
    }
}



//  PUBLIC Interface


TauLopPBC::TauLopPBC () {
}


TauLopPBC::~TauLopPBC () {
}


void TauLopPBC::getPBCCost (Arrangement *r, Cost *cpbc) {
        
    int b_size = 0;
    int b_ini  = 0;
    
    do { // Loop over all the horizontal intervals of the matrix arrangements
        
        // 1. Find the next block size
        r->getNextHInterval(b_ini, b_size);
        
#if TLOP_DEBUG == 1
        cout << "b_ini: " << b_ini << "  size: " << b_size << endl;
#endif
        
        Cost *c = new Cost();
        this->intervalCost(r, b_ini, c);
        
        // 2. The cost is multiplied for the range of rows evaluated.
        cpbc->add(c, b_size);
        delete c;
        
        // 3. Next interval
        b_ini += b_size;
        
    } while (b_ini < r->getWidth());
    
}



