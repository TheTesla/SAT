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

using namespace std;

//NEU Sat-Instanz



Sat_Instance_t readCNF(string CNFFilename)
{
    Sat_Instance_t Sat_Instance;
    Sat_Clause_t Clause;
    ifstream iCNFfile;
    string tabentry;
    Table CNFtab;
    unsigned tabrow, tabcol, clausenumb, maxlit;
    signed litnumb;

    iCNFfile.open(CNFFilename.c_str());
    CNFtab.delimiter = ' ';
    CNFtab.loadTable(iCNFfile);

    // Einlesen der SAT-Instanz
    // Ermitteln der Anzahl an Klauseln und Literale
    tabrow = CNFtab.findrow("p",0,0);
    tabrow++;
    clausenumb = 0;
    litnumb = 0;
    maxlit = 0;

    Sat_Instance.noEliminatedClauses = 0;
    Sat_Instance.noUnitClauses = 0;
    Sat_Instance.noSinglepolLiterals = 0;
    Sat_Instance.WorkingLiteral = 0;

    while("" != CNFtab.Tableread(tabrow,0)){
        tabcol = 0;
        tabentry = 1;
        Clause.noLiterals = 0;
        Clause.eliminated = false;
        Clause.Literal.clear();
        while(1){
            tabentry = CNFtab.Tableread(tabrow,tabcol);
            if(tabentry == "0") break;
            if(!(istringstream(tabentry) >> litnumb)) break;
            Clause.Literal.push_back(litnumb);
            if(abs(litnumb) > maxlit){
                maxlit = abs(litnumb);
            }
            tabcol++;
        }
        Clause.noLiterals = tabcol;
        if(1 == tabcol) Sat_Instance.noUnitClauses++;
        Sat_Instance.Clause.push_back(Clause);
        tabrow++;
        clausenumb++;
    }

    // Grundinitialisierung der SAT-Instanz mit Klausel und Literalanzahl und sonstige
    Sat_Instance.consistent = false; // counter und sonstige Parameter noch nicht generiert
    Sat_Instance.noClauses = clausenumb-1;
    Sat_Instance.noLiterals = maxlit;

    return Sat_Instance;
}

Sat_Instance_t Sat_Update_Database(Sat_Instance_t Sat_Instance)
{
     // Update Literal, SinglePolLit, consistent
    unsigned i,j;
    signed Lit;
    Sat_Literal_t EmptyLiteral;
    EmptyLiteral.pos.noClauses = 0;
    EmptyLiteral.pos.set = false;
    EmptyLiteral.neg.noClauses = 0;
    EmptyLiteral.neg.set = false;

    Sat_Instance.Literal.assign(Sat_Instance.noLiterals+1, EmptyLiteral);
    Sat_Instance.SAT = false;
    Sat_Instance.UNSAT = false;
    Sat_Instance.WorkingLiteral = 0;


    // Aufbau der Inverssuche/Counter Literal[]
    for(i=0;i<Sat_Instance.noClauses;i++){
        for(j=0;j<Sat_Instance.Clause[i].noLiterals;j++){
            Lit = Sat_Instance.Clause[i].Literal[j];
            if(Lit>0){
                Sat_Instance.Literal[Lit].pos.inClause.push_back(i);
                Sat_Instance.Literal[Lit].pos.Index.push_back(j);
                Sat_Instance.Literal[Lit].pos.Cleliminated.push_back(false);
                Sat_Instance.Clause[i].Index.push_back(Sat_Instance.Literal[Lit].pos.noClauses);
                Sat_Instance.Clause[i].Litdeleted.push_back(false);
                Sat_Instance.Literal[Lit].pos.noClauses++;
            }else{
                Sat_Instance.Literal[-Lit].neg.inClause.push_back(i);
                Sat_Instance.Literal[-Lit].neg.Index.push_back(j);
                Sat_Instance.Literal[-Lit].neg.Cleliminated.push_back(false);
                Sat_Instance.Clause[i].Index.push_back(Sat_Instance.Literal[-Lit].neg.noClauses);
                Sat_Instance.Clause[i].Litdeleted.push_back(false);
                Sat_Instance.Literal[-Lit].neg.noClauses++;
            }
        }
    }

    // Suche nach Single Polarity

    for(i=1;i<Sat_Instance.noLiterals;i++){
        if((0 == Sat_Instance.Literal[i].pos.noClauses) && (0 == Sat_Instance.Literal[i].pos.noClauses)){
            cout << "Sat_Update_Database: Literal " << i << " nicht vorhanden" << endl;
        }
        else if ((0 == Sat_Instance.Literal[i].pos.noClauses) && (0 != Sat_Instance.Literal[i].pos.noClauses)){
            Sat_Instance.SinglepolLiterals.push_back(-i);
            Sat_Instance.noSinglepolLiterals++;
        }
        else if ((0 != Sat_Instance.Literal[i].pos.noClauses) && (0 == Sat_Instance.Literal[i].pos.noClauses)){
            Sat_Instance.SinglepolLiterals.push_back(i);
            Sat_Instance.noSinglepolLiterals++;
        }
        else{

        }
    }
    Sat_Instance.consistent = true;
    return Sat_Instance;
}

