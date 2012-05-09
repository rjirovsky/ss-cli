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


#include "databasemanager.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

DatabaseManager::DatabaseManager()
{
    
}

void DatabaseManager::loadDatabase(const string& path, const string& key) throw(logic_error){
    
    m_db = new Database();
    m_key = key;
    m_path = path;
    
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
            if (header != Database::HEADER){
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
            
            m_db->insertItem(item);
        }
        dbFile.close();
        
    } else {
        throw ifstream::failure("Cannot read database file! Check your premissions.");
    }
    
}

void DatabaseManager::createDatabase(const string& path, const string& key) throw(exception)
{
    ofstream dbFile;
    dbFile.open(path.c_str(), ofstream::out);
    
    if (dbFile.is_open())
    {
        dbFile << Database::HEADER << endl;
        
        dbFile << m_hash << endl;   // hash must be computed here
        
        dbFile << Database::CAPTION;
        
        dbFile.close();
    } else {
        throw ofstream::failure("Cannot write database file! Check your premissions.");
    }
}

void DatabaseManager::closeDatabase()
{
    delete(m_db);
    m_db = NULL;
    
    m_key.clear();
    m_hash.clear();
    m_path.clear();
}


void DatabaseManager::saveDatabase()
{
    if (m_db){
        ofstream dbFile;
        dbFile.open(m_path.c_str(), ofstream::out|ofstream::trunc);
        
        if (dbFile.is_open())
        {              
            dbFile << Database::HEADER << endl;            
            dbFile << m_hash << endl; //temporary
            dbFile << "group name login password" << endl;
            
            list<Item*> items = m_db->getAllItems();
            
            for (list<Item*>::iterator iterator = items.begin(), end = items.end(); iterator != end; ++iterator) {
                dbFile << (**iterator).getGroup() << " " << (**iterator).getName() << " " << (**iterator).getLogin() << " " 
                << (**iterator).getPassword() << endl;        
            }
            
            dbFile.close();

        } else {
            throw ofstream::failure("Cannot write database file! Check your premissions.");
        }
    } else {
        throw runtime_error("Database is NOT opened!");
    }
}


void DatabaseManager::addItem(string group, const string& name, string login, const string& password) throw(invalid_argument) {
  
    if (group.empty()) group = "default"; 
    if (login.empty()) login = "none";
    
    
    if (name.empty() || password.empty()){
        throw invalid_argument("Name and password cannot be empty!");
    }
    
    if (!m_db->getItemByName(name)){
        Item* workingItem = new Item(group, name, m_db->encrypt(login), m_db->encrypt(password));
        m_db->insertItem(workingItem);
    } else {
        throw invalid_argument("Such name ( " + name + " ) already in database!");
    }
}

void DatabaseManager::removeItem(const string& name) throw(invalid_argument)
{
    Item* item = m_db->getItemByName(name);
    
    if (item){
        m_db->deleteItem(item);
    } else {
        throw invalid_argument("No entry with this name ( " + name + " ) in database!");
    }
    
    delete item;
    item = NULL;
}

void DatabaseManager::editItem(string group, const string& name, string login, const string& password) throw(invalid_argument)
{
    if (group.empty()) group = "default"; 
    if (login.empty()) login = "none";
    
    if (name.empty() || password.empty()){
        throw invalid_argument("Name and password cannot be empty!");
    }
    
    Item* item = m_db->getItemByName(name);
    if (item){
        item->setGroup(group);
        item->setLogin(m_db->encrypt(login));
        item->setPassword(m_db->encrypt(password));
    } else
        throw invalid_argument("Such name ( " + name + " ) not in database!");
}



void DatabaseManager::printItemsByName(const string& name) const
{
    list<Item*> itemsList = m_db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        if ((*iterator)->getName().find(name) != string::npos){
            cout << (**iterator) << endl;      
        }
    }
}
void DatabaseManager::printItemsByGroup(const string& group) const
{
    list<Item*> itemsList = m_db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        if ((*iterator)->getGroup().find(group) != string::npos){
            cout << (**iterator) << endl;      
        }
    }
}

void DatabaseManager::printItemByName(const string& name) const
{
    cout << m_db->getItemByName(name) << endl;
}


void DatabaseManager::printAllItems() const
{
    cout << "Entries for database in " << m_db->getPath() << ":" << endl;
    cout << "Name " << "(group)" << endl; 
    
    list<Item*> itemsList = m_db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        cout << (**iterator) << endl;        
    }

}


void DatabaseManager::printAllItemsWithSecrets() const
{
    list<Item*> itemsList = m_db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        cout << (**iterator) << endl << m_db->decrypt((**iterator).getLogin()) 
             << " " << m_db->decrypt((**iterator).getPassword()) << endl;        
    }
    
}

DatabaseManager::~DatabaseManager()
{

}

