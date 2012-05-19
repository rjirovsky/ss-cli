/*
    Database
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

#include <fstream>
#include <sstream>
#include <fcntl.h>
//#include <openssl/rand.h>

const string Database::HEADER = "SAFE_STORAGE";
const string Database::CAPTION = "group;name;login;password;";

bool compare(Item* first, Item* second);

Database::Database(string path):m_path(path)
{
    mlock(&m_key, CryptoPP::CIPHER::MAX_KEYLENGTH);
    mlock(&iv, CryptoPP::CIPHER::BLOCKSIZE);
    
    memset( m_key, '\0', CryptoPP::CIPHER::MAX_KEYLENGTH );
    
    //RAND_bytes(iv,CryptoPP::CIPHER::BLOCKSIZE );  //TO-DO:generate iv and store to file
    memset( iv, 0x01, CryptoPP::CIPHER::BLOCKSIZE ); 
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

string Database::encrypt(string& plainText)
{
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
        //cerr << "Encryption error: " << e.what() << endl;
    }   
    
    return cypherText;
}

string Database::decrypt(string& cypherText)
{
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
        //std::cerr << "Decryption error: " << e.what() << std::endl;
    }
    
    return recoveredText;
}

void Database::deriveKey(const string& password)
{

    CryptoPP::HASH hash;
    for (int i=1;i <=1000000;i++){      ///increase derivation time
        hash.Update((unsigned char*) password.c_str(), password.length());
    }
    hash.Final(m_key);
//     cout << "key:" << m_key << endl;
}

byte* Database::computeHash(string in)
{
    byte * buff = new byte[CryptoPP::HASH::DIGESTSIZE];
    CryptoPP::HASH hash;
    hash.CalculateDigest(buff, (unsigned char*) in.c_str(), in.length());
//     cout << buff << endl;
    return buff;
}

bool Database::checkPassword()
{

    string keyStr = string((char*) m_key);
    byte * sum = computeHash(encrypt(keyStr));

    bool result = !memcmp(m_checksum, sum, CryptoPP::HASH::DIGESTSIZE);
//     cout << "user:" << sum << endl;
//     cout << "file:" <<  m_checksum << endl;

    delete [] sum;
    return result;
}

void Database::deriveChecksum()
{
    string keyStr = string((char*) m_key);
    byte * sum = computeHash(encrypt(keyStr));
    memcpy(m_checksum, sum, CryptoPP::HASH::DIGESTSIZE);
    //cout << m_checksum << endl;
    delete [] sum;
}


Database::~Database()
{
    for (list<Item*>::iterator it = items.begin(); it != items.end(); ++it) {        
        delete *it;
    }
    items.clear();
    
    munlock(&m_key, CryptoPP::CIPHER::MAX_KEYLENGTH);
    munlock(&iv, CryptoPP::CIPHER::BLOCKSIZE);    
}


/**
 * @brief Overloaded operator << for printing Item without secrets.
 */
ostream& operator <<(ostream& out, const Item& item) 
{    
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