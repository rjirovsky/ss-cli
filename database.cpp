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
#include <fcntl.h>

const string Database::HEADER = "SAFE_STORAGE";

Database::Database()
{
    opened = false;
    modified = false;   
}

void Database::openDatabase(const string& path, const string& key) throw(exception)
{      
    if (isOpened()){
        throw logic_error("Another database already opened!");
    }
    
    m_path = path;
    m_key = key;
          
    parseDatabaseFile(path);        
    
    opened = true;
}

void Database::parseDatabaseFile(const string& path) throw(exception)
{   
    
    stringstream line;
    string in;
    string header;
    string group, name, login, password;
    
    ifstream dbFile;
    dbFile.open(m_path.c_str(),  fstream::in);
    
    if (dbFile.is_open())
    {
        if (dbFile.good()){
            getline (dbFile,header);
            if (header != HEADER){
                throw runtime_error("Invalid database header!");
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
        throw ifstream::failure("Cannot read database file!");
    }
}

void Database::closeDatabase()
{
    opened = false;
    modified = false;
    items.clear();
    m_key.clear();
    m_hash.clear();
    m_path.clear();
}

void Database::saveDatabase()throw(exception)
{
    if (isOpened()){
        ofstream dbFile;
        try{
            dbFile.open(m_path.c_str(), ofstream::out|ofstream::trunc);
            dbFile << HEADER << endl;
            
            dbFile << "HASH"; //temporary
            
            dbFile << "group name login password";
            
            for (list<Item*>::iterator iterator = items.begin(), end = items.end(); iterator != end; ++iterator) {
                dbFile << (**iterator).getGroup() << (**iterator).getName() << (**iterator).getLogin() 
                << (**iterator).getPassword() << endl;        
            }
            
            dbFile.close();
        } catch (ofstream::failure ex){
            throw;
        }
        
        modified = false;
    } else {
        throw logic_error("Database is NOT opened!");
    }
}

void Database::createDatabase(const string& path, const string& key) throw(exception)
{
    ofstream dbFile;
    dbFile.open(path.c_str(), ofstream::out);
    
    if (dbFile.is_open())
    {
        dbFile << HEADER << endl;
        
        dbFile << "HASH";   //temporary
        
        dbFile << "group name login password";
        
        dbFile.close();
    } else {
        throw ofstream::failure("Cannot write database file! Check your premissions.");
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
    modified = true;
}

void Database::deleteItem(Item* item)
{
    items.remove(item); 
    modified = true;
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

