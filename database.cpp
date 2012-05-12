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

#include <fstream>
#include <sstream>
#include <fcntl.h>

const string Database::HEADER = "SAFE_STORAGE";
const string Database::CAPTION = "group;name;login;password;";

bool compare(Item* first, Item* second);

Database::Database(string path):m_path(path)
{
    mlock(&m_key, CryptoPP::CIPHER::MAX_KEYLENGTH);
    mlock(&iv, CryptoPP::CIPHER::BLOCKSIZE);
    
    memset( m_key, '\0', CryptoPP::CIPHER::MAX_KEYLENGTH ); ///AES-256
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


string Database::encrypt(string plainText)
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
        cerr << "Encryption error: " << e.what() << endl;
    }   
    
    return cypherText;
}

string Database::decrypt(string cypherText)
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
        std::cerr << "Decryption error: " << e.what() << std::endl;
    }
    
//     cout << endl << "Decrypted key: " <<endl;
//     cout << recoveredText << endl;
    return recoveredText;
}

void Database::deriveKey(const string& password)
{
    cout << "Database::deriveKey()" << endl;

//     cout << SHA256("ahoj") << endl;
//     cout << SHA256("ahoj") << endl;
//     cout << SHA256("ahoj") << endl;
//     

//     CryptoPP::SHA256 hash;
//     hash.CalculateDigest(m_key, (unsigned char*) password.c_str(), password.length());
//     cout << m_key << endl;
//     cout << m_key << endl;
//     cout << m_key << endl;
    
/*  
    cout << SHA256(password) << endl;
    cout << SHA256(password) << endl;
    cout << SHA256(password) << endl;*/
    
    string passHash = SHA256(password);
    copy(passHash.begin(),passHash.end(),m_key);    // string to byte *
//     cout << m_key << endl;
//      cout << "Derived key: >" << hex << (int*) m_key << "<" << endl;
}

string Database::SHA256(string in)
{
    in = "heslo";
    byte buff[SHA1::DIGESTSIZE];
    memset(&buff,'\0',SHA1::DIGESTSIZE);
    CryptoPP::SHA1 hash;
    hash.CalculateDigest(buff, (unsigned char*) in.c_str(), in.length()-1);
    
    cout << "SHA1 result native: >" << buff << "<" << endl;
    cout << "SHA1 result string: >" << string((char*) buff) << "<" << endl;
    
    return string((char*) buff);
}


bool Database::checkPassword()
{

    cout << "Decrypted key >" << decrypt(m_checksum) << "<" << endl;

    cout << "Checked   key >" << string((char*) m_key) << "<" << endl;
    return decrypt(m_checksum) == string((char*) m_key);
}

void Database::deriveChecksum()
{
    m_checksum = encrypt( string((char*) m_key) );
//     cout << "Derived checksum >" << hex << (int*) m_checksum.c_str() << "<" << endl;
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