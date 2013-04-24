
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

class CNF
{
    public:
        CNF();
        ~CNF();
        signed noLit;
        signed addLit();
        signed cone();
        signed czero();
        signed cand(signed a, signed b);
        signed cnot(signed a);
        signed cor(signed a, signed b);
        signed cxor(signed a, signed b);
        void miter(signed a, signed b);
        vector<Sat_Clause_t> Clause;
};

CNF::CNF()
{
    noLit = 0;
}

CNF::~CNF()
{

}

signed CNF::addLit()
{
    noLit++;
    return noLit;
}

signed CNF::cone()
{
    signed l;
    Sat_Clause_t c;
    l = addLit();
    c.Literal.clear(); c.Literal.push_back(l);
    Clause.push_back(c);
    return l;
}

signed CNF::czero()
{
    signed l;
    Sat_Clause_t c;
    l = addLit();
    c.Literal.clear(); c.Literal.push_back(-l);
    Clause.push_back(c);
    return l;
}

signed CNF::cand(signed a, signed b)
{
    signed y;
    y = addLit();
    Sat_Clause_t c;
    c.Literal.clear(); c.Literal.push_back(-a); c.Literal.push_back(-b); c.Literal.push_back(y);   Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(-a); c.Literal.push_back(b); c.Literal.push_back(-y);   Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(a); c.Literal.push_back(-b); c.Literal.push_back(-y);   Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(a); c.Literal.push_back(b); c.Literal.push_back(-y);    Clause.push_back(c);
    return y;
}

signed CNF::cnot(signed a)
{
    signed y;
    y = addLit();
    Sat_Clause_t c;
    c.Literal.clear(); c.Literal.push_back(a); c.Literal.push_back(y);     Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(-a); c.Literal.push_back(-y);   Clause.push_back(c);
    return y;
}

signed CNF::cor(signed a, signed b)
{
    signed y;
    y = addLit();
    Sat_Clause_t c;
    c.Literal.clear(); c.Literal.push_back(-a); c.Literal.push_back(-b); c.Literal.push_back(y);   Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(-a); c.Literal.push_back(b); c.Literal.push_back(y);   Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(a); c.Literal.push_back(-b); c.Literal.push_back(y);   Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(a); c.Literal.push_back(b); c.Literal.push_back(-y);    Clause.push_back(c);
    return y;
}

signed CNF::cxor(signed a, signed b)
{
    signed y;
    y = addLit();
    Sat_Clause_t c;
    c.Literal.clear(); c.Literal.push_back(-a); c.Literal.push_back(-b); c.Literal.push_back(-y);   Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(-a); c.Literal.push_back(b); c.Literal.push_back(y);   Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(a); c.Literal.push_back(-b); c.Literal.push_back(y);   Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(a); c.Literal.push_back(b); c.Literal.push_back(-y);    Clause.push_back(c);
    return y;
}

void CNF::miter(signed a, signed b)
{
    Sat_Clause_t c;
    c.Literal.clear(); c.Literal.push_back(a); c.Literal.push_back(b);    Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(-a); c.Literal.push_back(-b);     Clause.push_back(c);
}


Sat_Instance_t CNFobj2Sat(CNF CNFobj)
{
    unsigned x;
    Sat_Instance_t sat;
    sat.consistent = false;
    sat.noClauses = CNFobj.Clause.size();
    sat.Clause = CNFobj.Clause;
    for(x=0;x<CNFobj.Clause.size();x++){
        sat.Clause[x].eliminated = false;
        sat.Clause[x].noLiterals = sat.Clause[x].Literal.size();
    }
    sat.noLiterals = CNFobj.noLit;
    sat.noSinglepolLiterals = 0;
    sat.noUnitClauses = 0;
    sat.UNSAT = false;
    sat.SAT = false;
    sat.noEliminatedClauses = 0;
    sat.WorkingLiteral = 0;
    return sat;
}

int main()
{
    Sat_Instance_t SAT;
    Sat_Instance_t Solution;
    unsigned x;
    signed a, b, c, d;
    CNF testCNF;

    //SAT = readCNF("C:\\Projekte\\C++\\SAT\\easy\\aim-50-1_6-yes1-1.cnf");

    a = testCNF.addLit();
    b = testCNF.addLit();
    c = testCNF.cxor(a, b);
    d = testCNF.cxor(b, a);
    testCNF.miter(c, d);

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
