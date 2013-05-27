
#include "booleq2cnf.h"
#include <iostream>

graph::graph()
{
    operation = INP;
    operand1 = NULL;
    operand1key = 0;
    operand2 = NULL;
    operand2key = 0;
    Literal = 0;
    self = this;
}


graph::~graph()
{

}

graph graph::operator&(graph x)
{
    graph* gr;
    gr = new graph;
    gr->self = gr;
    gr->operation = AND;
    gr->operand1 = this->self;
    gr->operand2 = x.self;
    return *gr;
}

graph graph::operator|(graph x)
{
    graph* gr;
    graph* g;
    gr = new graph;
    gr->self = gr;
    gr->operation = OR;
    gr->operand1 = this->self;
    gr->operand2 = x.self;
    return *gr;
}

graph graph::operator^(graph x)
{
    graph* gr;
    graph* g;
    gr = new graph;
    gr->self = gr;
    gr->operation = XOR;
    gr->operand1 = this->self;
    gr->operand2 = x.self;
    return *gr;
}

graph graph::operator~()
{
    graph* gr;
    gr = new graph;
    gr->self = gr;
    gr->operation = NOT;
    gr->operand1 = this->self;
    gr->operand2 = NULL;
    return *gr;
}

graph grSET()
{
    graph* gr;
    gr = new graph;
    gr->self = gr;
    gr->operation = SET;
    gr->operand1 = NULL;
    gr->operand2 = NULL;
    return *gr;
}

graph grCLR()
{
    graph* gr;
    gr = new graph;
    gr->self = gr;
    gr->operation = CLR;
    gr->operand1 = NULL;
    gr->operand2 = NULL;
    return *gr;
}


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
    CNF cnf;

    for(x=0;x<oplist.size();x++){
        if(INP==oplist[x].op){
            if(0==oplist[x].g->Literal){
                oplist[x].g->Literal = cnf.addLit();
            }
        }
    }

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
        if(SET==oplist[x].op){
            oplist[x].g->Literal = cnf.ione();
        }
        if(CLR==oplist[x].op){
            oplist[x].g->Literal = cnf.izero();
        }
    }

    cnf.iout(oplist[x-1].g->Literal);

    return cnf;
}

