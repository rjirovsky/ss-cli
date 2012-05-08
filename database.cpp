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


#include "database.h"
#include "item.h"
#include <fstream>

#include <sstream>

Database::Database(const string& path, const string& key):m_path(path),m_key(key)
{
    
    fstream dbFile(m_path.c_str());
    
    parseDatabaseFile(dbFile);
    

}

void Database::parseDatabaseFile(fstream& dbFile) throw(logic_error)
{
    const string HEADER = "SAFE_STORAGE";
    
    stringstream line;
    string in;
    string header;
    string group, name, login, password;
    
    if (dbFile.is_open())
    {
        if (dbFile.good()){
            getline (dbFile,header);
            if (header != HEADER){
                throw logic_error("Invalid database header!");
            }
        }
        if (dbFile.good()){
            getline (dbFile,m_hash);
        }
        if (dbFile.good()){ //params line
            getline (dbFile,in);
        }
        while ( dbFile.good() )
        {
            getline (dbFile,in);
            line.str(in);
            line >> group;
            line >> name;
            line >> login;
            line >> password;
            line.clear();
            
            Item* item = new Item(group,name,login,password);
            
            insertItem(item);
        }
        dbFile.close();
    } else {
        throw invalid_argument("Cannot read database file!");
    }
}


Item* Database::getItemByName(const string& name)
{
    Item* item = NULL;
    for (list<Item*>::iterator iterator = items.begin(), end = items.end(); iterator != end; ++iterator) {
        if ((*iterator)->getName() == name){
            item = *iterator;
            break;
        }
    }
    return item;
}

void Database::insertItem(Item* item)
{    
    items.push_back(item);    
}

void Database::deleteItem(Item* item)
{
    items.remove(item);    
}


string Database::encrypt(string str)
{
    return str;
}

string Database::decrypt(string str)
{
    return str;
}

Database::~Database()
{

}

