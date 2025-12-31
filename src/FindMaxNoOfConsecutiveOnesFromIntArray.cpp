Instructions:

Write a function that returns the maximum number of consecutive 'ones' in an integer array of length N.

You can assume the input is an array containing integers 0 and 1 only.

For example, for the following input the function should return 7.

	010101111011011101111111000000010

This can be written in pseudo-code, Java, or C/C++. Syntax mistakes are not important. 

// Example program
#include <iostream>
#include <string>
#include <array>
#include <algorithm>

int main()
{
    int cArray[] = {0,1,0,0,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0};
  
    const int sizeArray = sizeof(cArray)/sizeof(cArray[0]);
  
    std::array<int, sizeArray> cppArray;
  
    std::move(std::begin(cArray), std::end(cArray), cppArray.begin());
  
    auto maxiConsecutiveOnes = 0;
    auto runningMaximumOnes= 0;
  
    for(const auto& i : cppArray) {
      
      if(cppArray[i] == 1) {
          runningMaximumOnes++;
      } else {
         
          if(maxiConsecutiveOnes < runningMaximumOnes) {
              maxiConsecutiveOnes = runningMaximumOnes;
          }
          runningMaximumOnes = 0;
      }
   }
      
   std::cout << " maximum consecutive ones = " << maxiConsecutiveOnes << std::endl;
}
























