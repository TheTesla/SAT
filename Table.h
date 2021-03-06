/* Klasse verwaltet string-Tabellen
 *
 * Copyright (C) 2012 Stefan Helmert <stefan.helmert@gmx.net
 *
 *
 * loadTable() liest Datensatz aus Textdatei ein, Komma als Spaltentrenner
 * Leerzeichen am Anfang und Ende eines Eintrags werden entfernt

 table-class
 - read from csv-file (pin-file)
 - search-methods
 - method to erase empty rows


 */

#ifndef TABLE_H
#define TABLE_H


#include <cstdlib>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

class Table
{
    public:
        Table();
        ~Table();
        void loadTable(ifstream &File);
        string Tableread(int row, int col);
        char delimiter;
        int findrow(string find, int col=0, int firstrow=0);
        int findcol(string find, int row=0, int firstcol=0);
        void eraseemptyrows(void);
    private:
        string* tabstr;
        int cols;
        int rows;
        void findtablesize(ifstream &File);

};

#endif
