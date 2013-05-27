
/*  Copyright (C) 2013 Stefan Helmert
    Boolean Satisfiability Solver - counterbased
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include "table.h"
#include <sstream>
#include "Satsolver.h"

#include "booleq2cnf.h"

class logic_vector
{
    public:
        logic_vector(unsigned x);
        ~logic_vector();
        unsigned gr_vec_n;
        graph* gr_vec;
        logic_vector operator& (logic_vector x);
        logic_vector operator| (logic_vector x);
        logic_vector operator^ (logic_vector x);
        logic_vector operator~ ();
        logic_vector operator== (logic_vector x);
        logic_vector operator+ (logic_vector x);
        logic_vector operator- (logic_vector x);
        //logic_vector int2lv(signed value, unsigned length);

};

logic_vector::logic_vector(unsigned x)
{
    gr_vec_n = x;
    gr_vec = new graph[x];
}

logic_vector::~logic_vector()
{
    delete(gr_vec);
}



logic_vector logic_vector::operator&(logic_vector x)
{
    logic_vector y(gr_vec_n);
    unsigned i;
    for(i=0;i<gr_vec_n;i++){
        y.gr_vec[i] = gr_vec[i] & x.gr_vec[i];
    }
    return y;
}

logic_vector logic_vector::operator|(logic_vector x)
{
    logic_vector y(gr_vec_n);
    unsigned i;
    for(i=0;i<gr_vec_n;i++){
        y.gr_vec[i] = gr_vec[i] | x.gr_vec[i];
    }
    return y;
}

logic_vector logic_vector::operator^(logic_vector x)
{
    logic_vector y(gr_vec_n);
    unsigned i;
    for(i=0;i<gr_vec_n;i++){
        y.gr_vec[i] = gr_vec[i] ^ x.gr_vec[i];
    }
    return y;
}

logic_vector logic_vector::operator~()
{
    logic_vector y(gr_vec_n);
    unsigned i;
    for(i=0;i<gr_vec_n;i++){
        y.gr_vec[i] = ~gr_vec[i];
    }
    return y;
}

logic_vector logic_vector::operator==(logic_vector x)
{
    logic_vector y(1);
    logic_vector tmp(gr_vec_n);
    unsigned i;
    tmp.gr_vec[0] = ~(gr_vec[0] ^ x.gr_vec[0]);

    for(i=1;i<gr_vec_n;i++){
        tmp.gr_vec[i] = tmp.gr_vec[i-1] & ~(gr_vec[i] ^ x.gr_vec[i]);
    }

    y.gr_vec[0] = tmp.gr_vec[gr_vec_n-1];
    return y;
}

logic_vector logic_vector::operator+(logic_vector x)
{
    logic_vector y(gr_vec_n);
    logic_vector co(gr_vec_n);
    unsigned i;
    y.gr_vec[0]  = gr_vec[0] ^ x.gr_vec[0];
    co.gr_vec[0] = gr_vec[0] & x.gr_vec[0];
    for(i=1;i<gr_vec_n;i++){
        y.gr_vec[i] = gr_vec[i] ^ x.gr_vec[i] ^ co.gr_vec[i-1];
        co.gr_vec[i]= gr_vec[i] & x.gr_vec[i] | x.gr_vec[i] & co.gr_vec[i-1] | gr_vec[i] & co.gr_vec[i-1];
    }
    return y;
}

logic_vector logic_vector::operator-(logic_vector x)
{
    logic_vector y(gr_vec_n);
    logic_vector co(gr_vec_n);
    unsigned i;
    y.gr_vec[0]  = gr_vec[0] ^ x.gr_vec[0];
    co.gr_vec[0] = gr_vec[0] ^ ~x.gr_vec[0];
    for(i=1;i<gr_vec_n;i++){
        y.gr_vec[i] = gr_vec[i] ^ ~x.gr_vec[i] ^ co.gr_vec[i-1];
        co.gr_vec[i]= gr_vec[i] & ~x.gr_vec[i] | ~x.gr_vec[i] & co.gr_vec[i-1] | gr_vec[i] & co.gr_vec[i-1];
    }
    return y;
}

logic_vector int2lv(signed value, unsigned length)
{
    logic_vector y(length);

    unsigned i;
    for(i=0;i<length;i++){
        if(value % 2) {
            y.gr_vec[i] = grSET();
        }
        else{
            y.gr_vec[i] = grCLR();
        }
        value = value >> 1;
    }
    return y;
}


int main()
{
    Sat_Instance_t SAT;
    Sat_Instance_t Solution;
    unsigned x;
    CNF testCNF;
    graph g1, g3, g4, g2;
    graph g5, g6, g7, g;





    logic_vector lv1(4), lv2(4), lv3(3), lv4(3), lv5(4), lv6(4), lv7(3), lv9(1);

    vector<operation_t> oplist;


    lv5 = lv1 + lv2 + int2lv(2,4);
    lv6 = lv2 + lv1 + int2lv(2,4);
    //lv5 = int2lv(9,4) - int2lv(5,4);
    //lv6 = int2lv(4,4);
    lv9 = lv5 == lv6;
    //lv4 = lv3;

    g7 = ~lv9.gr_vec[0];


    //g7 = g1 & g2 & g3 & g1 & g1;

    oplist = graph2op(oplist, g7);



    testCNF = oplist2cnf(oplist);


    SAT = CNFobj2Sat(testCNF);

    SAT = Sat_Update_Database(SAT);
    print_SAT_Instance_Infos(SAT);
    print_CNF(SAT);
    print_Counters(SAT);

    cout << "SAT? " << DPLLrec(SAT, &Solution) << endl;
    cout << endl;
    cout << Solution.Literal.size()-1 << endl;
    for(x=1;x<Solution.Literal.size();x++){
        if(Solution.Literal[x].neg.set) cout << "-" << x << " ";
        if(Solution.Literal[x].pos.set) cout << x << " ";
    }

    cout << endl;


    return 0;
}
