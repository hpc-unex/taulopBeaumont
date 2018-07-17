//
//  main.cpp
//  TauLopCost
//
//  Created by jarico on 18/4/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#include "config.h"
#include "matrix_config.h"
#include <stdio.h>
#include <string.h>
#include "matrix.hpp"
#include "algorithm.hpp"
#include "heuristic.hpp"
#include "arrangement.hpp"
#include "cost.hpp"

#include "algorithm_basic.hpp"
#include "algorithm_malik.hpp"

#include "heuristic_malik.hpp"
#include "heuristic_combinatorial.hpp"
#include "heuristic_beaumont.hpp"

#include "algorithm_MxM.hpp"
#include "algorithm_W2D.hpp"

#include "taulop_operator.hpp"
#include "taulop_cost.hpp"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <limits>
#include "ilist.h"
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>


#include <list>
#include <iterator>

using namespace std;


#define BEAUMONT_DEBUG_MODE  0


int column_algorithm = PT2PT;
int row_algorithm    = PT2PT;


// Give details of the algorithms, including a final table of Cxq (PxP)
//   with the minimum values of each step
bool verbose_mode = false;




#if BEAUMONT_DEBUG_MODE==1
void extended_verbose_output (BeaumontHeuristic *b) {
    
    list<BeaumontCost> **costs;
    costs = b->getIntermediate();
    
    int P = b->getP();
    
    string efile;
    string efolder = "./data";
    
    mode_t old_umask;
    old_umask = umask(0);
    if (int status = mkdir (efolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH)) {
        if(status != 0 && errno != EEXIST) {
            cerr << "ERROR: cannot create folder: " << efolder << endl;
            return;
        } else {
            cerr << "WARNING: using already created folder: " << efolder << endl;
        }
    }
    umask(old_umask);
    
    if (efolder[efolder.length()-1] != '/') {
        efolder += "/";
    }
    
    efile = "part";
    
    
    long size;
    char *buf;
    char *ptr;
    
    size = pathconf(".", _PC_PATH_MAX);
    
    if ((buf = (char *)malloc((size_t)size)) == NULL) {
        cerr << "ERROR: allocating memory" << endl;
    }
    ptr = getcwd(buf, (size_t)size);
    
    chdir(efolder.c_str());
    
    
    cout << "# Extended Beumont et al. algorithm intermediate results table" << endl;
    
    cout << "# C ";
    for (int q = 0; q < P; q++) {
        cout << std::setw(28) << "q=" << q;
    }
    cout << endl;
    cout << "#---";
    for (int q = 0; q < P; q++) {
        cout << std::setw(30) << "----------------------";
    }
    cout << endl;
    
    for (int c = 0; c < P; c++) {
        
        cout << "  " << c;
        for (int q = 0; q < c; q++) {
            cout << std::setw(30) << "X";
        }
        
        for (int q = c; q < P; q++) {
            
            list <BeaumontCost> :: iterator lb_iter;
            
            int n = 0;
            cout << " [";
            for(lb_iter = (costs[c + q*P])->begin(); lb_iter != (costs[c + q*P])->end(); ++lb_iter) {
                
                if (lb_iter != costs[c + q*P]->begin()) {
                    cout << " | ";
                }
                cout << fixed << setprecision(2) << std::setw(8) << "  " << (*lb_iter).f_per
                << " / " << std::setw(4) << (*lb_iter).f_cut
                << " / " << setprecision(3) << std::setw(8) << (*lb_iter).f_theta;
                
                (*lb_iter).f_arr->plot   (efile + "_" + to_string(c) + "_" + to_string(q) + "_" + to_string(n) + ".gpl");
                (*lb_iter).f_arr->part2D (efile + "_" + to_string(c) + "_" + to_string(q) + "_" + to_string(n) + ".2dist");
                n++;
            }
            cout << " ] ";
        }
        cout << endl;
    }
    
    chdir(ptr);
}
#endif



