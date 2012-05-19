/*
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

#include <list>
#include <stdexcept>
#include <sys/mman.h>
#include <cstdio>
#include <iostream>

#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>
#include <cryptopp/aes.h>

#define CIPHER AES
#define CIPHER_MODE CBC_Mode
#define HASH SHA1


#ifndef DATABASE_H
#define DATABASE_H

using namespace std;
// using namespace CryptoPP;

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

    Database(string path);
    
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
    string encrypt(string& str);
    
    /**
     * @brief   Decrypt given string using symetric cypher. Uses m_key.
     * 
     * @param   str string to decrypt
     */
    string decrypt(string& str);
    
    /**
     * @brief   Derive key from user password.
     * It uses hash function in HASH constant.
     * 
     * @param   password    user defined password
     */
    void deriveKey(const string& password);
    
    /**
     * @brief   Derive control hash for password check.
     * It uses hash function in HASH constant.
     */
    void deriveChecksum();
    
    /**
     * @brief   Check if password is correct.
     */
    bool checkPassword();
    
    string getChecksum(){return m_checksum;}
    void setChecksum(string checksum){m_checksum = checksum;}
    
//     string hash(string in);
    
    /**
     * @brief   Free all alocated memory.
     */
    virtual ~Database();
    
    /**
     * @brief Sort database content alphabeticaly.  
     */
    void sortDatabase();
    
    
    const static string HEADER;                         ///control header of file
    const static string CAPTION;                        ///atributes caption
private:
    list<Item*> items;                                  ///list of Items from database file
    byte m_key[CryptoPP::CIPHER::MAX_KEYLENGTH];        ///key for symetric cypher 256b
    byte iv[ CryptoPP::CIPHER::BLOCKSIZE ];             ///inicialization vector
    string m_checksum;                                  ///control hash from file
    string m_path;                                      ///path to database file
};

#endif // DATABASE_H
