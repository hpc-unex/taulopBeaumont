//
//  heuristic_beaumont.cpp
//  TauLopCost
//
//  Created by jarico on 10/10/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#include "heuristic.hpp"
#include "heuristic_beaumont.hpp"

#include "matrix.hpp"
#include "algorithm.hpp"
#include "rectangle.hpp"
#include "arrangement.hpp"

#include <limits>
#include <iomanip>
#include <fstream>
#include <math.h>

using namespace std;



void BeaumontHeuristic::setOrderBySpeed_parts () {

    for (int i = 0; i < M; i++) {
        float min = speeds_parts[i];
        int   idx = i;
        for (int j = i + 1; j < M; j++) {
            if (speeds_parts[j] < min) {
                min = speeds_parts[j];
                idx = j;
            }
        }
        speeds_parts[idx] = speeds_parts[i];
        speeds_parts[i]   = min;
        
        int aux    = ranks_prev[idx];
        ranks_prev[idx] = ranks_prev[i];
        ranks_prev[i]   = aux;
        
        aux        = nodes_parts[idx];
        nodes_parts[idx] = nodes_parts[i];
        nodes_parts[i]   = aux;
        
        aux            = rankIntra_prev[idx];
        rankIntra_prev[idx] = rankIntra_prev[i];
        rankIntra_prev[i]   = aux;
    }
    
    return;
    
}


void BeaumontHeuristic::setOrderByNode_parts () {
    
    for (int i = 0; i < P; i++) {
        int  min = nodes_parts[i];
        int  idx = i;
        for (int j = i + 1; j < P; j++) {
            if (nodes_parts[j] < min) {
                min = nodes_parts[j];
                idx = j;
            }
        }
        nodes_parts[idx]  = nodes_parts[i];
        nodes_parts[i]    = min;
        
        int aux     = ranks_prev[idx];
        ranks_prev[idx]  = ranks_prev[i];
        ranks_prev[i]    = aux;
        
        float spd   = speeds_parts[idx];
        speeds_parts[idx] = speeds_parts[i];
        speeds_parts[i]   = spd;
        
        aux            = rankIntra_prev[idx];
        rankIntra_prev[idx] = rankIntra_prev[i];
        rankIntra_prev[i]   = aux;
    }
    
    return;
}

// PRIVATE interface

void BeaumontHeuristic::setOrderBySpeed () {

    for (int i = 0; i < M; i++) {
        float min = speeds[i];
        int   idx = i;
        for (int j = i + 1; j < M; j++) {
            if (speeds[j] < min) {
                min = speeds[j];
                idx = j;
            }
        }
        speeds[idx] = speeds[i];
        speeds[i]   = min;
        
        int aux    = ranks[idx];
        ranks[idx] = ranks[i];
        ranks[i]   = aux;
        
        aux        = nodes[idx];
        nodes[idx] = nodes[i];
        nodes[i]   = aux;
        
        aux            = rankIntra[idx];
        rankIntra[idx] = rankIntra[i];
        rankIntra[i]   = aux;
    }
    
    return;
    
}


void BeaumontHeuristic::setOrderByNode () {
    
    for (int i = 0; i < P; i++) {
        int  min = nodes[i];
        int  idx = i;
        for (int j = i + 1; j < P; j++) {
            if (nodes[j] < min) {
                min = nodes[j];
                idx = j;
            }
        }
        nodes[idx]  = nodes[i];
        nodes[i]    = min;
        
        int aux     = ranks[idx];
        ranks[idx]  = ranks[i];
        ranks[i]    = aux;
        
        float spd   = speeds[idx];
        speeds[idx] = speeds[i];
        speeds[i]   = spd;
        
        aux            = rankIntra[idx];
        rankIntra[idx] = rankIntra[i];
        rankIntra[i]   = aux;
    }
    
    return;
}


// Create a new rectangle from parameters. Rectangle is assinged to a process.
// Row and Col starts in 0.
// width and height sizes are in blocks. They must be scaled up to N.
// X and Y coordinates are in blocks. They must be scaled up to N.
void BeaumontHeuristic::addRectangle (Column *c, int index, int row, int col, float width, int start_x) {
    
    int col_width = round(width * this->N);
    
    // TBT: security for column width
    if (start_x + col_width > this->N) {
        col_width = this->N - start_x;
    }
    if (index == P-1) {
        col_width = this->N - start_x;
    }
    // -TBT
    
    c->setWidth(col_width);
    
    int start_y = 0;
    for (int r = 0; r < row; r++) {
        Rectangle *p = c->get(r);
        start_y += p->getHeight();
    }
    
    // Adding a rectangle scaling up to height N
    Rectangle *p = new Rectangle();
    
    p->setRank      (this->ranks     [index]);
    p->setRankIntra (this->rankIntra [index]);
    p->setNode      (this->nodes     [index]);
    
    p->setName      (to_string(this->nodes [index]));
    p->setPos       (row, col);
    
    int height;
    if (row == (c->getRowNr() - 1)) { // Rectangle in the last position
        height = this->N - start_y;
    } else {
        height = round((this->speeds[index] / width) * this->N);
    }
    p->setSize      (col_width, height);
    p->setCoords    (start_x, start_y);
    
    c->set(p, row);
}


