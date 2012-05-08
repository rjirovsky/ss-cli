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


using namespace std;

DatabaseManager::DatabaseManager()
{
    db = new Database();
}

void DatabaseManager::loadDatabase(const string& dbFile, const string& key) throw(logic_error){
    
    try{
    
    db->openDatabase(dbFile, key);
    
    } catch (logic_error& ex){
        throw;
    }
    this->key = key; 
}

void DatabaseManager::createDatabase(const string& dbFile, const string& key) throw(exception)
{
    try{
        
        db->createDatabase(dbFile, key);
        
    } catch (exception& ex){
        throw;
    }
}

void DatabaseManager::saveDatabase()
{
    //save db to file
}


void DatabaseManager::addItem(string group, const string& name, string login, const string& password) throw(invalid_argument) {
  
    if (group.empty()) group = "default"; 
    if (login.empty()) login = "none";
    
    
    if (name.empty() || password.empty()){
        throw invalid_argument("Name and password cannot be empty!");
    }
    
    if (!db->getItemByName(name)){
        Item* workingItem = new Item(group, name, db->encrypt(login), db->encrypt(password));
        db->insertItem(workingItem);
    } else
        throw invalid_argument("Such name ( " + name + " ) already in database!");
    
}

void DatabaseManager::removeItem(const string& name) throw(invalid_argument)
{
    Item* item = db->getItemByName(name);
    
    if (item){
        db->deleteItem(item);
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
    
    Item* item = db->getItemByName(name);
    if (item){
        item->setGroup(group);
        item->setLogin(db->encrypt(login));
        item->setPassword(db->encrypt(password));
    } else
        throw invalid_argument("Such name ( " + name + " ) not in database!");
}



void DatabaseManager::printItemsByName(const string& name) const
{
    list<Item*> itemsList = db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        if ((*iterator)->getName().find(name) != string::npos){
            cout << (**iterator) << endl;      
        }
    }
}
void DatabaseManager::printItemsByGroup(const string& group) const
{
    list<Item*> itemsList = db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        if ((*iterator)->getGroup().find(group) != string::npos){
            cout << (**iterator) << endl;      
        }
    }
}

void DatabaseManager::printItemByName(const string& name) const
{
    cout << db->getItemByName(name) << endl;
}


void DatabaseManager::printAllItems() const
{
    list<Item*> itemsList = db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        cout << (**iterator) << endl;        
    }

}


void DatabaseManager::printAllItemsWithSecrets() const
{
    list<Item*> itemsList = db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        cout << (**iterator) << endl << db->decrypt((**iterator).getLogin()) 
             << " " << db->decrypt((**iterator).getPassword()) << endl;        
    }
    
}

DatabaseManager::~DatabaseManager()
{
    
    list<Item*> itemsList = db->getAllItems();
    for (list<Item*>::iterator iterator = itemsList.begin(), end = itemsList.end(); iterator != end; ++iterator) {
        
        delete(*iterator);
    }
    itemsList.clear();
    
    delete(db);
}