void verbose_output (BeaumontHeuristic *b) {

    list<BeaumontCost> **costs;
    costs = b->getIntermediate();
    
    int P = b->getP();
    
    cout << "# Original Beumont et al. algorithm minimum table" << endl;
    
    cout << "# C ";
    for (int q = 0; q < P; q++) {
        cout << std::setw(28) << "q=" << q;
    }
    cout << endl;
    cout << "#---";
    for (int q = 0; q < P; q++) {
        cout << std::setw(30) << "----------------------";
    }
    cout << endl;
    
    for (int c = 0; c < P; c++) {
        
        cout << "  " << c;
        for (int q = 0; q < c; q++) {
            cout << std::setw(30) << "X";
        }
        
        for (int q = c; q < P; q++) {
            
            list <BeaumontCost> :: iterator lb_iter;
            
            float f_per_opt   = numeric_limits<float>::max();
            int   f_cut_opt   = numeric_limits<int>::max();
            float f_theta_opt = numeric_limits<float>::max();
            
            for(lb_iter = (costs[c + q*P])->begin(); lb_iter != (costs[c + q*P])->end(); ++lb_iter) {
                
                if ((*lb_iter).f_per < f_per_opt) {
                    f_per_opt   = (*lb_iter).f_per;
                    f_cut_opt   = (*lb_iter).f_cut;
                    f_theta_opt = (*lb_iter).f_theta;
                }
            }
            
            cout << fixed << setprecision(2) << std::setw(8) << "  " << f_per_opt << " / " << std::setw(4) << f_cut_opt
            << " / " << setprecision(3) << std::setw(8) << f_theta_opt;
        }
        cout << endl;
    }
}