float BeaumontHeuristic::algorithmTime (Arrangement *r) {
  
    float time = 0.0;
    
    Cost *c  = this->algorithm->execute (r);
    time = c->getTime();
    delete c;
    
    return (time);
}

















// PUBLIC interface

BeaumontHeuristic::BeaumontHeuristic (Algorithm *algorithm) {
    this->algorithm = algorithm;
}


BeaumontHeuristic::~BeaumontHeuristic () {
    
    for (int c = 0; c < this->M; c++) {
        for (int q = 0; q < this->M; q++) {
            if (f[c + q*M] != nullptr) {
                delete this->f[c + q*M];
            }
        }
    }
    
    delete [] this->f;

    delete [] this->ranks;
    delete [] this->ranks_prev;
    delete [] this->rankIntra_prev;
    delete [] this->ranks_nodes;
    delete [] this->rankIntra_nodes;
    delete [] this->rankIntra;
    delete [] this->nodes;
    delete [] this->nodes_parts;
    delete [] this->speeds;
    delete [] this->speeds_parts;
    delete [] this->speeds_nodes;
    
}


void BeaumontHeuristic::setP (int P) {
    this->P = P;
}


void BeaumontHeuristic::setB (int B) {
    this->B = B;
}


void BeaumontHeuristic::setN (int N) {
    this->N = N;
}



int BeaumontHeuristic::getP () {
    return this->P;
}

void BeaumontHeuristic::loadCPM_scatter(string file) {

    ranks     = new int   [P];
    rankIntra = new int   [P];
    nodes     = new int   [P];
    speeds    = new float [P];

    ranks_prev     = new int   [P];
    rankIntra_prev = new int   [P];
    nodes_parts     = new int   [P];
    speeds_parts    = new float [P];


    ifstream ifs;
    ifs.open(file);
    if (ifs.fail()) {
        cerr << "ERROR: cannot open file: " << file << endl;
        return;
    }
    
    // First line containing ranks
    for (int i = 0; i < P; i++) {
        string dat;
        ifs >> dat;
        ranks_prev[i] = atoi(dat.c_str());
    }
    
    // Second line containing nodes
    for (int i = 0; i < P; i++) {
        string dat;
        ifs >> dat;
        nodes_parts[i] = atoi(dat.c_str());
    }
    
    
    // Third line contains speeds (areas)
    for (int i = 0; i < P; i++) {
        string dat;
        ifs >> dat;
        speeds_parts[i] = atof(dat.c_str());
    }
    
    // rankintra is calculated
    for (int i = 0; i < P; i++) {
        int node = nodes_parts[i];
        int ri = 0;
        for (int j = 0; j < i; j++) {
            if (nodes_parts[j] == node)
                ri++;
        }
        rankIntra_prev[i] = ri;
    }

   this->setOrderByNode_parts();

   this->OrderScatter();

}

