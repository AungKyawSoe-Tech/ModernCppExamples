// Example program
#include <iostream>
#include <string>

#include <algorithm>
#include <functional>
#include <iterator>

#include <vector>
#include <set>
#include <map>
  
using Pair_t = std::multimap< std::string, std::string> ;
using MMAPIterator = std::multimap<std::string, std::string>::iterator ;

std::set<std::string> mDictionary = { "act",  "ant",  "art",  "bat",  "bet",  "boss",  "cat",  "cap",  "cop",  "dear", "dog",
    "dip", "ear", "end", "eel", "fad", "fat", "fog", "gap", "god", "hat", "hit", "hot", "ink", "irk", "jot", "jab", 
    "lap", "lip", "lot", "man", "nan", "nat", "net", "pat", "pet", "tap", "tar", "ten", "rat", "woo", "yoo", "zoo"};

int main()
{
    Pair_t Pairs = {};
    std::vector<std::string> emptyVectors = {};
    
    //std::cout << "All words in the mDictionary are :" << std::endl;
    for(const auto& it : mDictionary) {
       
       std::string sortedWord = it;
       std::sort(sortedWord.begin(), sortedWord.end());

       Pairs.emplace(std::make_pair(sortedWord, it));
      
       //std::cout << "for word " << it << ", sorted word = " << sortedWord << std::endl;
    }
    
    for(auto& obj : Pairs) {
        
        std::cout << "unsorted word = " << obj.second << ", sorted word : " << obj.first << std::endl;
    }
    
   
    for(auto& obj : Pairs) {
        
        std::pair <MMAPIterator, MMAPIterator> equalPairs = Pairs.equal_range(obj.first);
        
        if(equalPairs.second != equalPairs.first)
        {
            std::cout << "for the word = " << obj.second << ", its anagrams are : ";
        
            for (MMAPIterator it=equalPairs.first; it!=equalPairs.second; ++it) {
                
                if(obj.second != it->second) {
                    std::cout << it->second << ", ";
                }
                
                //std::cout << it->first << " : " << it->second << ", ";
            }
        
            std::cout << std::endl;
        } else {
            std::cout << "for the word = " << obj.second << " there are no anagrams!";
        }
    }
}
