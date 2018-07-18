//
//  cost.cpp
//  TauLopCost
//
//  Created by jarico on 21/4/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#include "cost.hpp"

#include <limits>
#include <iomanip>


Cost::Cost () {
    this->volIntra = 0;
    this->volInter = 0;
    this->time     = 0.0;
}

Cost::Cost (Cost *t) {
    this->volIntra = t->volIntra;
    this->volInter = t->volInter;
    this->time     = t->time;
}

Cost::~Cost () {
    
}


unsigned long int Cost::getVolume () {
    // Volume is the addition of inter + intra
    return (this->volIntra + this->volInter);
}


unsigned long int Cost::getVolIntra () {
    return (this->volIntra);
}


unsigned long int Cost::getVolInter () {
    return (this->volInter);
}


float Cost::getTime () {
    return this->time;
}


void Cost::add (TauLopCost *tc, unsigned int size) {

    this->volIntra += (tc->getVolume(0) * size);
    this->volInter += (tc->getVolume(1) * size);
    
    this->time += (tc->getTime() * size);

}


void Cost::add (Cost *c, unsigned int size) {
    
    this->volIntra += (c->volIntra * size);
    this->volInter += (c->volInter * size);
    
    this->time += (c->time * size);
}


void Cost::show () {

    cout << fixed << std::setprecision(6) << "Time   (s):  " << this->time / 1000000.0 << endl;
    cout << fixed << std::setprecision(3) << "Volume (KB): " << this->getVolume() / 1024
                                                    << "  (" << this->volIntra    / 1024
                                                    << " / " << this->volInter    / 1024 << ")" << endl;
}

