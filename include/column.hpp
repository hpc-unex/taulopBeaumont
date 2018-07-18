//
//  column.hpp
//  TauLopCost
//
//  Created by jarico on 18/4/16.
//  Copyright © 2016 Juan A. Rico. All rights reserved.
//

#ifndef column_hpp
#define column_hpp

#include "matrix_config.h"

#include "rectangle.hpp"

#include <iostream>
#include <vector>
using namespace std;


class Column {
    
private:
    
    int  width;
    int  row_nr;  // Number of rows in this column (rectangles)
                  //  Calculated on insertion of rectangles.
    Rectangle **v_proc;
    
public:
    
     Column  (int row_nr);
     Column  (Column *c);
    ~Column  ();

    // TBD:  Problems with Column(int row_nr) + insert() / set()
    //  Constructor set P=row_nr. If we insert a new rectangle, P is increased -> ERROR
    //  P needs to be set in Constructor or increased in insert, not both.
    //  However, I need to test every call to insert in the code.
    void     insert      (Rectangle *p, int row);
    void     set         (Rectangle *p, int row);

    Rectangle *get         (int row);
    
    void     change (int r_orig, int r_dest);
    
    void     setWidth    (int width);
    int      getWidth    ();
    
    int      getRowNr    ();
    
    int      getNodeNr   ();
    
    void show ();
    ostream& operator<< (ostream& outs);
};

#endif /* column_hpp */
