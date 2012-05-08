/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Radek Jirovský <email>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "item.h"
#include "list"
#include <stdexcept>

#ifndef DATABASE_H
#define DATABASE_H

using namespace std;

class Database
{

public:
    Database(const string& path, const string& key);
    void insertItem(Item* item);
    void deleteItem(Item* item);

    void parseDatabaseFile(fstream& dbFile) throw(logic_error);
    
    Item* getItemByName(const string& name);
    list<Item*> getAllItems(){return items;}
    
    string encrypt(string str);
    string decrypt(string str);
    
    virtual ~Database();
    
private:
    list<Item*> items;
    string m_key;
    string m_hash;
    string m_path;
    const string HEADER;
};

#endif // DATABASE_H