void BeaumontHeuristic::loadCPM_nodes (string file) {
    
        

    ranks     = new int   [P];
    rankIntra = new int   [P];

    nodes_parts     = new int   [P];
    speeds_parts    = new float [P];
    int max = 0;
    ifstream ifs;
    ifs.open(file);
    if (ifs.fail()) {
        cerr << "ERROR: cannot open file: " << file << endl;
        return;
    }
    
    // First line containing ranks
    for (int i = 0; i < P; i++) {
        string dat;
        ifs >> dat;
        ranks[i] = atoi(dat.c_str());
    }
    
    // Second line containing nodes
    for (int i = 0; i < P; i++) {
        string dat;
        ifs >> dat;
        nodes_parts[i] = atoi(dat.c_str());
	if(max < nodes_parts[i])
		max = nodes_parts[i];
    }
    M = max + 1;
    cout << " M: " << M << endl;

    nodes     = new int   [M];
    speeds    = new float [M];
    
    // Third line contains speeds (areas)
    for (int i = 0; i < P; i++) {
        string dat;
        ifs >> dat;
        speeds_parts[i] = atof(dat.c_str());
    }
    
    // rankintra is calculated
    for (int i = 0; i < P; i++) {
        int node = nodes_parts[i];
        int ri = 0;
        for (int j = 0; j < i; j++) {
            if (nodes_parts[j] == node)
                ri++;
        }
        rankIntra[i] = ri;
    }


    for(int i=0;i<P; i++)
	nodes[nodes_parts[i]] = nodes_parts[i];

    for(int i=0; i<P; i++)
	speeds[nodes_parts[i]] = 0.0;

    for(int i = 0; i<P; i++)
	speeds[nodes_parts[i]] += speeds_parts[i];
	

    for(int i = 0; i<P; i++){
	cout << "Node: " << nodes_parts[i] << "   Speed: " << speeds[nodes_parts[i]] << 
	"  RankIntra: " << rankIntra[i] << "  Rank: " << ranks[i] << " RankIntra: " << rankIntra[i] << " NodePart: " << nodes_parts[i] << " i: " << i << endl;

    }
    P_ = P;
    P = M;
    this->setOrderBySpeed();    

    ifs.close();
}


void BeaumontHeuristic::loadCPM (string file) {
    
    
    ranks     = new int   [P];
    rankIntra = new int   [P];
    nodes     = new int   [P];
    speeds    = new float [P];


    ifstream ifs;
    ifs.open(file);
    if (ifs.fail()) {
        cerr << "ERROR: cannot open file: " << file << endl;
        return;
    }
    
    // First line containing ranks
    for (int i = 0; i < P; i++) {
        string dat;
        ifs >> dat;
        ranks[i] = atoi(dat.c_str());
    }
    
    // Second line containing nodes
    for (int i = 0; i < P; i++) {
        string dat;
        ifs >> dat;
        nodes[i] = atoi(dat.c_str());
    }
    
    
    // Third line contains speeds (areas)
    for (int i = 0; i < P; i++) {
        string dat;
        ifs >> dat;
        speeds[i] = atof(dat.c_str());
    }
    
    // rankintra is calculated
    for (int i = 0; i < P; i++) {
        int node = nodes[i];
        int ri = 0;
        for (int j = 0; j < i; j++) {
            if (nodes[j] == node)
                ri++;
        }
        rankIntra[i] = ri;
    }
    
    if(order == BY_SPEED)
   	this->setOrderBySpeed();
    else if(order == BY_NODE)
	this->setOrderByNode();
    ifs.close();
}

/*
 bool secondtime;
    // Change Speeds by node
    for (int i = 0; i < this->P; i++) {
	secondtime = false;
	for(int j = 0; j < this->P; j++){
		if(this->nodes[j] == i && secondtime)
			this->speeds[i] = this->speeds[j] + this->speeds[i];
		else if(this->nodes[j] == i && !secondtime)
			secondtime = true;
	}   
    }
*/
void BeaumontHeuristic::loadPart2D (string file) {
 
    // Matrix and arrangement
    Matrix *m = new Matrix ();
    m->load(file);
    
    // Get arrangement data
    Arrangement *r = m->getArrangement(0);
    
#if (DEBUG_MODE==1)
    Cost *c = this->algorithm->execute(r);
    double time = c->getTime();
    cout << fixed << setprecision(3) << "Communication estimated time: " << time << endl;
#endif
    
    // Get values
    this->P = r->getP();
    this->B = r->getB();
    int H = r->getHeight();
    int W = r->getWidth();
    
    if (H != W) {
        cerr << "ERROR: by now, algorithm accepts only square matrices." << endl;
        return;
    }
    this->N = H;
    
    this->speeds = new float [this->P];
    r->getRelativeSpeeds(this->speeds);
    
    this->nodes = new int[this->P];
    r->getNodes(this->nodes);
    
    this->ranks = new int[this->P];
    int i = 0;
    for (int col = 0; col < r->getColNr(); col++) {
        for (int row = 0; row < r->getRowNr(col); row++) {
            Rectangle *p = r->get(row, col);
            this->ranks[i++] = p->getRank();
        }
    }
    
    
    this->rankIntra = new int[this->P];
    i = 0;
    for (int col = 0; col < r->getColNr(); col++) {
        for (int row = 0; row < r->getRowNr(col); row++) {
            Rectangle *p = r->get(row, col);
            this->rankIntra[i++] = p->getRankIntra();
        }
    }

/*
    for (int i = 0; i < P; i++) {
        int node = this->nodes[i];
        int ri = 0;
        for (int j = 0; j < i; j++) {
            if (this->nodes[j] == node)
                ri++;
        }
        this->rankIntra[i] = ri;
    }
*/
    // Set default order
    if(order == BY_SPEED){
	this->setOrderBySpeed();
	cout << "ORDER BY SPEED" << endl;
   } else if(order == BY_NODE){
	this->setOrderByNode();
	cout << "ORDER BY NODE" << endl;
   }
}


