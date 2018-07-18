//
//  heuristic_malik.cpp
//  TauLopCost
//
//  Created by jarico on 26/4/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#include "heuristic.hpp"
#include "heuristic_malik.hpp"

#include "algorithm.hpp"
#include "rectangle.hpp"
#include "arrangement.hpp"

#include <iostream>
#include <string>
#include <iomanip>



bool comparar_coste(Cost *c1, Cost *c2) {

    //if (c1->getTime() * c1->getVolInter() > c2->getTime() * c2->getVolInter())
    if (c1->getTime() > c2->getTime())
    //if (c1->getVolInter() > c2->getVolInter())
        return true;
    
    return false;
}



void MalikHeuristic::createCombinations (Arrangement* &r, Cost* &cost) {
    
    // Number of combinations:
    //     fact(g_0) + fact(g_1) + ... + fact(g_n) + fact(#cols)
    
    // 1. Compact procs in groups per node
    Arrangement *r_curr = new Arrangement(r);
    r_curr->groupByNode();
    
    // 2. Final optimal arrangement
    Arrangement *r_opt = nullptr;
    
    // 3. Permute by column and find better arrangement incrementally
    for (int col = 0; col < r_curr->getColNr(); col++) {
        
        if (col == 0) {
            // 3.1.a) For the first column, create the arrangement
            int cols[1] = {0};
            r_opt = r_curr->createSubArrangement(1, cols);
            
        } else {
            // 3.1.b) Add column to current arrangement
            Column *cm = r_curr->getColumn(col);
            r_opt->addColumn(cm);
        }
        
        // 3.2. Permutations between groups in the current column
        int  groups_nr = 0;
        int  curr_node = -1;
        int *groups = new int [r_opt->getNodeNr(col)];
        
        for (int row = 0; row < r_opt->getRowNr(col); row++) {
            Rectangle *p = r_opt->get(row, col);
            if (curr_node != p->getNode()) {
                curr_node = p->getNode();
                groups[groups_nr] = curr_node;
                groups_nr++;
            }
        }
        
        // 3.3. Optimal cost and arrangement up to this column
        Arrangement *r_col = nullptr;
        Cost *c_col = nullptr;
        
        // 3.4. Given the number of groups, create a vector of all possible permutations
        long int tot_perm_nr = factorial(groups_nr);
        int *perm = new int [tot_perm_nr * groups_nr];
        this->generate_permutations(groups, groups_nr, perm);
        
        // 3.5. Test all possible permutations
        for (int perm_nr = 0; perm_nr < tot_perm_nr; perm_nr++) {
            
            // 3.5.1. Create a new arrangement for a given permutation of the current column
            Arrangement *r_perm = new Arrangement (r_opt);
            
            int rows[r_opt->getRowNr(col)];
            int row_nr = 0;
            
            for (int g = 0; g < groups_nr; g++) {
                for (int k = 0; k < r_opt->getRowNr(col); k++) {
                    Rectangle *p = r_opt->get(k, col);
                    if (p->getNode() == perm[(perm_nr * groups_nr) + g]) {
                        rows[row_nr] = p->getRank();
                        row_nr++;
                    }
                }
            }
            r_perm->changeRows(col, rows);

            // 3.5.2. Evaluate the arrangement permutation cost
            Cost *c_perm = this->alg->execute(r_perm);
            
#if TLOP_DEBUG == 0
            cout << fixed << setprecision(6) << "Malik (tau-Lop) Cost: " << c_perm->getTime()  / 1000000.0 << endl;
            c_perm->show();
            r_perm->show();
#endif
            // 3.5.3. Store arrangement if better
            if (perm_nr == 0) {
                c_col = new Cost(c_perm);
                r_col = new Arrangement(r_perm);
                
            } else {                
                if (comparar_coste(c_col, c_perm)) {
                    delete (r_col);
                    delete (c_col);
                    c_col = new Cost(c_perm);
                    r_col = new Arrangement(r_perm);
                }
            }
            
            delete r_perm;
            delete c_perm;
        }
        
        delete [] perm;
        
        r_opt = r_col;
        cost  = c_col;
        
#if TLOP_DEBUG == 0
        cout << fixed << setprecision(6) << "Time (col = " << col << "): " << cost->getTime()  / 1000000.0 << endl;
        r_opt->show();
        r_opt->plot("/Users/jarico/Development/combinations/s.gpl");
#endif
    }
    
    
    // 4. Test permutations of columns
    int col_nr = r_opt->getColNr();
    long int col_permnr = factorial(col_nr);
    int *columns = new int [col_nr];
    for (int i = 0; i < col_nr; i++) columns[i] = i;
    
    int new_cols[col_nr]; // New positions of the columns
    
    // 4.1. Generate all possible permutations of entire columns
    int *perm_col = new int [col_permnr * col_nr];
    this->generate_permutations(columns, col_nr, perm_col);
    
    Arrangement *s_opt = nullptr;
    
    for (int perm_nr = 0; perm_nr < col_permnr; perm_nr++) {
        
        Arrangement *s_perm = new Arrangement (r_opt);
        
        // 4.1.1. Change columns according to the permutation
        for (int c = 0; c < col_nr; c++) {
            new_cols[c] = perm_col[(perm_nr * col_nr) + c];
        }
        s_perm->changeCols(new_cols);
        
        // 4.1.2. Get the cost of this permutation  (in volume ??)
        Cost *c_perm = this->alg->execute(s_perm);
        
#if TLOP_DEBUG == 0
        cout << fixed << setprecision(6) << "Malik (tau-Lop) -Column- Cost: " << c_perm->getTime()  / 1000000.0 << endl;
        c_perm->show();
        s_perm->show();
#endif
        
        // 4.1.3. Store permutation if better
        if (comparar_coste(cost, c_perm)) {
            
            if (s_opt != nullptr)
                delete (s_opt);
            delete (cost);
            cost  = new Cost(c_perm);
            s_opt = new Arrangement(s_perm);
        }
        
        delete s_perm;
    }
    
    delete [] perm_col;
    delete [] columns;
    
    if (s_opt != nullptr) {
        delete (r_opt);
        r_opt = new Arrangement(s_opt);
    }
    
    
#if TLOP_DEBUG == 1
    cout << fixed << setprecision(6) << "Time (columns): " << cost->getTime()  / 1000000.0 << endl;
    r_opt->show();
    r_opt->plot("/Users/jarico/Development/combinations/s.gpl");
#endif
    
    // 5. Return best arrangment
    r = new Arrangement(r_opt);
    
    delete r_curr;
    return;
}





