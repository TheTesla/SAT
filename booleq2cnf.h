#ifndef BOOLEQ2CNF_H_INCLUDED
#define BOOLEQ2CNF_H_INCLUDED

#include <vector>
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
        ~graph();
        graph* operand1;
        graph* self;
        signed operand1key;
        graph* operand2;
        signed operand2key;
        signed Literal;
        optype_t operation;
        graph operator& (graph x);
        graph operator| (graph x);
        graph operator^ (graph x);
        graph operator~ ();


};

struct operation_t
{
    optype_t op;
    graph* g;
    signed operand1;
    graph* operand1ptr;
    signed operand2;
    graph* operand2ptr;
};

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

vector<operation_t> graph2op(vector<operation_t> oplist, graph& g);
Sat_Instance_t CNFobj2Sat(CNF CNFobj);
CNF oplist2cnf(vector<operation_t> oplist);

#endif // BOOLEQ2CNF_H_INCLUDED