//Part by nodes velocity
void BeaumontHeuristic::loadPart2D_nodes (string file) {
 
    // Matrix and arrangement
    Matrix *m = new Matrix ();
    m->load(file);
    
    // Get arrangement data
    Arrangement *r = m->getArrangement(0);
    
#if (DEBUG_MODE==1)
    Cost *c = this->algorithm->execute(r);
    double time = c->getTime();
    cout << fixed << setprecision(3) << "Communication estimated time: " << time << endl;
#endif
    
    // Get values
    this->M = r->getNodeNr();
    this->P = r->getP();
    this->B = r->getB();
    int H = r->getHeight();
    int W = r->getWidth();
    
    if (H != W) {
        cerr << "ERROR: by now, algorithm accepts only square matrices." << endl;
        return;
    }
    this->N = H;
    
    this->speeds = new float [this->M];
    this->speeds_parts = new float [this->P];
    r->getRelativeSpeeds(this->speeds_parts);
    cout << "PRE speed: " << this->speeds[0] << endl;
    this->nodes = new int[this->M];
    this->nodes_parts = new int[this->P];
    r->getNodes(this->nodes_parts);
    
    this->ranks = new int[this->P];
    this->ranks_prev = new int[this->P];
    int i = 0;
    for (int col = 0; col < r->getColNr(); col++) {
        for (int row = 0; row < r->getRowNr(col); row++) {
            Rectangle *p = r->get(row, col);
            this->ranks[i] = p->getRank();
	    this->ranks_prev[i++] = p->getNode();

        }
    }
    
    
    this->rankIntra = new int[this->P];
    this->rankIntra_prev = new int[this->P];
    i = 0;
    for (int col = 0; col < r->getColNr(); col++) {
        for (int row = 0; row < r->getRowNr(col); row++) {
            Rectangle *p = r->get(row, col);
            this->rankIntra[i] = p->getRankIntra();
	    this->rankIntra_prev[i++] = 0;

        }
    }
/*
    for (int i = 0; i < P; i++) {
        int node = this->nodes[i];
        int ri = 0;
        for (int j = 0; j < i; j++) {
            if (this->nodes[j] == node)
                ri++;
        }
        this->rankIntra[i] = ri;
    }
*/
    // Set default order

    for(int i=0;i<this->P; i++){
	this->nodes[this->nodes_parts[i]] = this->nodes_parts[i];

    }
    for(int i=0; i<this->M; i++)
	this->speeds[i] = 0.0;

    for(int i = 0; i<this->P; i++){
	this->speeds[this->nodes_parts[i]] += this->speeds_parts[i];
	usleep(500);
	
	cout << "Node: " << this->nodes_parts[i] << "   Speed: " << this->speeds[this->nodes_parts[i]] << 
	"  RankIntra: " << this->rankIntra[i] << "  Rank: " << this->ranks[i] << " RankIntra: " << this->rankIntra[i] << " NodePart: " << this->nodes_parts[i] << " i: " << i << endl;
    }



  
    this->P_ = this->P;
    this->P = this->M;
    this->setOrderByNode();
}

void BeaumontHeuristic::loadPart2D_scatter(string file){

    // Matrix and arrangement
    Matrix *m = new Matrix ();
    m->load(file);
    
    // Get arrangement data
    Arrangement *r = m->getArrangement(0);
    
#if (DEBUG_MODE==1)
    Cost *c = this->algorithm->execute(r);
    double time = c->getTime();
    cout << fixed << setprecision(3) << "Communication estimated time: " << time << endl;
#endif
    
    // Get values
    this->P = r->getP();
    this->B = r->getB();
    int H = r->getHeight();
    int W = r->getWidth();
    
    if (H != W) {
        cerr << "ERROR: by now, algorithm accepts only square matrices." << endl;
        return;
    }
    this->N = H;
    
    this->speeds = new float [this->P];
    this->speeds_parts = new float [this->P];
    r->getRelativeSpeeds(this->speeds_parts);
    
    this->nodes_parts = new int[this->P];
    this->nodes = new int[this->P];
    r->getNodes(this->nodes_parts);

    
    this->ranks = new int[this->P];
    this->ranks_prev = new int[this->P];
    int i = 0;
    for (int col = 0; col < r->getColNr(); col++) {
        for (int row = 0; row < r->getRowNr(col); row++) {
            Rectangle *p = r->get(row, col);
            this->ranks_prev[i++] = p->getRank();
        }
    }
    
    
    this->rankIntra = new int[this->P];
    this->rankIntra_prev = new int[this->P];
    i = 0;
    for (int col = 0; col < r->getColNr(); col++) {
        for (int row = 0; row < r->getRowNr(col); row++) {
            Rectangle *p = r->get(row, col);
            this->rankIntra_prev[i++] = p->getRankIntra();
        }
    }

   this->setOrderByNode_parts();

   this->OrderScatter();

}


