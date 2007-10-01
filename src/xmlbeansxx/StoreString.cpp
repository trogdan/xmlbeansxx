/*
    Copyright 2004-2005 TouK s.c.
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License. */


#include "StoreString.h"
#include "BeansException.h"
#include "defs.h"
#include "Tracer.h"
#include "TextUtils.h"

#include <cstring>



namespace xmlbeansxx {
//------------------------------------------------

namespace {

char *duplicate(const char *str) {

    char *buf;
    int l=strlen(str)+1;
    buf=new char[l];


    if (!((((unsigned long)buf)&1)==0)) {
        throw IllegalStateException();
    }
    //strcpy(buf,str);
    for(int i=0;i<l;i++) buf[i]=str[i];
    return buf;
}

void freeDuplicate(char *str) {
    delete[] str;
}
}
//------------------------------------------------
SimpleString::SimpleString(const std::string &s) {
    buf=duplicate(s.c_str());
}
SimpleString::SimpleString(const SimpleString &from) {
    copyFrom(from);
}

SimpleString::~SimpleString() {
    freeDuplicate(buf);
}

void SimpleString::copyFrom(const SimpleString &from) {
    buf=duplicate(from.c_str());
}

SimpleString &SimpleString::operator=(const SimpleString &b) {
    copyFrom(b);
    return *this;
}

const char *SimpleString::c_str() const {
    return buf;
}
//------------------------------------------------

/*
CStr::CStr(const char *buf):buf(buf) {}
const char *CStr::get() const {
        return buf;
    }
bool CStr::operator<(const CStr &b) const {
    return strcmp(buf,b.buf)<0;
}
*/

//------------------------------------------------
StringStorage::StringStorage() {
    //add default storage
    add(std::string());
}

void StringStorage::add(const std::string &str) {
    const char *cs=str.c_str();
    return add(cs);
}

void StringStorage::add(const char * cs) {
    if (cs==NULL) cs="";
    if (!isStored(cs)) {
        const char *s(duplicate(cs));
        contents.insert(std::pair<const char *,int>(s,stored.size()));
        SSInfo ssi;
        ssi.str=s;
        ssi.hashCode=CStrHashFn()(s);
        stored.push_back(ssi);
    }
}
bool StringStorage::isStored(const char *str) {
    return contents.find(str)!=contents.end();
}

unsigned long StringStorage::get(const char *str) {
    StoreMap::iterator it=contents.find(str);
    if (it!=contents.end()) {
        return (it->second*2+1);
    } else return 0;
}

StringStorage::~StringStorage() {
    while (!contents.empty()) {
        StoreMap::iterator it=contents.begin();
        contents.erase(it);
        freeDuplicate(const_cast<char *>(it->first));
    }

    stored.clear();
}

//------------------------------------------------

void StoreString::construct(const std::string &str) {
    buf=getStorage()->get(str.c_str());
    if (buf==0) {
        buf=(unsigned long)duplicate(str.c_str());
    }
}

StringStorage *StoreString::getStorage() {
    static StringStorage *storage=new StringStorage();
    return storage;
}

StoreString::StoreString() {
    construct(std::string());
}

StoreString::StoreString(const std::string &str) {
    construct(str);
}
StoreString::StoreString(const char *str) {
    construct(std::string(str));
}

StoreString::StoreString(const StoreString &from) {
    copyFrom(from);
}

StoreString::~StoreString() {
    if (!isStored()) {
        delete[] ((char *)buf);
    }
    buf=0;
}

std::string StoreString::toString() const {
    return std::string(const_cast<char *>(c_str()));
}

StoreString StoreString::store(const std::string &str) {
    getStorage()->add(str);
    return StoreString(str);
}
StoreString StoreString::store(const char *str) {
    getStorage()->add(str);
    return StoreString(str);
}

}