void print_CNF(Sat_Instance_t Sat_Instance)
{
    unsigned Literali, Clause;
    for(Clause=0;Clause<Sat_Instance.noClauses;Clause++){
        if(Sat_Instance.Clause[Clause].eliminated) cout << "- ";
        cout << Clause << ": ";
        for(Literali=0;Literali<Sat_Instance.Clause[Clause].noLiterals;Literali++){
            cout << Sat_Instance.Clause[Clause].Literal[Literali] << " ";
        }
        cout << endl;
    }
}

void print_Counters(Sat_Instance_t Sat_Instance)
{
    unsigned x, y;
    cout << "Positive Literale: " << endl;
    for(x=0;x<=Sat_Instance.noLiterals;x++){
        cout << x << ": ";
        for(y=0;y<Sat_Instance.Literal[x].pos.noClauses;y++){
            cout << Sat_Instance.Literal[x].pos.inClause[y] << " ";
        }
        cout << endl;
    }
    cout << "Negative Literale: " << endl;
    for(x=0;x<=Sat_Instance.noLiterals;x++){
        cout << x << ": ";
        for(y=0;y<Sat_Instance.Literal[x].neg.noClauses;y++){
            cout << Sat_Instance.Literal[x].neg.inClause[y] << " ";
        }
        cout << endl;
    }
}

void print_SAT_Instance_Infos(Sat_Instance_t Sat_Instance)
{
    unsigned x;
    cout << "Number of Clauses: " << Sat_Instance.noClauses << endl;
    cout << "Number of Literals: " << Sat_Instance.noLiterals << endl;
    cout << "Number of eliminated Clauses: " << Sat_Instance.noEliminatedClauses << endl;
    cout << "Number of Unit Clauses: " << Sat_Instance.noUnitClauses << endl;
    cout << "Number of single polarity Literals: " << Sat_Instance.noSinglepolLiterals << endl;
    cout << "Konsistent: " << Sat_Instance.consistent << endl;

    cout << "Unit Clauses: ";
    for(x=0;x<Sat_Instance.noUnitClauses;x++){
        cout << "[" << Sat_Instance.UnitClauses[x] << "] ";
    }
    cout << endl;
    cout << "Single polarity Literals: ";
    for(x=0;x<Sat_Instance.noSinglepolLiterals;x++){
        cout << Sat_Instance.SinglepolLiterals[x] << " ";
    }
    cout << endl;
    cout << "Working Literal: " << Sat_Instance.WorkingLiteral << endl;
}


