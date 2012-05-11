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




// #include <openssl/sha.h>
// #include <string.h>

#include <fstream>
#include <sstream>
#include <fcntl.h>





const string Database::HEADER = "SAFE_STORAGE";
const string Database::CAPTION = "group;name;login;password;";

bool compare(Item* first, Item* second);

Database::Database()
{

}

Item* Database::getItemByName(const string& name)
{
    Item* item = NULL;
    for (list<Item*>::iterator iterator = items.begin(), end = items.end(); iterator != end; ++iterator) {
        if ((*iterator)->name == name){
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



void Database::sortDatabase()
{
    items.sort(compare);
}


string Database::encrypt(string& str)
{
    string plainText = str;
    string cypherText;
    
    try
    {       
        // Encryptor
        CryptoPP::CIPHER_MODE<CryptoPP::CIPHER>::Encryption
        Encryptor( m_key, sizeof(m_key), iv );
        
        // Encryption
        CryptoPP::StringSource( plainText, true,
                    new CryptoPP::StreamTransformationFilter( Encryptor,
                            new CryptoPP::StringSink( cypherText )
                                                            ) // StreamTransformationFilter
                              ); // StringSource
    }
    catch( CryptoPP::Exception& e)
    {
        cerr << e.what() << endl;
    }   
    
    return cypherText;
}

string Database::decrypt(string& str)
{
    string cypherText = str;
    string recoveredText;
    
    try{
       
        
        // Decryptor
        CryptoPP::CIPHER_MODE<CryptoPP::CIPHER>::Decryption
        Decryptor( m_key, sizeof(m_key), iv );
        
        // Decryption
        CryptoPP::StringSource( cypherText, true,
                                new CryptoPP::StreamTransformationFilter( Decryptor,
                                                                          new CryptoPP::StringSink( recoveredText )
                                ) // StreamTransformationFilter
        ); // StringSource
    }
    catch( CryptoPP::Exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    return recoveredText;
}

void Database::deriveKey(string password)
{

    CryptoPP::SHA256 hash;
    hash.Update((unsigned char *)password.c_str(), password.length());
//     hash.Update(pbData2, nData2Len);
//     hash.Update(pbData3, nData3Len);
    hash.Final(m_key);
    
    
    ///temp key
    ///::memset( m_key, 0x01, CryptoPP::CIPHER::MAX_KEYLENGTH ); ///AES-256
    ::memset( iv, 0x01, CryptoPP::CIPHER::BLOCKSIZE );        ///blocksize 128b
    
}



Database::~Database()
{
    for (list<Item*>::iterator it = items.begin(); it != items.end(); ++it) {        
        delete *it;
    }
    items.clear();
}

ostream& operator <<(ostream& out, const Item& item) {
    
    //out.setf(ios::left);
    out << item.name << "(" << item.group << ")";
    
    return out;
}

/**
 * @brief Compare two Item objects lexicographically.
 * 
 * First compares groups, if equal compares names.
 * 
 * @param   first
 * @param   second
 * @return  true - first is lower (comes first), false - equal or higher
 */
bool compare(Item* first, Item* second)
{
    if (first->group < second->group){
        return true;
    }
    else if (first->group == second->group){
        if (first->name < second->name)
            return true;
        else 
            return false;
    } else
        return false;      
}