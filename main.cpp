
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
        y.gr_vec[i].self = &y.gr_vec[i]; // vmtl. aufgrund eines Compilerbugs nötig, außerhalb Funktion/Methode kein Probelm, vmtl. wird bei Zuweisung Quellobjekt gelöscht
    }
    return y;
}

logic_vector logic_vector::operator|(logic_vector x)
{
    logic_vector y(gr_vec_n);
    unsigned i;
    for(i=0;i<gr_vec_n;i++){
        y.gr_vec[i] = gr_vec[i] | x.gr_vec[i];
        y.gr_vec[i].self = &y.gr_vec[i]; // vmtl. aufgrund eines Compilerbugs nötig, außerhalb Funktion/Methode kein Probelm, vmtl. wird bei Zuweisung Quellobjekt gelöscht
    }
    return y;
}

logic_vector logic_vector::operator^(logic_vector x)
{
    logic_vector y(gr_vec_n);
    unsigned i;
    for(i=0;i<gr_vec_n;i++){
        y.gr_vec[i] = gr_vec[i] ^ x.gr_vec[i];
        y.gr_vec[i].self = &y.gr_vec[i]; // vmtl. aufgrund eines Compilerbugs nötig, außerhalb Funktion/Methode kein Probelm, vmtl. wird bei Zuweisung Quellobjekt gelöscht
    }
    return y;
}

logic_vector logic_vector::operator~()
{
    logic_vector y(gr_vec_n);
    unsigned i;
    for(i=0;i<gr_vec_n;i++){
        y.gr_vec[i] = ~gr_vec[i];
        y.gr_vec[i].self = &y.gr_vec[i]; // vmtl. aufgrund eines Compilerbugs nötig, außerhalb Funktion/Methode kein Probelm, vmtl. wird bei Zuweisung Quellobjekt gelöscht
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

    logic_vector lv1(2), lv2(2), lv3(2), lv4(2);

    vector<operation_t> oplist;


    lv3 = lv2 & ~lv1;

    lv4 = lv3 ^ lv3;

    g6 = lv4.gr_vec[0];
    g5 = lv4.gr_vec[1];

    g7 = g5 & g6;


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