Sat_Instance_t Sat_Apply_WorkingLiteral(Sat_Instance_t Sat_Instance)
{
    signed Literal, LitelCla;
    unsigned Clause, Index, Lindex, Cindex, x;
    if(Sat_Instance.WorkingLiteral){
        //Sat_Instance.consistent = false; // -- Single Polariy Literal werden bei Elimination einer Clause geupdated
        Literal = Sat_Instance.WorkingLiteral;
        if(Literal>0){
            if(Sat_Instance.Literal[Literal].pos.set) return Sat_Instance;
            Sat_Instance.Literal[Literal].pos.set = true;
            Cindex = 0;
            while(0!=Sat_Instance.Literal[Literal].pos.noClauses){
                while(Sat_Instance.Literal[Literal].pos.Cleliminated[Cindex]) Cindex++;
                Clause = Sat_Instance.Literal[Literal].pos.inClause[Cindex];
                Sat_Instance.Clause[Clause].eliminated = true;
                Sat_Instance.noEliminatedClauses++;
                // Literal-Counter anpassen -- Nach Eliminieren einer Clause
                Lindex = 0;
                while(Sat_Instance.Clause[Clause].noLiterals){
                    Sat_Instance.Clause[Clause].noLiterals--;
                    while(Sat_Instance.Clause[Clause].Litdeleted[Lindex]) Lindex++;
                    LitelCla = Sat_Instance.Clause[Clause].Literal[Lindex];
                    Sat_Instance.Clause[Clause].Litdeleted[Lindex] = true;
                    // positive Literale der elimierten Clause
                    if(LitelCla>0){
                        Index = Sat_Instance.Clause[Clause].Index[Lindex];
                        Sat_Instance.Literal[LitelCla].pos.Cleliminated[Index] = true;
                        Sat_Instance.Literal[LitelCla].pos.noClauses--;
                        if(0==Sat_Instance.Literal[LitelCla].pos.noClauses){
                            if(0!=Sat_Instance.Literal[LitelCla].neg.noClauses){
                                if(Literal!=LitelCla){
                                    Sat_Instance.noSinglepolLiterals++;
                                    Sat_Instance.SinglepolLiterals.push_back(-LitelCla);
                                }
                            }
                        }
                    // negative Literale der elimierten Clause
                    }else{
                        Index = Sat_Instance.Clause[Clause].Index[Lindex];
                        Sat_Instance.Literal[-LitelCla].neg.Cleliminated[Index] = true;
                        Sat_Instance.Literal[-LitelCla].neg.noClauses--;
                        if(0==Sat_Instance.Literal[-LitelCla].neg.noClauses){
                            if(0!=Sat_Instance.Literal[-LitelCla].pos.noClauses){
                                if(Literal!=LitelCla){
                                    Sat_Instance.noSinglepolLiterals++;
                                    Sat_Instance.SinglepolLiterals.push_back(-LitelCla);
                                }
                            }
                        }
                    }
                }
            }
            Lindex = 0;
            Cindex = 0;
            while(0!=Sat_Instance.Literal[Literal].neg.noClauses){
                while(Sat_Instance.Literal[Literal].neg.Cleliminated[Cindex]) Cindex++;
                Clause = Sat_Instance.Literal[Literal].neg.inClause[Cindex];
                Index = Sat_Instance.Literal[Literal].neg.Index[Cindex];
                Sat_Instance.Clause[Clause].noLiterals--;
                Sat_Instance.Clause[Clause].Litdeleted[Index] = true;
                Sat_Instance.Literal[Literal].neg.noClauses--;
                Cindex++;
                if(1==Sat_Instance.Clause[Clause].noLiterals){
                    x = 0;
                    while(Sat_Instance.Clause[Clause].Litdeleted[x]) x++;
                    if(Sat_Instance.UnitClauses.size()){
                        if(Sat_Instance.UnitClauses.back() == -Sat_Instance.Clause[Clause].Literal[x]){
                            Sat_Instance.UNSAT = true;
                            return Sat_Instance;
                        }
                    }
                    Sat_Instance.UnitClauses.push_back(Sat_Instance.Clause[Clause].Literal[x]);
                    Sat_Instance.noUnitClauses++;
                }else if(0==Sat_Instance.Clause[Clause].noLiterals){
                    if(false == Sat_Instance.Clause[Clause].eliminated) Sat_Instance.UNSAT = true;
                    return Sat_Instance;
                }
            }
        }
        else{
            if(Sat_Instance.Literal[-Literal].neg.set) return Sat_Instance;
            Sat_Instance.Literal[-Literal].neg.set = true;
            Cindex = 0;
            while(0!=Sat_Instance.Literal[-Literal].neg.noClauses){
                while(Sat_Instance.Literal[-Literal].neg.Cleliminated[Cindex]) Cindex++;
                Clause = Sat_Instance.Literal[-Literal].neg.inClause[Cindex];
                Sat_Instance.Clause[Clause].eliminated = true;
                Sat_Instance.noEliminatedClauses++;
                // Literal-Counter anpassen -- Nach Eliminieren einer Clause
                Lindex = 0;
                while(Sat_Instance.Clause[Clause].noLiterals){
                    Sat_Instance.Clause[Clause].noLiterals--;
                    while(Sat_Instance.Clause[Clause].Litdeleted[Lindex]) Lindex++;
                    LitelCla = Sat_Instance.Clause[Clause].Literal[Lindex];
                    Sat_Instance.Clause[Clause].Litdeleted[Lindex] = true;
                    // positive Literale der elimierten Clause
                    if(LitelCla>0){
                        Index = Sat_Instance.Clause[Clause].Index[Lindex];
                        Sat_Instance.Literal[LitelCla].pos.Cleliminated[Index] = true;
                        Sat_Instance.Literal[LitelCla].pos.noClauses--;
                        if(0==Sat_Instance.Literal[LitelCla].pos.noClauses){
                            if(0!=Sat_Instance.Literal[LitelCla].neg.noClauses){
                                if(Literal!=LitelCla){
                                    Sat_Instance.noSinglepolLiterals++;
                                    Sat_Instance.SinglepolLiterals.push_back(-LitelCla);
                                }
                            }
                        }
                    // negative Literale der elimierten Clause
                    }else{
                        Index = Sat_Instance.Clause[Clause].Index[Lindex];
                        Sat_Instance.Literal[-LitelCla].neg.Cleliminated[Index] = true;
                        Sat_Instance.Literal[-LitelCla].neg.noClauses--;
                        if(0==Sat_Instance.Literal[-LitelCla].neg.noClauses){
                            if(0!=Sat_Instance.Literal[-LitelCla].pos.noClauses){
                                if(Literal!=LitelCla){
                                    Sat_Instance.noSinglepolLiterals++;
                                    Sat_Instance.SinglepolLiterals.push_back(-LitelCla);
                                }
                            }
                        }
                    }
                }
            }
            Lindex = 0;
            Cindex = 0;
            while(0!=Sat_Instance.Literal[-Literal].pos.noClauses){
                while(Sat_Instance.Literal[-Literal].pos.Cleliminated[Cindex]) Cindex++;
                Clause = Sat_Instance.Literal[-Literal].pos.inClause[Cindex];
                Index = Sat_Instance.Literal[-Literal].pos.Index[Cindex];
                Sat_Instance.Clause[Clause].noLiterals--;
                Sat_Instance.Clause[Clause].Litdeleted[Index] = true;
                Sat_Instance.Literal[-Literal].pos.noClauses--;
                Cindex++;
                if(1==Sat_Instance.Clause[Clause].noLiterals){
                    x = 0;
                    while(Sat_Instance.Clause[Clause].Litdeleted[x]) x++;
                    if(Sat_Instance.UnitClauses.size()){
                        if(Sat_Instance.UnitClauses.back() == -Sat_Instance.Clause[Clause].Literal[x]){
                            Sat_Instance.UNSAT = true;
                            return Sat_Instance;
                        }
                    }
                    Sat_Instance.UnitClauses.push_back(Sat_Instance.Clause[Clause].Literal[x]);
                    Sat_Instance.noUnitClauses++;
                }else if(0==Sat_Instance.Clause[Clause].noLiterals){
                    if(false == Sat_Instance.Clause[Clause].eliminated) Sat_Instance.UNSAT = true;
                    return Sat_Instance;
                }
            }
        }
    }


    if(Sat_Instance.noClauses==(Sat_Instance.noEliminatedClauses)){
        Sat_Instance.SAT = true;
    }
    return Sat_Instance;
}

