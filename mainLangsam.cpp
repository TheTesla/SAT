#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include "table.h"
#include <sstream>

using namespace std;


// bestimmt ob Literal in normaler (pos) oder inverser (neg) Form vorkommt
struct literal_t
{
    bool pos;
    bool neg;
};

// Klausel besteht aus Literalen
struct clause_t
{
    vector<literal_t> literal;
};

// SAT-Instanz besteht aus Klauseln
struct sat_inst_t
{
    vector<clause_t> clause;
};

struct list_of_clauses_t
{
    vector<unsigned long> clauseindex;
};

// umgekehrte Zuordnung zwischen Literal und Klausel
// liefert eine List an Klauseln, in der das gesuchte Literal enthalten ist
// es wird nach normal und invertiert unterschieden
struct clauses_of_literal_t
{
    vector<list_of_clauses_t> clauses_pos_lit;
    vector<list_of_clauses_t> clauses_neg_lit;
};

void print_col(clauses_of_literal_t col)
{
    unsigned long x, y;
    cout << "clauses of positive literal:" << endl;
    for(x=0;x<col.clauses_pos_lit.size();x++){
        cout << x << ": ";
        for(y=0;y<col.clauses_pos_lit.at(x).clauseindex.size();y++){
            cout << col.clauses_pos_lit.at(x).clauseindex.at(y) << " ";
        }
         cout << endl;
    }

    cout << "clauses of negative literal:" << endl;
    for(x=0;x<col.clauses_neg_lit.size();x++){
        cout << x << ": ";
        for(y=0;y<col.clauses_neg_lit.at(x).clauseindex.size();y++){
            cout << col.clauses_neg_lit.at(x).clauseindex.at(y) << " ";
        }
         cout << endl;
    }
}

unsigned long max_ul(unsigned long x, unsigned long y)
{
    if(x>y){
        return x;
    }
    return y;
}

unsigned long last_literal(sat_inst_t satinstance)
{
    signed long x, y, literal_nmbr, max_lit;
    literal_nmbr = 0;
    max_lit = 0;
    for(x=0;x<satinstance.clause.size()-1;x++){
        for(y=satinstance.clause.at(x).literal.size()-1;y>=0;y--){
            if(satinstance.clause.at(x).literal.at(y).pos){
                literal_nmbr = y;
                break;
            }
            if(satinstance.clause.at(x).literal.at(y).neg){
                literal_nmbr = y;
                break;
            }
        }
        max_lit = max_ul(max_lit, literal_nmbr);
    }
    return max_lit;
}

// diese Zuordnung aus SAT-Instanz erzeugen
clauses_of_literal_t organise_clauses_by_literal(sat_inst_t satinstance)
{
    clauses_of_literal_t col;
    list_of_clauses_t empty_list_of_clauses;
    unsigned long x, y, litereral_nmbr;
    litereral_nmbr = last_literal(satinstance);
    col.clauses_neg_lit.assign(litereral_nmbr+1,empty_list_of_clauses);
    col.clauses_pos_lit.assign(litereral_nmbr+1,empty_list_of_clauses);
    for(x=0;x<satinstance.clause.size();x++){
        for(y=0;y<satinstance.clause.at(x).literal.size();y++){
                if(satinstance.clause.at(x).literal.at(y).pos){
                    col.clauses_pos_lit.at(y).clauseindex.push_back(x); // Klausel-Nummer eintragen, bei pos. Literal
                }
                if(satinstance.clause.at(x).literal.at(y).neg){
                    col.clauses_neg_lit.at(y).clauseindex.push_back(x); // Klausel-Nummer eintragen, bei neg. Literal
                }
         }
    }
    print_col(col);
    return col;
}




void print_CNF(sat_inst_t CNF)
{
    unsigned long x, y;
    for(x=0;x<CNF.clause.size();x++){
        for(y=0;y<CNF.clause.at(x).literal.size();y++){
                if(CNF.clause.at(x).literal.at(y).pos){
                    cout << y << " ";
                }
                if(CNF.clause.at(x).literal.at(y).neg){
                    cout << "-" << y << " ";
                }
         }
         cout << endl;
    }
}



struct clause_state_t
{
    unsigned long literal_counter;
    bool eliminated;
};

struct clause_state_vec_t
{
    vector<clause_state_t> clause_state;
    literal_t guess;
};

void print_counters(vector<clause_state_t> clause_state)
{
    unsigned long x, y;
    cout << "literal_counter: " << endl;
    for(x=0;x<clause_state.size();x++){
        cout << clause_state.at(x).literal_counter << " ";
    }
    cout << endl;

}

