
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


typedef signed std_logic;

enum optype_t
{
    INP,
    AND,
    OR,
    NOT,
    XOR
};



class graph
{
    public:
        graph();
        graph(signed x);
        ~graph();
        graph* operand1;
        graph* self;
        signed operand1key;
        graph* operand2;
        signed operand2key;
        signed Literal;
        signed key;
        optype_t operation;
        graph operator& (graph x);
        graph operator| (graph x);
        graph operator^ (graph x);
        graph operator~ ();


};

graph::graph()
{
    operation = INP;
    operand1 = NULL;
    operand1key = 0;
    operand2 = NULL;
    operand2key = 0;
    Literal = 0;
    self = this;
    key = 0;
}

graph::graph(signed x)
{
    operation = INP;
    operand1 = NULL;
    operand1key = 0;
    operand2 = NULL;
    operand2key = 0;
    Literal = 0;
    self = this;
    key = x;
}


graph::~graph()
{

}

graph graph::operator&(graph x)
{
    graph gr;
    gr.operation = AND;
    gr.operand1 = this;
    gr.operand2 = x.self;
    return gr;
}

graph graph::operator|(graph x)
{
    graph gr;
    gr.operation = OR;
    gr.operand1 = this;
    gr.operand2 = x.self;
    return gr;
}

graph graph::operator^(graph x)
{
    graph gr;
    gr.operation = XOR;
    gr.operand1 = this;
    gr.operand2 = x.self;
    return gr;
}

graph graph::operator~()
{
    graph gr;
    gr.operation = NOT;
    gr.operand1 = this;
    gr.operand2 = NULL;
    return gr;
}


struct operation_t
{
    optype_t op;
    graph* g;
    signed operand1;
    graph* operand1ptr;
    signed operand2;
    graph* operand2ptr;
};


vector<operation_t> graph2op(vector<operation_t> oplist, graph& g)
{
    operation_t operation;
    vector<operation_t> opv1, opv2;
    opv1.clear();
    opv2.clear();
    if(g.operand1) opv1 = graph2op(oplist, *g.operand1);
    if(g.operand2) opv2 = graph2op(oplist, *g.operand2);
    oplist.insert(oplist.end(), opv1.begin(), opv1.end());
    oplist.insert(oplist.end(), opv2.begin(), opv2.end());
    operation.g = &g;
    operation.op = g.operation;
    operation.operand1ptr = g.operand1;
    operation.operand2ptr = g.operand2;
    oplist.push_back(operation);
    return oplist;
}

vector<operation_t> oplist_assign_literals(vector<operation_t> oplist)
{
    unsigned x;
    signed lit;
    lit = 1;

    // bereits vergebene Literale -> letztes finden
    for(x=0;x<oplist.size();x++){
        if(oplist[x].g->key>lit) lit = oplist[x].g->key;
    }

    for(x=0;x<oplist.size();x++){
        if(0==oplist[x].g->key){
            lit++;
            oplist[x].g->key = lit;
            oplist[x].operand1 = oplist[x].operand1ptr->key;
            oplist[x].operand2 = oplist[x].operand2ptr->key;
        }else{
            oplist[x].operand1 = NULL;
            oplist[x].operand2 = NULL;
        }
    }
    return oplist;
}

class CNF
{
    public:
        CNF();
        ~CNF();
        signed noLit;
        signed addLit();
        signed ione();
        signed izero();
        signed iand(signed a, signed b);
        signed inot(signed a);
        signed ior(signed a, signed b);
        signed ixor(signed a, signed b);
        void miter(signed a, signed b);
        void iout(signed a);
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

signed CNF::ione()
{
    signed l;
    Sat_Clause_t c;
    l = addLit();
    c.Literal.clear(); c.Literal.push_back(l);
    Clause.push_back(c);
    return l;
}


signed CNF::izero()
{
    signed l;
    Sat_Clause_t c;
    l = addLit();
    c.Literal.clear(); c.Literal.push_back(-l);
    Clause.push_back(c);
    return l;
}

signed CNF::iand(signed a, signed b)
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

signed CNF::inot(signed a)
{
    signed y;
    y = addLit();
    Sat_Clause_t c;
    c.Literal.clear(); c.Literal.push_back(a); c.Literal.push_back(y);     Clause.push_back(c);
    c.Literal.clear(); c.Literal.push_back(-a); c.Literal.push_back(-y);   Clause.push_back(c);
    return y;
}

signed CNF::ior(signed a, signed b)
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

signed CNF::ixor(signed a, signed b)
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

void CNF::iout(signed a)
{
    Sat_Clause_t c;
    c.Literal.clear(); c.Literal.push_back(a);   Clause.push_back(c);
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

CNF oplist2cnf(vector<operation_t> oplist)
{
    unsigned x;
    signed ind;
    CNF cnf;
    signed l1, l2;

    for(x=0;x<oplist.size();x++){
        if(INP==oplist[x].op){
            if(0==oplist[x].g->Literal){
                oplist[x].g->Literal = cnf.addLit();
            }
        }
    }
cout << endl;
    for(x=0;x<oplist.size();x++){
        l1 = 0;
        l2 = 0;
        if(oplist[x].operand1ptr) l1 = oplist[x].operand1ptr->Literal;
        if(oplist[x].operand2ptr) l2 = oplist[x].operand2ptr->Literal;
        cout << oplist[x].op << " " << oplist[x].g->Literal << " " << l1 << " " << l2 << endl;
    }
cout << endl;
    for(x=0;x<oplist.size();x++){
        if(AND==oplist[x].op){
            oplist[x].g->Literal = cnf.iand(oplist[x].operand1ptr->Literal, oplist[x].operand2ptr->Literal);
        }
        if(OR==oplist[x].op){
            oplist[x].g->Literal = cnf.ior(oplist[x].operand1ptr->Literal, oplist[x].operand2ptr->Literal);
        }
        if(XOR==oplist[x].op){
            oplist[x].g->Literal = cnf.ixor(oplist[x].operand1ptr->Literal, oplist[x].operand2ptr->Literal);
        }
        if(NOT==oplist[x].op){
            oplist[x].g->Literal = cnf.inot(oplist[x].operand1ptr->Literal);
        }
    }
cout << endl;
    for(x=0;x<oplist.size();x++){
        l1 = 0;
        l2 = 0;
        if(oplist[x].operand1ptr) l1 = oplist[x].operand1ptr->Literal;
        if(oplist[x].operand2ptr) l2 = oplist[x].operand2ptr->Literal;
        cout << oplist[x].op << " " << oplist[x].g->Literal << " " << l1 << " " << l2 << endl;
    }
cout << endl;
    cnf.iout(oplist[x-1].g->Literal);

    return cnf;

}


int main()
{
    Sat_Instance_t SAT;
    Sat_Instance_t Solution;
    unsigned x;
    CNF testCNF;
    graph g1(1), g2(2), g3(3), g4(4);
    graph g5(5), g6, g7;

    vector<operation_t> oplist;



    g6 = g1 & g2 & g1 & g3 & g4;


    g7 = (g1 & g2 & g1 | g3 & g4) ^ (g1 & g2 & g1 | g3 & g4);



    oplist = oplist_assign_literals(oplist);
    oplist = graph2op(oplist, g7);

    for(x=0;x<oplist.size();x++){
        cout << oplist[x].op << " " << oplist[x].g->key << " " << oplist[x].operand1 << " " << oplist[x].operand2 << endl;
    }

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
