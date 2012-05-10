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

const  string DatabaseManager::SEPARATOR = "--------------------------------"; //32
const int    DatabaseManager::COL_WIDTH = 24;

using namespace std;

DatabaseManager::DatabaseManager()
{
    
}

void DatabaseManager::loadDatabase(const string& path, const string& key) throw(exception){
    
    m_db = new Database();
    m_key = key;
    m_path = path;
    
    stringstream line;
    string header, in;
    ifstream dbFile;
    
    dbFile.open(m_path.c_str(),  ifstream::in);
    
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
        if (dbFile.good()){ //throw out params line
            getline (dbFile,in);
        }
        while ( dbFile.good() )
        {

            getline (dbFile,in);    //read one line from file
            
            
            if (!in.empty()){   //skip empty lines
                //parse line
                Item* item = new Item();
                line << in;
                getline (line,in,';');
                item->group = string(in, 0, in.length());
                
                getline (line,in,';');
                item->name = string (in, 0, in.length());
                
                getline (line,in,';');
                item->login = string(in, 0, in.length());
                
                getline (line,in,';');
                item->password = string(in, 0, in.length());
                
                m_db->insertItem(item); //insert to db
            }
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
        
        dbFile << Database::CAPTION << endl;
        
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
            dbFile << Database::CAPTION << endl;
            
            list<Item*> items = m_db->getAllItems();
            
            for (list<Item*>::iterator iterator = items.begin(), end = items.end(); iterator != end; ++iterator) {
                dbFile << (**iterator).group << ";" << (**iterator).name << ";" << (**iterator).login << ";" 
                << (**iterator).password << ";" << endl;        
            }
            
            dbFile.close();

        } else {
            throw ofstream::failure("Cannot write database file! Check your premissions.");
        }
    } else {
        throw runtime_error("Database is NOT opened!");
    }
}

void DatabaseManager::exportDatabase(const string& out) throw(exception)
{
    if (m_db){
        ofstream outFile;
        outFile.open(out.c_str(), ofstream::out|ofstream::trunc);
        
        if (outFile.is_open())
        {              
            outFile << Database::CAPTION << endl;
            
            list<Item*> items = m_db->getAllItems();
            
            for (list<Item*>::iterator iterator = items.begin(), end = items.end(); iterator != end; ++iterator) {
                outFile << (**iterator).group << ";" << (**iterator).name << ";" 
                        << m_db->decrypt((**iterator).login) << ";" 
                        << m_db->decrypt((**iterator).password) << ";" << endl;        
            }
            
            outFile.close();
            
        } else {
            throw ofstream::failure("Cannot write output file! Check your premissions.");
        }
    } else {
        throw runtime_error("Database is NOT opened!");
    }
}

void DatabaseManager::importCSV(const string& input) throw(exception)
{
    stringstream line;    
    string header, in;
    ifstream csvFile;
    int counter = 0;
    
    csvFile.open(input.c_str(),  ifstream::in);
    
    if (csvFile.is_open())
    {
        if (csvFile.good()){
            getline (csvFile,header);
            if (header != Database::CAPTION){
                throw runtime_error("Invalid CSV header!");
            }
        }

        while ( csvFile.good() )
        {
            getline (csvFile,in);    //read one line from file
            
            if (!in.empty()){   //skip empty lines
                Item* item = new Item();
                line << in;
                getline (line,in,';');
                item->group = string(in, 0, in.length());
                
                getline (line,in,';');
                item->name = string (in, 0, in.length());
                
                getline (line,in,';');
                item->login = string(in, 0, in.length());
                
                getline (line,in,';');
                item->password = string(in, 0, in.length());
 
                try {
                    addItem(item);
                    counter++;
                } catch (invalid_argument& ex){
                    cerr << ex.what() << endl;
                    delete item;
                    continue;
                }
            }
        }
        csvFile.close();
        cout << "Successfuly imported entries: " << counter << endl;
        
    } else {
        throw ifstream::failure("Cannot read database file! Check your premissions.");
    }    
}



void DatabaseManager::addItem(Item* item) throw(invalid_argument) {
  
    if (item->group.empty()) item->group = "default"; 
    if (item->login.empty()) item->login = "none";
    
    
    if (item->name.empty() || item->password.empty()){
        throw invalid_argument("Name and password cannot be empty!");
    }
    
    if (!m_db->getItemByName(item->name)){
        item->login = m_db->encrypt(item->login);
        item->password = m_db->encrypt(item->password);
        m_db->insertItem(item);
    } else {
        throw invalid_argument("Such name (" + item->name + ") already in database! Skipping.");
    }
}

