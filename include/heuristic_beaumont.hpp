//
//  heuristic_beaumont.hpp
//  TauLopCost
//
//  Created by jarico on 10/10/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//
//
// Implements the Beaumont algorithm in
//    [Beaumont et al "Matrix Multiplication on Heterogeneous Platforms", 2001]
// without "r" variable. That is, evaluates all the possible arrangements with P processes and
// get the best (not evaluates the arrangements with less than P processes).
// For evaluating the cost, the heuristic can call to an algorithm based on half-perimeters or
//   time (as tau-Lop).
// The order of the processes is based in their areas, as Beaumont algorithm establishes.



#ifndef heuristic_beaumont_hpp
#define heuristic_beaumont_hpp

#include "heuristic.hpp"
#include "algorithm.hpp"
#include "arrangement.hpp"
#include <unistd.h>
#include <list>
#include <iterator>

#include <iostream>
using namespace std;


#define DEBUG_MODE  1


const int BY_SPEED = 0;
const int BY_NODE  = 1;
const int BY_HIERARCH = 2;
const int BY_SCATTER = 3;
const int BY_DEFAULT = 4;



class BeaumontCost {
public:
    
    float        f_per;
    int          f_cut;
    Arrangement *f_arr;
    float        f_hp;
    float        f_theta;
    
    BeaumontCost (float f_per, int f_cut, Arrangement *a, float f_hp, float f_theta) {
        this->f_per   = f_per;
        this->f_cut   = f_cut;
        this->f_arr   = a;
        this->f_hp    = f_hp;
        this->f_theta = f_theta;
    }
};



class BeaumontHeuristic : public Heuristic {
    
private:
    
    float  *speeds,*speeds_parts,*speeds_nodes;
    int    *nodes, *nodes_parts, *nodes_aux;
    
    int    *ranks, *ranks_prev, *ranks_nodes;
    int    *rankIntra, *rankIntra_prev, *rankIntra_nodes;
    
    int  N;
    int  P;
    int  P_;
    int  M;
    int  B;
    int order;
  
    
    Algorithm *algorithm;
    
    // Matrix of costs
    list<BeaumontCost> **f;
    
    
    // PRIVATE interface
    void  setOrderBySpeed ();
    void  setOrderByNode  ();
    
    //Rectangle *createRectangle (int rank, int rankIntra, int node, int row, int col, int width, int height, int x, int y);
    void addRectangle (Column *c, int index, int row, int col, float width, int start_x);
    
    //void  rearrange       (Arrangement *r);
    float algorithmTime   (Arrangement *r);
    
public:
    
    BeaumontHeuristic  (Algorithm *alg);
    ~BeaumontHeuristic ();
    
    void  setP (int P);
    void  setB (int B);
    void  setN (int N);
    void  cleanArrays();
    
    int   getP ();
    
    void  loadCPM    (string file);
    void  loadCPM_nodes    (string file);
    void  loadCPM_scatter  (string file);
    void  loadPart2D (string file);
    void  loadPart2D_nodes (string file);
    void  loadPart2D_scatter (string file);
    
    void  setOrder        (int order); 
    void  OrderScatter  (); 
    void  setOrderBySpeed_parts ();
    void  setOrderByNode_parts  ();  
    
    virtual  Arrangement ** apply        (Arrangement *r = nullptr);
    virtual  long int       combinations (Arrangement *r);
    
    list<BeaumontCost> **getIntermediate ();
    
};

#endif /* heuristic_beaumont_hpp */