// PUBLIC interface

MalikHeuristic::MalikHeuristic (Algorithm *alg, string comm_mode, string network) {
    this->alg       = alg;
    this->comm_mode = comm_mode;
    this->network   = network;
    
}


MalikHeuristic::~MalikHeuristic () {
    
}


Arrangement ** MalikHeuristic::apply (Arrangement *r) {
    
    // Malik algorithm return the better arrangement according to
    //   the heuristic H2 in Paper C&C.
    Arrangement **v_arrang = new Arrangement* [2];
    Cost *c = nullptr;
    
    this->createCombinations (r, c);
    
    v_arrang[0] = r;
    v_arrang[1] = nullptr;
    
    return (Arrangement **)v_arrang;
}


long int MalikHeuristic::combinations (Arrangement *r) {
    
    
    // Compute all permutations in each column and entire columns
    //    r_0 ! + r_1 ! + ... + r_c !  +  #cols !
    
    Arrangement *r_curr = new Arrangement(r);
    long int perm_nr = 0;
    int      groups;
    int      curr_node;
    
    // 1. How many groups per column.
    r_curr->groupByNode();
    
    for (int col = 0; col < r_curr->getColNr(); col++) {
        
        curr_node = -1;
        groups = 0;
        
        for (int row = 0; row < r_curr->getRowNr(col); row++) {
            
            Rectangle *p = r_curr->get(row, col);
            if (curr_node != p->getNode()) {
                groups++;
                curr_node = p->getNode();
            }
        }
        
        perm_nr = perm_nr + factorial(groups);
    }
    // Rearrange columns
    perm_nr = perm_nr + factorial(r_curr->getColNr());
    
    delete r_curr;
    return perm_nr;
}

