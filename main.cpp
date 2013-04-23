
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

int main()
{
    Sat_Instance_t SAT;
    Sat_Instance_t Solution;
    unsigned x;


    SAT = readCNF("C:\\Projekte\\C++\\SAT\\easy\\aim-50-1_6-yes1-1.cnf");


    SAT = Sat_Update_Database(SAT);
    print_SAT_Instance_Infos(SAT);

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
