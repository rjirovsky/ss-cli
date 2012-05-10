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

#include "list"
#include <stdexcept>
#include <sys/mman.h>
#include <cstdio>
#include <iostream>

#ifndef DATABASE_H
#define DATABASE_H

using namespace std;

/**
 * @brief Stricture for saving user input.
 */
struct Item {
    
    /**
     * @brief Constructor tries to lock memory.
     * 
     * It is locked whole page.
     */
    Item(){
        this->password.reserve(128);
        this->login.reserve(128);
        if(mlock(this, sizeof(*this))){  //min. 2*32B characters + 2*128B cypher of pass and login
            perror("Unable to lock Item in memory!");
        }
    }
    
    ~Item(){
         munlock(this, sizeof(*this));
    }
    
    string group;
    string name;
    string login;
    string password;
    
    friend ostream& operator <<(ostream& out, const Item& item);
};

/**
 * @brief   Represents database of entries.
 * Manipulates with items. Stores it in std::list.
 * It does encryption and decryption operations.
 */
class Database
{

public:

    Database();
    
    /**
     * @brief   Insert Item to database.
     * It is stored only in memory in container std::list.
     * To save to disk use saveDatabase().
     * It set Database::modified flag to true. 
     * 
     * @param   item    item to insert
     */
    void insertItem(Item* item);
    
    /**
     * @brief   Delete Item from database.
     * It is deleted only from container std::list.
     * To save changes to disk use saveDatabase().
     * It set Database::modified flag to true. 
     * 
     * @param   item    item to insert
     */
    void deleteItem(Item* item);
    
    /**
     * @brief   Return active database file path.
     * 
     * @return  path to database file
     */
    string getPath() const {return m_path;}
    
    


    
    /**
     * @brief   Find Item with given name.
     * 
     * @param   name    exact name to find
     * 
     * @return  one Item with given name
     */
    Item* getItemByName(const string& name);
    
    /**
     * @brief   Get list of all Items
     * 
     * @return  std::list of Item pointers 
     */
    list<Item*> getAllItems(){return items;}
    
    /**
     * @brief   Encrypt given string using symetric cypher. Uses m_key.
     * 
     * @param   str string to encrypt
     */
    string encrypt(string str);
    
    /**
     * @brief   Decrypt given string using symetric cypher. Uses m_key.
     * 
     * @param   str string to decrypt
     */
    string decrypt(string str);
    
    
    /**
     * @brief   Free all alocated memory.
     */
    virtual ~Database();
    
    void sortDatabase();
    
    
    const static string HEADER; ///control header of file
    const static string CAPTION;
private:
    list<Item*> items;  ///list of Items from database file
    string m_key;       ///key for symetric cypher
    string m_hash;      ///control hash from file
    string m_path;      ///path to database file
};



#endif // DATABASE_H
