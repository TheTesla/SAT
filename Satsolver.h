#ifndef SATSOLVER_H_INCLUDED
#define SATSOLVER_H_INCLUDED

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include "table.h"
#include <sstream>

// no = number of
struct Sat_Clause_t
{
    unsigned noLiterals;
    bool eliminated;
    vector<signed> Literal;
    vector<bool> Litdeleted;
    vector<unsigned> Index;
};

struct Sat_Lit_pol_t
{
    vector<unsigned> inClause;
    vector<bool> Cleliminated;
    vector<unsigned> Index;
    unsigned noClauses;
    bool set;
};

struct Sat_Literal_t
{
    Sat_Lit_pol_t pos;
    Sat_Lit_pol_t neg;
};

struct Sat_Instance_t
{
    unsigned noClauses;
    unsigned noEliminatedClauses;
    unsigned noLiterals;
    vector<Sat_Clause_t> Clause;
    vector<Sat_Literal_t> Literal;
    vector<signed> UnitClauses;
    unsigned noUnitClauses;
    vector<signed> SinglepolLiterals;
    unsigned noSinglepolLiterals;
    bool consistent;
    signed WorkingLiteral;
    bool UNSAT;
    bool SAT;
};

Sat_Instance_t readCNF(string CNFFilename);
Sat_Instance_t Sat_Update_Database(Sat_Instance_t Sat_Instance);
void print_CNF(Sat_Instance_t Sat_Instance);
void print_Counters(Sat_Instance_t Sat_Instance);
void print_SAT_Instance_Infos(Sat_Instance_t Sat_Instance);
Sat_Instance_t Sat_Apply_WorkingLiteral(Sat_Instance_t Sat_Instance);
Sat_Instance_t Sat_Simplify(Sat_Instance_t Sat_Instance);
unsigned Sat_min_Lit(Sat_Instance_t Sat_Instance);
bool DPLLrec(Sat_Instance_t Sat_Instance);
bool DPLLrec(Sat_Instance_t Sat_Instance, Sat_Instance_t* Solution);

#endif // SATSOLVER_H_INCLUDED