void BeaumontHeuristic::OrderScatter(){

   int x=0;
   int pos=0;
   int act=0;
   int x_per=0;

   for(int j=0;j<3; j++){ //Bucle para buscar velocidades del nodo i
		act = 0;
		for(int i=0; i<this->P;i++){
			
			if(this->nodes_parts[i] == act){
				
				if(x == 0){
					//cout << "i: " << i << endl;
					this->speeds[pos] = this->speeds_parts[i];
					this->rankIntra[pos] = this->rankIntra_prev[i];
					this->ranks[pos] = this->ranks_prev[i];
					this->nodes[pos] = this->nodes_parts[i];

					x=x_per;
					pos++;
					act++;
				}else{
					x--;

				}
			} 
			if(x_per >= 2 && x == 0 && this->nodes_parts[i+1] != act){
				x=x_per;
				act++;

			}
		}
	x_per++;
	x=x_per;
    }

    	 for(int i = 0; i<this->P; i++){

		cout << "Node: " << this->nodes[i]	 << "  Speed: " << this->speeds[i] << endl;
	}


}

void BeaumontHeuristic::setOrder (int ord) {

    switch (ord) {
        case BY_SPEED:
           // this->setOrderBySpeed();
	    order = BY_SPEED;
            break;
        case BY_NODE:
           // this->setOrderByNode();
	    order = BY_NODE;
            break;
        case BY_HIERARCH:
           // this->setOrderByNode();
	    order = BY_HIERARCH;
            break;
        case BY_SCATTER:
           // this->setOrderByNode();
	    order = BY_SCATTER;
            break;
        default:
            cerr << "ERROR: no supported order." << endl;
    }
    return;
}


