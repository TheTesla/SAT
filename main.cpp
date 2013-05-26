
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


struct node
{
    node* prev_node;
};

node addnode(node n)
{
    node* n1;
    node* n2;
    n1 = new node;
    n2 = new node;
    *n2 = n;
    n1->prev_node = n2;
    return *n1;
}

int main()
{
    Sat_Instance_t SAT;
    Sat_Instance_t Solution;
    unsigned x;
    CNF testCNF;
    graph g1, g3, g4, g2;
    graph g5, g6, g7, g;


    node n1,n2,n3;

    n1.prev_node = NULL;
    n2 = addnode(n1);
    n3 = addnode(n2);
    cout << n3.prev_node << " " << n3.prev_node->prev_node << " " << n3.prev_node->prev_node->prev_node << endl;



    logic_vector lv1(2), lv2(2), lv3(1), lv4(1), lv5(1);

    vector<operation_t> oplist;


    lv3 = ~(lv2 & lv1);

    lv4 = lv1 == lv2;
    cout << lv4.gr_vec_n << " " << lv4.gr_vec_n << endl;
    lv5 = lv4 == lv4;
    //lv4 = lv3;

    g7 = lv4.gr_vec[0] & lv5.gr_vec[0];


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