Sat_Instance_t Sat_Simplify(Sat_Instance_t Sat_Instance)
{
    signed Literal;
    //print_CNF(Sat_Instance);
    if(Sat_Instance.WorkingLiteral){
        Sat_Instance = Sat_Apply_WorkingLiteral(Sat_Instance);
    }

    while((0!=Sat_Instance.noUnitClauses) || (0!=Sat_Instance.noSinglepolLiterals)){
        while(Sat_Instance.noUnitClauses){
            Literal = Sat_Instance.UnitClauses.back();
            Sat_Instance.UnitClauses.pop_back();
            Sat_Instance.noUnitClauses--;
            Sat_Instance.WorkingLiteral = Literal;
            Sat_Instance = Sat_Apply_WorkingLiteral(Sat_Instance);
            if(Sat_Instance.UNSAT) {
                return Sat_Instance;
            }
        }

        while(Sat_Instance.noSinglepolLiterals){
            Literal = Sat_Instance.SinglepolLiterals.back();
            Sat_Instance.SinglepolLiterals.pop_back();
            Sat_Instance.noSinglepolLiterals--;
            Sat_Instance.WorkingLiteral = Literal;
            Sat_Instance = Sat_Apply_WorkingLiteral(Sat_Instance);
            if(Sat_Instance.UNSAT) {
                return Sat_Instance;
            }
        }
    }
    return Sat_Instance;
}


