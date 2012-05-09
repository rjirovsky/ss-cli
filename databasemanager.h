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
#include <string>
#include <list>
#include <stdexcept>

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

using namespace std;

class DatabaseManager
{

public:
    DatabaseManager();
    
    /**
     * @brief   Open database file and verify key. 
     * 
     * @param   dbFile      path to db file
     * @param   key         master key to cypher
     * 
     * @throw   logic_error error with message
     */
    void loadDatabase(const string& dbFile, const string& key) throw(logic_error);
    void closeDatabase();
    
    /**
     * @brief   Save changes in active database to file.
     * 
     * @throw   exception   if no database opened or IO error
     */
    void saveDatabase();
    
    /**
     * @brief   Create new database file on path with given key.
     * After creation, database file is NOT opened!
     * 
     * @param   path    db file location
     * @param   key     secret key to encrypt and decrypt secrets
     * 
     * @throw   exception   on IO error
     */
    void createDatabase(const string& dbFile, const string& key) throw(exception);
    
    /**
     * @brief   Add new entry to database.
     * 
     * @param   group       optional param., if empty, uses "default" group namespace
     * @param   name        required param., unique name for entry
     * @param   login       optional param., if empty, uses "none" string, it's stored encrypted
     * @param   password    required param., password is encrypted 
     * 
     * @throw   invalid_argument    if name or password is empty or name is already in db
     */
    void addItem(string group, const string& name, string login, const string& password) throw(invalid_argument);
    
    /**
     * @brief   Remove entry with given name from database.
     * 
     * @param   name    name of entry
     * 
     * @throw   invalid_argument    if entry is not in database
     */
    void removeItem(const string& name) throw(invalid_argument);
    
    /**
     * @brief   Edit existing entry by name
     * 
     * @param   group       optional param., if empty, uses "default" group namespace
     * @param   name        required param., name of existing entry, must be the same
     * @param   login       optional param., if empty, uses "none" string, it's stored encrypted
     * @param   password    required param., password is encrypted 
     * 
     * @throw   invalid_argument    if name or password is empty or name is NOT in database
     */
    void editItem(string group, const string& name, string login, const string& password) throw(invalid_argument);
    
    /**
     * @brief  Search for substring in group attribute of each entry. Can print more than one entries. 
     */
    void printItemsByGroup(const string& group) const;
    
    /**
     * @brief   Search for substring in name attribute of each entry. Can print more than one entries.
     * 
     * @param name  string to search in name 
     */
    void printItemsByName(const string& name) const;
    
    /**
     * @brief   Print entry with given name. It should be only one.
     * 
     * @param name  name of entry 
     */
    void printItemByName(const string& name) const;    
    
    /**
     * Prints name and group for each Item in db.
     */
    void printAllItems() const;
    
    /**
     * Carefull! Same as printAllItems(), but prints readable login and password.
     */
    void printAllItemsWithSecrets() const;

    virtual ~DatabaseManager();

private:
    Database* m_db;   ///database with entries
    string m_key;     ///key to cypher
    string m_hash;
    string m_path;
};

#endif // DATABASEMANAGER_H
