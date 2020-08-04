#ifndef __OLD_UTILS
#define __OLD_UTILS


#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>

#include <vector>
#include <string>
#include <map>
#include <set>

#include <cstdio>
#include <cctype>


//
// map helper function
//

template<typename K, typename V>
bool mapHas(const std::map<K, V> &map, const K &key) 
{
  return map.count(key) > 0;
}


template<typename K, typename V>
void mapForceInsert(std::map<K, V> &map, const std::pair<K, V> &pair) {
  if (mapHas(map, pair.first)) {
    map.erase(map.find(pair.first));
  }
  map.insert(pair);
}


#endif