unsigned Sat_min_Lit(Sat_Instance_t Sat_Instance)
{
    unsigned x,y;

    for(x=0;x<Sat_Instance.noClauses;x++){
        // Positive Literale prüfen reicht, da sonst Singe Polarity Literal -> sowieso eliminiert
        if(false==Sat_Instance.Clause[x].eliminated){
            for(y=0;y<Sat_Instance.Clause[x].noLiterals;y++){
                if(false==Sat_Instance.Clause[x].Litdeleted[y]) return Sat_Instance.Clause[x].Literal[y];
            }
        }
    }
}

bool DPLLrec(Sat_Instance_t Sat_Instance)
{

    signed Lit;
    Sat_Instance = Sat_Simplify(Sat_Instance);
    if(true == Sat_Instance.SAT){return true;}
    if(true == Sat_Instance.UNSAT){return false;}
    Lit = Sat_min_Lit(Sat_Instance);
    Sat_Instance.WorkingLiteral = Lit;
    if(true == DPLLrec(Sat_Instance)){return true;}
    Sat_Instance.WorkingLiteral = -Lit;
    if(true == DPLLrec(Sat_Instance)){return true;}
    return false;
}

bool DPLLrec(Sat_Instance_t Sat_Instance, Sat_Instance_t* Solution)
{

    signed Lit;
    Sat_Instance = Sat_Simplify(Sat_Instance);
    *Solution = Sat_Instance;
    if(true == Sat_Instance.SAT){return true;}
    if(true == Sat_Instance.UNSAT){return false;}
    Lit = Sat_min_Lit(Sat_Instance);
    Sat_Instance.WorkingLiteral = Lit;
    if(true == DPLLrec(Sat_Instance, Solution)){return true;}
    Sat_Instance.WorkingLiteral = -Lit;
    if(true == DPLLrec(Sat_Instance, Solution)){return true;}
    return false;
}