void DatabaseManager::removeItem(const string& name) throw(invalid_argument)
{
    Item* item = m_db->getItemByName(name);
    
    if (item){
        m_db->deleteItem(item);
    } else {
        throw invalid_argument("No entry with this name (" + name + ") in database!");
    }
    
    delete item;
    item = NULL;
}

void DatabaseManager::editItem(Item * item) throw(invalid_argument)
{
    if (item->group.empty()) item->group = "default"; 
    if (item->login.empty()) item->login = "none";
    
    if (item->name.empty() || item->password.empty()){
        throw invalid_argument("Name and password cannot be empty!");
    }
    
    Item* dbItem = m_db->getItemByName(item->name);
    if (dbItem){
        dbItem->group = item->group;
        dbItem->login = m_db->encrypt(item->login);
        dbItem->password = m_db->encrypt(item->password);
    } else
        throw invalid_argument("Such name ( " + item->name + " ) not in database!");
}


void DatabaseManager::printItemsByName(const string& name) const
{
    cout << "Database " << m_path << ":" << endl;
    cout.setf(ios::left);
    cout.width(COL_WIDTH);
    cout << "NAME" << "(GROUP)" << endl;
    cout << SEPARATOR << endl;
    
    bool found = false;
    list<Item*> itemsList = m_db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        if ((*iterator)->name.find(name) != string::npos){
            cout.width(COL_WIDTH);
            cout << (**iterator) << endl;
            cout << SEPARATOR << endl;
            found = true;
        }
    }
    
    if (!found){
        cout << "No entry!" << endl;
        cout << SEPARATOR << endl;
    }
}


void DatabaseManager::printItemsByGroup(const string& group) const
{
    cout << "Database " << m_path << ":" << endl;
    cout.setf(ios::left);
    cout.width(COL_WIDTH);
    cout << "NAME" << "(GROUP)" << endl;
    cout << SEPARATOR << endl; 
    
    bool found = false;
    list<Item*> itemsList = m_db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        if ((*iterator)->group.find(group) != string::npos){
            cout.width(COL_WIDTH);
            cout << (**iterator) << endl;
            cout << SEPARATOR << endl;
            
            found = true;
        }
    }
    if (!found){
        cout << "No entry!" << endl;
        cout << SEPARATOR << endl;
    }
}


void DatabaseManager::printItemByName(const string& name) const
{
    cout << "Database " << m_path << ":" << endl;
    cout.setf(ios::left);
    cout.width(COL_WIDTH);
    cout << "NAME" << "(GROUP)" << endl;
    cout.width(COL_WIDTH);
    cout << "LOGIN" << "PASSWORD" << endl; 
    cout << SEPARATOR << endl; 
    
    Item* item = m_db->getItemByName(name);
    if (item){
        cout.width(COL_WIDTH);
        cout << *item << endl;
        cout.width(COL_WIDTH);
        cout << m_db->decrypt(item->login) 
        << m_db->decrypt(item->password) << endl; 
        cout << SEPARATOR << endl;
    } else {
        cout << "Name not found!" << endl;
        cout << SEPARATOR << endl;
    }
}


void DatabaseManager::printAllItems() const
{
    cout << "Database " << m_path << ":" << endl;
    cout.setf(ios::left);
    cout.width(COL_WIDTH);
    cout << "NAME" << "(GROUP)" << endl;
    cout << SEPARATOR << endl; 
    
    bool found = false;
    
    list<Item*> itemsList = m_db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        cout.width(COL_WIDTH);
        cout << (**iterator) << endl;
        cout << SEPARATOR << endl;
        
        found = true;
    }
    if (!found){
        cout << "Database is empty!" << endl;
        cout << SEPARATOR << endl;
    }
}


void DatabaseManager::printAllItemsWithSecrets() const
{
    cout << "Database " << m_path << ":" << endl;
    cout.setf(ios::left);
    cout.width(COL_WIDTH);
    cout << "NAME" << "(GROUP)" << endl;
    cout.width(COL_WIDTH);
    cout << "LOGIN" << "PASSWORD" << endl; 
    cout << SEPARATOR << endl; 
    
    bool found = false;
    list<Item*> itemsList = m_db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        cout.width(COL_WIDTH);
        //cout << (**iterator) << endl;
        cout.width(COL_WIDTH);
        cout << m_db->decrypt((**iterator).login) << m_db->decrypt((**iterator).password) << endl; 
        cout << SEPARATOR << endl;
        
        found = true;
    }
    if (!found){
        cout << "Database is empty!" << endl;
        cout << SEPARATOR << endl;
    }    
}

void DatabaseManager::sortDatabase()
{
   m_db->sortDatabase();
}



DatabaseManager::~DatabaseManager()
{

}