void print_solverstate(vector<clause_state_vec_t> solver_state)
{
    int x;
    for(x=0;x<50;x++){
        if(solver_state.at(x).guess.neg) {cout << "0"; continue;}
        if(solver_state.at(x).guess.pos) {cout << "1"; continue;}
        cout << "-";
    }
    cout << endl;
}


bool SATSolver(sat_inst_t satinstance)
{
    unsigned long x, y, nmbr_of_last_literal;
    clauses_of_literal_t col;
    clause_state_t empty_clause_state;
    empty_clause_state.literal_counter = 0;
    empty_clause_state.eliminated = false;
    vector<clause_state_t> clause_states;
    vector<clause_state_t> initial_clause_state;
    clause_state_vec_t empty_clause_state_vec;
    //empty_clause_state_vec.clause_state = empty_clause_state;
    empty_clause_state_vec.guess.pos = false;
    empty_clause_state_vec.guess.neg = false;
    //vector<literal_t> solution_vec;
    vector<clause_state_vec_t> solver_state;
    col = organise_clauses_by_literal(satinstance);
    print_col(col);
    nmbr_of_last_literal = col.clauses_neg_lit.size();
    if(nmbr_of_last_literal<col.clauses_pos_lit.size()) nmbr_of_last_literal = col.clauses_pos_lit.size();

    // clause_states anlegen
    for(x=0;x<satinstance.clause.size();x++){
        clause_states.push_back(empty_clause_state);
        for(y=1;y<satinstance.clause.at(x).literal.size();y++){
                if(satinstance.clause.at(x).literal.at(y).pos){
                    clause_states.at(x).literal_counter++;
                }
                if(satinstance.clause.at(x).literal.at(y).neg){
                    clause_states.at(x).literal_counter++;
                }
         }
    }

    // counter aus SAT-Instanz initialisieren
    for(x=0;x<clause_states.size();x++){
        initial_clause_state.push_back(empty_clause_state);
        for(y=1;y<satinstance.clause.at(x).literal.size();y++){
            if(satinstance.clause.at(x).literal.at(y).pos){
                initial_clause_state.at(x).literal_counter++;
            }
            if(satinstance.clause.at(x).literal.at(y).neg){
                initial_clause_state.at(x).literal_counter++;
            }
        }

    }

    print_counters(initial_clause_state);

    // UNSAT bei nicht erfüllter Klausel
    for(x=1;x<clause_states.size();x++){
        cout << clause_states.at(x).literal_counter << endl;
        if(0==clause_states.at(x).literal_counter){
            //cout << "false" << endl;
            //return false;
        }
    }

    solver_state.assign(nmbr_of_last_literal, empty_clause_state_vec);

    solver_state.at(0).clause_state = initial_clause_state;


    int k = 0; //---test

    // Lösung raten
    for(x = 1;x < nmbr_of_last_literal;x++){
        solver_state[x].clause_state = solver_state[x-1].clause_state; // eine Stufe tiefer im Baum
        if((false == solver_state[x].guess.pos) && (false == solver_state[x].guess.neg)){
            solver_state[x].guess.neg = true; // Raten: literal = 0
            solver_state[x].guess.pos = false; // Raten: literal = 0
        }
        else if((false == solver_state[x].guess.pos) && (true == solver_state[x].guess.neg)){
            solver_state[x].guess.neg = false; // Raten: literal = 0
            solver_state[x].guess.pos = true; // Raten: literal = 0
        }
        else if((true == solver_state[x].guess.pos) && (false == solver_state[x].guess.neg)){
            if(1 == x) return false; // Alles durchprobiert, keine Lösung -> UNSAT
            solver_state[x].guess.neg = false; // Raten: literal = 0
            solver_state[x].guess.pos = false; // Raten: literal = 0
            x -= 2; // umgekehrtes Literal der einen Stufe schon geprüft -> eine Stufe zurück
            //cout << x << endl;
            k++;
            if(k==100000){
                    k=0;
                    print_solverstate(solver_state);
            }
            continue;
        }
        else{
            cout << "DEBUG: Fehler"<< endl;
        }


        // ---test----
//        x-=k;
//        if(x==9) k=1;

        print_solverstate(solver_state);
//        continue;


        if(solver_state[x].guess.neg){
            for(y=0;y<col.clauses_pos_lit[x].clauseindex.size();y++){ // negatives Literal raten -> positive streichen
               solver_state[x].clause_state[col.clauses_pos_lit[x].clauseindex[y]].literal_counter--;
               if(0 == solver_state[x].clause_state[col.clauses_pos_lit[x].clauseindex[y]].literal_counter){ // Backtracking
                    x -= 1; // die gleiche Stufe noch mal wiederholen mit umgekehrten Literal
                    //cout << "backtrack" << endl;
                    goto outercontinue;
                    continue;
                }
            }
            for(y=0;y<col.clauses_neg_lit[x].clauseindex.size();y++){ // negatives Literal raten -> Klausel mit negativem Literal erfüllt/eliminiert
                //cout << "x= " << x << " y= " << y << " clause= " << col.clauses_neg_lit.at(x).clauseindex.at(y) << endl;
                solver_state[x].clause_state[col.clauses_neg_lit[x].clauseindex[y]].eliminated = true;
                //cout << "clause eleminated" << endl;
            }



            for(y=0;true;y++){
                if(false==solver_state[x].clause_state[y].eliminated) break;
                //cout << y << " " << solver_state.at(x).clause_state.size() << endl;
                if(y==solver_state[x].clause_state.size()-1) return true;
            }
        }
        else{
            for(y=0;y<col.clauses_neg_lit[x].clauseindex.size();y++){ // negatives Literal raten -> positive streichen
                solver_state[x].clause_state[col.clauses_neg_lit[x].clauseindex[y]].literal_counter--;
                if(0 == solver_state[x].clause_state[col.clauses_neg_lit[x].clauseindex[y]].literal_counter){ // Backtracking
                    x -= 1; // die gleiche Stufe noch mal wiederholen mit umgekehrten Literal
                    //cout << "backtrack" << endl;
                    goto outercontinue;
                    continue;
                }
            }
            for(y=0;y<col.clauses_pos_lit[x].clauseindex.size();y++){ // negatives Literal raten -> Klausel mit negativem Literal erfüllt/eliminiert
                solver_state[x].clause_state[col.clauses_pos_lit[x].clauseindex[y]].eliminated = true;
                //cout << "clause eleminated" << endl;
            }
            for(y=0;true;y++){
                if(false==solver_state[x].clause_state[y].eliminated) break;
                if(y==solver_state[x].clause_state.size()-1) return true;
            }
        }
        outercontinue:
        if(0){}
    }
    // Kann hier nie hinkommen, da immer erst ein counter == 0 wird -> continue
    cout << "BUG ?"  << endl;
    return true;
}