int main (int argc, char **argv) {
    
    int opt;
    
    string cpm_file      = "./speeds.cpm";
    string out_cpm_file  = "";
    string part_file     = "";
    string out_part_file = "./outpart.2dist";
    string out_img_file  = "./outpart.gpl";
    int    N             = 100;
    int    B             = 32;
    int    P             = 0;
    int    M             = 0;
    string net           = "IB";
    bool   use_part_file = false;
    bool   generate_img  = false;
    bool   generate_part = false;
    string algorithm     = "MxM";
    int    order         = BY_SPEED;
    
    
    while ((opt = getopt(argc, argv, "hve:c:p:o:i:P:N:B:a:n:gt:m:O:")) != -1) {
        
        switch(opt) {
            case 'h':
                cerr << "Beaumont et al. column-based matrix partitioning algorithm and optimizations" << endl <<
                "Usage:   beaumont [options]"                                                  << endl << endl <<
                " [Generic Options]"                                                           << endl <<
                "\t -h help"                                                                   << endl <<
                "\t -v verbose mode"                                                           << endl << endl <<
                " [CPM data file Options]"                                                     << endl <<
                "\t -c <CPM file>          (default: " << cpm_file  << ")"                     << endl <<
                "\t -P <proc number>       (default: " << P << ")"                             << endl <<
                "\t -N <matrix size>       (default: " << N << "x" << N << ")"                 << endl <<
                "\t -B <block size>        (default: " << B << "x" << B << ")"                 << endl << endl <<
                " [FuPerMod partition file Options]"                                           << endl <<
                "\t -p <part file>         (default: none, use of CPM)"                        << endl <<
                "\t -o <output part file>  (default: " << out_part_file << ")"                 << endl <<
                "\t -i <Gnuplot img file>  (default: " << out_img_file << ")"                  << endl << endl <<
                " [t-Lop Model Options]"                                                       << endl <<
                "\t -a <algorithm>         (default: " << algorithm << "[, W2D])"              << endl <<
                "\t -n <network>           (default: " << net << "[, TCP])"                    << endl <<
                "\t -m <comm. mode>        (default: P2P [, Ring, Bcast])"                     << endl << endl <<
                " [Optimization Options]"                                                      << endl <<
                "\t -O <order>             (default: " << order << "[, BY_NODE,BY_SPEED,BY_HIERARCH,BY_SCATTER,BY_DEFAULT])"              << endl << endl;
                return 0;
            case 'c':
                cpm_file = optarg;
                break;
            case 'v':
                verbose_mode = true;
                break;
            case 'p':
                part_file = optarg;
                use_part_file = true;
                break;
            case 'o':
                out_part_file = optarg;
                generate_part = true;
                break;
            case 'i':
                out_img_file = optarg;
                generate_img = true;
                break;
            case 'P':
                P = atoi(optarg);
                break;
            case 'N':
                N = atoi(optarg);
                break;
            case 'B':
                B = atoi(optarg);
                break;
            case 'a':
                algorithm = optarg;
                break;
            case 'n':
                net = optarg;
                break;
            case 'm':
                if        (!strcmp(optarg,"P2P")) {
                    column_algorithm = PT2PT;
                    row_algorithm    = PT2PT;
                } else if (!strcmp(optarg,"Ring")) {
                    column_algorithm = RING;
                    row_algorithm    = RING;
                } else if (!strcmp(optarg,"Bcast")) {
                    column_algorithm = BCAST_BIN_OPENMPI;
                    row_algorithm    = PT2PT;
                } else {
                    cerr << "ERROR: unknown algorithm: " << optarg << endl;
                    return -1;
                }
                break;
            case 'O':
                if        (!strcmp(optarg, "BY_SPEED")) {
                    order = BY_SPEED;
                } else if (!strcmp(optarg, "BY_NODE")) {
                    order = BY_NODE;
                } else if (!strcmp(optarg, "BY_HIERARCH")) {
                    order = BY_HIERARCH;
                } else if (!strcmp(optarg, "BY_SCATTER")) {
                    order = BY_SCATTER;
                } else if (!strcmp(optarg, "BY_DEFAULT")) {
                    order = BY_DEFAULT;
                } else {
                    cerr << "ERROR: unknown ORDER algorithm: " << optarg << endl;
                    return -1;
                }
                break;
            default:
                cerr << "ERROR: invalid arguments." << endl;
                return -1;
        }
    }
    
    // Algorithm
    Algorithm *alg = nullptr;
    
    if        (algorithm == "MxM") {
        alg = new MxM_Algorithm ();
    } else if (algorithm == "W2D") {
        alg = new W2D_Algorithm ();
    } else {
        cerr << "ERROR: algorithm not supported: " << algorithm << endl;
    }
    
    // Set network type for using specific model parameters
    TauLopParam::setInstance(net);
    
    
    // Beaumont heuristic
    BeaumontHeuristic *b = new BeaumontHeuristic (alg);
    b->setOrder(order);
    
    if (use_part_file) { // Use FuPerMod generated part 2D file
	if(order == BY_SCATTER){
		b->loadPart2D_scatter(part_file);
	//	b->setOrderByNode_parts();
	}
	else if(order == BY_SPEED | order == BY_NODE)
        	b->loadPart2D(part_file);
	else if(order == BY_HIERARCH)
		b->loadPart2D_nodes(part_file);
	else if(order == BY_DEFAULT){
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

			
    } else { // Use CPM file        
        b->setP(P);
        b->setB(B);
        b->setN(N);
	cout << "MAIN ORDER: "<< order << endl;
	if(order == BY_SPEED | order == BY_NODE)
	        b->loadCPM(cpm_file);
	else if(order == BY_HIERARCH)
		b->loadCPM_nodes(cpm_file);
	else if(order == BY_SCATTER)
		b->loadCPM_scatter(cpm_file);
    }
    
    
    // Combinations:
    long int n_comb = b->combinations(nullptr);
    cout << "# Combinations: " << n_comb << endl;

  
    Arrangement *r_opt = (Arrangement *)b->apply();
    
    if (verbose_mode) {
        r_opt->show();
	cout << "Post Show" << endl;
        Cost *c = alg->execute(r_opt);
	cout << "Post execute post" << endl;
        double time = c->getTime();
	cout << "Post getTime" << endl;
        cout << fixed << setprecision(3) << "Communication estimated time: " << time << endl;
    }
    
        
    // OUTPUTS
    
    // Output Part 2dist FuPerMod description file
    if (generate_part) {
        r_opt->part2D(out_part_file);
    }
    
    // Output Gnuplot file
    if (generate_img) {
        r_opt->plot(out_img_file);
    }
    
    // Detailed ouput
    if (verbose_mode) {
        verbose_output (b);
    }
    
#if BEAUMONT_DEBUG_MODE==1
    extended_verbose_output(b);
#endif

    
    delete b;
    return EXIT_SUCCESS;
}

