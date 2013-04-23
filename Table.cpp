/* Klasse verwaltet string-Tabellen
 *
 * Copyright (C) 2012 Stefan Helmert <stefan.helmert@gmx.net>
 *
 *
 * loadTable() liest Datensatz aus Textdatei ein, Komma als Spaltentrenner
 * Leerzeichen am Anfang und Ende eines Eintrags werden entfernt
 */

#include "Table.h"
#include <iostream>

Table::Table()
{
    delimiter = ',';
}

Table::~Table()
{
}

void Table::loadTable(ifstream &File)
{
    int col, row, i, pos, lastpos, endpos;
    string remainingstr, line;

    findtablesize(File); // wegen dynamischer Speicherverwaltung
    tabstr = new string[rows*cols];
    col = 0;
    row = 0;
    if(File.is_open()){
        while(File.good()){
            getline(File, line);
            remainingstr = line;
            for(col=0;col<cols;col++){
                pos = remainingstr.find_first_not_of(" ");
                if(pos < 0) pos = 0;
                remainingstr = remainingstr.substr(pos);
                endpos = remainingstr.find_first_of(delimiter);
                if(-1==endpos){// letzter Eintrag ohne Komma am Ende
                    tabstr[col+row*cols] = remainingstr;
                    break;
                }
                else{
                    tabstr[col+row*cols] = remainingstr.substr(0,endpos);
                    remainingstr = remainingstr.substr(endpos+1);
                }
                lastpos = tabstr[col+row*cols].find_last_not_of(" ");
                if(lastpos>-1) tabstr[col+row*cols] = tabstr[col+row*cols].substr(0,lastpos+1);
            }
            row++;
        }
    }
}

string Table::Tableread(int row, int col)
{
    if(row>=rows || col>cols || row<0 || col<0) return "";
    return tabstr[col + row*cols];
}

void Table::findtablesize(ifstream &File)
{
    string line;
    rows = 0;
    cols = 0;
    int tmp;
    if(File.is_open()){
        while(File.good()){
            getline(File, line);
            rows++;
            tmp = count(line.begin(),line.end(),delimiter);
            if(tmp>cols) cols = tmp;
        }
    cols++; // eine Zeile mehr als Trenner
    File.clear();
    File.seekg(0,ios::beg);
    }
}

int Table::findrow(string find, int col, int firstrow)
{
    int row;
    string str;
    for(row=firstrow;row<rows;row++){
        str = this->Tableread(row,col);
        if(0 == str.compare(find)) return row;
    }
    return -1;
}

int Table::findcol(string find, int row, int firstcol)
{
    int col;
    string str;
    for(col=firstcol;col<cols;col++){
        str = this->Tableread(row,col);
        if(0 == str.compare(find)) return col;
    }
    return -1;
}

void Table::eraseemptyrows(void)
{
    int rrow, wrow, col;
    bool colempty;
    wrow = 0;
    string str;
    for(rrow=0;rrow<rows;rrow++){
        colempty = true;
        for(col=0;col<cols;col++){
            str = this->Tableread(rrow, col);
            tabstr[col + wrow*cols] = str;
            if(0 != str.compare("")) colempty = false;
        }
        if(false == colempty) wrow++;
    }
}