int main()
{

    sat_inst_t sat;
    clause_t cl;
    literal_t deflit;
    clauses_of_literal_t col;
    ifstream iCNFfile;
    Table CNFtab;
    int tabrow, tabcol, clausenumb, litnumb;
    string tabentry;

    deflit.pos=false;
    deflit.neg=false;

    cl.literal.assign(60, deflit);
    sat.clause.assign(100, cl);
    //sat.clause.at(0).literal.resize(10);
//    sat.clause.at(0).literal.at(1).neg = true;
//    sat.clause.at(0).literal.at(4).neg = true;
//    sat.clause.at(2).literal.at(3).pos = true;
//    sat.clause.at(1).literal.at(3).neg = true;

    iCNFfile.open("C:\\Projekte\\C++\\SAT\\bin\\DEBUG\\aim-50-1_6-yes1-1.cnf");
    cout << iCNFfile << endl;
    CNFtab.delimiter = ' ';
    CNFtab.loadTable(iCNFfile);

    cout << CNFtab.Tableread(0,0) << endl;
    tabrow = CNFtab.findrow("p",0,0);
    cout << "p " << tabrow << endl;

    tabrow++;
    clausenumb = 0;
    litnumb = 0;
    while("" != CNFtab.Tableread(tabrow,0)){
        tabcol = 0;
        tabentry = 1;
        while(1){
            tabentry = CNFtab.Tableread(tabrow,tabcol);
            if(tabentry == "0") break;
            if(!(istringstream(tabentry) >> litnumb)) break;

            if(litnumb>0){
                sat.clause.at(clausenumb).literal.at(litnumb).pos = true;
            }
            else{
                sat.clause.at(clausenumb).literal.at(-litnumb).neg = true;
            }
            cout << tabentry << endl;
            tabcol++;

        }
        tabrow++;
        clausenumb++;
    }

    print_CNF(sat);
    col = organise_clauses_by_literal(sat);
    //print_col(col);
    if(SATSolver(sat)){
        cout << "SAT" << endl;
    }
    else{
        cout << "UNSAT" << endl;
    }



    cout << "Hello world!   " << sat.clause.at(0).literal.at(0).neg << endl;
    return 0;
}