Arrangement ** BeaumontHeuristic::apply (Arrangement *r) {

/*    
#if (DEBUG_MODE == 1)
    cout << "# Process number: " << P << endl;
    cout << "# Rank \t Node \t rnkIntra \t Speed " << endl;
    cout << "# ---- \t ---- \t -------- \t ----- " << endl;
    
    for (int i = 0; i < this->P_; i++) {
        cout << "   " << this->ranks[i] << "  \t   " << this->nodes[i] << "   \t    " << this->rankIntra[i]
        << "   \t  " << fixed << setprecision(3) << this->speeds[i] << endl;
    }
    cout << endl;
#endif	
*/    

    // Create matrix of costs
    this->f = new list<BeaumontCost> * [this->P * this->P];
    if (f == nullptr) {
        cerr << "ERROR: cannot allocate memory." << endl;
        return nullptr;
    }
    
    for (int c = 0; c < this->P; c++) {
        for (int q = 0; q < this->P; q++) {
            f[c + q*P] = new list<BeaumontCost>;
        }
    }

    
    // 1. Column C=0.
    int C = 0;
    float S = 0.0;
    
    for (int q = 0; q < P; q++) {
        
        // Original algorithm
        S = S + speeds[q];
        
        float f_per = 1 + S * (q+1);
        int   f_cut = 0;
        
        
        // Arrangement extensions
        Arrangement *a = new Arrangement ();
        int start_x = 0;
        Column *c = new Column (q+1);
        
        for (int i = 0; i < q+1; i++) {
            this->addRectangle(c, i, i, C, S, start_x);
        }
        
        a->addColumn(c);
        a->setHeight(this->N);
        a->setBlockSize(this->B);
        
        
        float f_hp = a->getHalfPerimeter() / this->N;
        float theta = 0.0;
        if (q == P-1) {
            theta = this->algorithmTime(a);
        }
        
        
        // Set values
        this->f[0 + q*P]->push_back({f_per, f_cut, a, f_hp, theta});
    }

    // SECOND Part
    // Values for every [C,q] are built from the [C-1,q-1] and [C,q-1] values differently.
    for (C = 1; C < P; C++) {
        
        for (int q = C; q < P; q++) {
            
            list <BeaumontCost> :: iterator lb_iter;
            
            // A) From Up-left: [C-1, q-1]
            
            for(lb_iter = this->f[(C-1) + (q-1)*P]->begin(); lb_iter != this->f[(C-1) + (q-1)*P]->end(); ++lb_iter) {
                
                // Original algorithm
                int r = q; // originally is used f_cut or r
                
                float width = 0.0;
                for (int i = r; i <= q; i++) {
                    width = width + speeds[i];
                }
                
                float f_per = (*lb_iter).f_per + 1 + width * (q+1-r);
                
                
                // Arrangement extensions
                Arrangement *a = new Arrangement((*lb_iter).f_arr);
                int start_x = a->getWidth();
                
                Column *c = new Column(1);
                this->addRectangle(c, q, 0, C, width, start_x);
                
                a->addColumn(c);
                a->setHeight(this->N);
                a->setBlockSize(this->B);
                

                float f_hp = a->getHalfPerimeter() / this->N;
                float theta = 0.0;
                if (q == P-1) {
                    theta = this->algorithmTime(a);
                }
                
                
                // Set values
                this->f[C + q*P]->push_back({f_per, r, a, f_hp, theta});
                
                delete c;
            }


            // B) From left: [C, q-1]
            for(lb_iter = this->f[C + (q-1)*P]->begin(); lb_iter != this->f[C + (q-1)*P]->end(); ++lb_iter) {
                
                // Original algorithm
                int r = (*lb_iter).f_cut;
                int procs = q - r;
                
                float curr_width = 0.0; // Of the current last column
                for (int i = r; i < r + procs; i++) {
                    curr_width = curr_width + speeds[i];
                }
                
                float width = 0.0; // Including the new proc
                width = curr_width + speeds[r + procs];
                
                float f_per = (*lb_iter).f_per - (curr_width * procs) + (width * (procs+1));
                
                
                // Arrangement extensions
                Arrangement *a = new Arrangement((*lb_iter).f_arr);
                int last_col = a->getColNr() - 1;
                a->delColumn(last_col);
                int start_x = a->getWidth();
                
                Column *c = new Column(procs + 1);
                for (int i = 0; i < procs + 1; i++) {
                    this->addRectangle(c, i+r, i, last_col, width, start_x);
                }
                
                a->addColumn(c);
                a->setHeight(this->N);                
                a->setBlockSize(this->B);
                
                
                float f_hp = a->getHalfPerimeter() / this->N;
                float theta = 0.0;
                if (q == P-1) {
                    theta = this->algorithmTime(a);
                }
                
                
                // Set values
                this->f[C + q*P]->push_back({f_per, r, a, f_hp, theta});
                
                delete c;
            }
        }
    }

    
    // THIRD Part
    // Rollback
    
    
    int q = P;
    Arrangement *a_min = nullptr;
    list <Column *> *cols = new list<Column *> ();
    
    int col_nr = 0;
    int kc[P];
    
    while (q != 0) {
        
        float f_per_min = numeric_limits<float>::max();
        int   r_min     = numeric_limits<int>::max();
        float f_hp_min  = numeric_limits<float>::max();
        float f_th_min  = numeric_limits<float>::max();
        
        for (int c = 0; c < C; c++) {
            
            list <BeaumontCost> :: iterator lb_iter;
            for(lb_iter = this->f[c + (q-1)*P]->begin(); lb_iter != this->f[c + (q-1)*P]->end(); ++lb_iter) {
                
                float f_per_curr = (*lb_iter).f_per;
                float f_th_curr  = (*lb_iter).f_theta;
                
                // TBD: thinking in the best form of using both methods depending on a parameter ???                
                //if (f_per_curr < f_per_min) {
                if (f_th_curr < f_th_min) {
                    
                    // Original algorithm
                    f_per_min = f_per_curr;
                    r_min     = (*lb_iter).f_cut;
                    
                    // Arrangement extensions
                    a_min     = (*lb_iter).f_arr;
                    f_hp_min  = (*lb_iter).f_hp;
                    f_th_min  = (*lb_iter).f_theta;
                }
            }
        }
        
        kc[col_nr] = q - r_min;
        col_nr++;
        q = r_min;
        
        // Put last column of the minimum cost arrangement
        cols->push_back(a_min->getColumn(a_min->getColNr()-1));
        //cols->push_front(a_min->getColumn(a_min->getColNr()-1));
    }

    // Optimal arrangement
    list <Column *> :: iterator it;
    Arrangement *a_opt = new Arrangement();
    for(it = cols->begin(); it != cols->end(); ++it) {
        Column *col = *it;
        a_opt->addColumn(col);

    }
    a_opt->setHeight(N);
    a_opt->setWidth(N);
    a_opt->setBlockSize(B);

     /*
     
    // Return arrangement
    return (Arrangement **)a_opt;
    
    */
    
   
    cout << "ORDER_: " << order << endl;
    /* SERGIO */
    switch (order) {
        case BY_SPEED or BY_NODE:
		{
			Arrangement *a_min = nullptr;

			float f_th_min  = numeric_limits<float>::max();

			for (int c = 0; c < C; c++) {

				list <BeaumontCost> :: iterator lb_iter;
				for(lb_iter = this->f[c + P * (P-1)]->begin(); lb_iter != this->f[c + P * (P-1)]->end(); ++lb_iter) {
				    
				    float f_th_curr  = (*lb_iter).f_theta;
				    
				    //fprintf(stdout, "f_th_curr: %9.3f  (C:%d)\n", f_th_curr, c);
				    
				    if (f_th_curr < f_th_min) {
					// Arrangement extensions
					a_min     = (*lb_iter).f_arr;
					f_th_min  = (*lb_iter).f_theta;
					//fprintf(stdout, "-->> F_TH_MIN: %9.3f  (C:%d)\n", f_th_min, c);
					//a_min->show();
				    }
				}
			}
			return (Arrangement **)a_min;
			break;
		}
	case BY_HIERARCH:
		{
			//Nodes arrangement
			list <Column *> :: iterator itn;
			Arrangement *a_nodes = new Arrangement();
			this->speeds_nodes = new float[this->M];
			this->ranks_nodes = new int[this->M];
			this->rankIntra_nodes = new int[this->M];
			int iterator_cols = 0;
			int k = 0;
			int rown = 0;
			int xn = 0;
			int yn = 0;
			int width_n = 0;
			int tam = a_opt->getHeight();


			list <Column *> :: iterator coln;
			 
				for (coln = cols->begin(); coln != cols->end(); ++coln) { //Bucles para iterar por los rectangulos del Arrangement
					Rectangle *r[8];
					for (int row = 0; row < a_opt->getRowNr(iterator_cols); row++) {

						Rectangle *p = a_opt->get(row, iterator_cols);
						Column *col = a_opt->getColumn(iterator_cols); // Anchos de las columnas
						width_n = col->getWidth();
						cout << "----------NODE: " << p->getNode() << endl;

				  		for(int j=0;j<this->P_; j++){ //Bucle para buscar velocidades del nodo i

							if(this->nodes_parts[j] == p->getNode()){ //Si es nodo i, guardamos la velocidad y rangos para mas tarde realizar particiones dentro de ese rectangulo
							 
								this->speeds_nodes[k] = this->speeds_parts[j];
								this->ranks_nodes[k] = this->ranks[j];
								this->rankIntra_nodes[k] = this->rankIntra[j];
								cout << "Node: " << this->nodes_parts[j] << " Rank: " << this->ranks[j] << " RankIntra: " << this->rankIntra[j] << endl;
								k++;
							}	
						}
						k = 0; //Volvemos a reiniciar el vector de velocidades por nodo
						//Ahora debemos realizar las operaciones pertinentes a la división del rectangulo correspondiente a cada nodo
						cout << this->speeds_nodes[0] << " " << this->speeds_nodes[1] << " " << this->speeds_nodes[2] << endl ;
			
						cout << "Ancho: " << width_n << "  Alto:" << p->getHeight() << " Tam MxM: " << tam << endl;
			
						for (int i=0; i < 3; i++){ //Cada nodo tiene de maximo 3 rectangulos, este bucle es para asignar los nuevos tamaños a los rectangulos y colocarlos en el arrangement

							if(this->speeds_nodes[i] > 0){ 

								Rectangle *p_ = new Rectangle(); // Creamos cada uno de los rectangulos que componen un nodo

								p_->setRank      (this->ranks_nodes[i]);
								p_->setRankIntra (this->rankIntra_nodes[i]);
								p_->setNode      (p->getNode());	    
								p_->setName      (to_string(p->getNode()));
						
								int height = round(this->speeds_nodes[i] * p->getHeight() / this->speeds[p->getNode()]); // Calculamos el alto de cada rectangulo
								//cout << "suma H y YN: " << yn + height << endl;
								if(yn + height > (tam-2)) // Redondeo
									height = height + (tam-(yn+height));			

								//cout << "FINAL yn:  " << height + yn << endl;
								//cout << "HEIGHT rectangle: " << this->speeds_nodes[i] << " = " << height << endl;
								p_->setPos       (rown, iterator_cols);	
								//cout << "POS rectangle: " << rown << ", " << iterator_cols << endl; 
								p_->setSize      (width_n, height);
								//cout << "SIZE rectangle: " << width_n << ", " << height << endl;
								p_->setCoords    (xn, yn);
								//cout << "COORDS rectangle: " << xn << ", " << yn << endl << endl;
								yn = yn + height;

								r[rown] = p_;
								rown++;	 

							}
						}
						this->speeds_nodes[0] = 0.0;
						this->speeds_nodes[1] = 0.0;
						this->speeds_nodes[2] = 0.0;
						this->ranks_nodes[0] = 0;
						this->ranks_nodes[1] = 0;
						this->ranks_nodes[2] = 0;
						this->rankIntra_nodes[0] = 0;
						this->rankIntra_nodes[1] = 0;
						this->rankIntra_nodes[2] = 0;
					}
		
		
					Column *c = new Column(rown); // Añadimos columnas al arrangement
					c->setWidth(width_n);
					for(int j=0; j<rown; j++){
						c->set(r[j],j);
						//cout << "Añadiendo COORDS:" << r[j]->getCoordX() << ", " << r[j]->getCoordY() << endl;
					}
		
					a_nodes->addColumn(c);
					//cout << "Columna añadida, WIDTH:" << c->getWidth() << endl;

					xn = xn + width_n;
					yn = 0;
				 	iterator_cols++;
					rown=0;
		
				}

				 a_nodes->setHeight(N);
			   	 a_nodes->setWidth(N);
				 a_nodes->setBlockSize(B);
				 a_nodes->setProcNr(this->P_);
				 cout << "ProcNr: " << this->P_ << endl;
				 a_nodes->setNodeNr(this->P_);
				 cout << "NodeNr: " << this->M << endl;
	

			    delete cols;
			    return (Arrangement **)a_nodes;
			    break;
		    }

	case BY_SCATTER:
		{
			return (Arrangement **)a_opt;
			break;
		}
	}
    
    
    
    
    
    // OTRA FORMA:
    // TBD: thinking in the best form of using both methods depending on a parameter ???
    /*
    Arrangement *a_min = nullptr;

    float f_th_min  = numeric_limits<float>::max();
    
    for (int c = 0; c < C; c++) {
        
        list <BeaumontCost> :: iterator lb_iter;
        for(lb_iter = this->f[c + P * (P-1)]->begin(); lb_iter != this->f[c + P * (P-1)]->end(); ++lb_iter) {
            
            float f_th_curr  = (*lb_iter).f_theta;
            
            fprintf(stdout, "f_th_curr: %9.3f  (C:%d)\n", f_th_curr, c);
            
            if (f_th_curr < f_th_min) {
                // Arrangement extensions
                a_min     = (*lb_iter).f_arr;
                f_th_min  = (*lb_iter).f_theta;
                //fprintf(stdout, "-->> F_TH_MIN: %9.3f  (C:%d)\n", f_th_min, c);
                //a_min->show();
            }
        }
    }
    return (Arrangement **)a_min;
     */
}



long int BeaumontHeuristic::combinations (Arrangement *r) {
    
    long int n = 0;
    
    
#if DEBUG_MODE==1
    // Create matrix of number of evaluations
    long int *f = new long int [this->P * this->P];
    if (f == nullptr) {
        cerr << "ERROR: cannot allocate memory." << endl;
        return -1;
    }
    
    for (int c = 0; c < this->P; c++) {
        for (int q = 0; q < this->P; q++) {
            f[c + q*P] = 0;
        }
    }
    
    // 1. Column C=0.
    for (int q = 0; q < P; q++) {
        f[q*P] = 1;
        n++;
    }
    
    // SECOND Part
    for (int C = 1; C < P; C++) {
        for (int q = C; q < P; q++) {
            
            long int prev = 0;
            prev = f[(C-1) + (q-1)*P];
            prev = prev + f[C + (q-1)*P];
            f[C + q*P] = prev;
            n += prev;
        }
    }
    
    delete [] f;
#else
    // According to [Beaumont et al., 2001], the cost is ~ p^2.5
    n = (this->P * this->P) * sqrt(this->P);
#endif
    return n;
}


list<BeaumontCost> **BeaumontHeuristic::getIntermediate () {
    return this->f;
}
