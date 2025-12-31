#include <iostream>
#include <array>
#include <string>

using namespace std;

struct ListNode {
       int val;
       ListNode *next;
       ListNode(int x = 0) : val(x), next(NULL) {}
};
 
class Solution {
public:
    void PrintList(ListNode *pListNode) {
         if (pListNode != NULL) {
            cout << pListNode->val << endl;
            PrintList(pListNode->next);
         } else {
            cout << "NULL\n" << endl;
		 }
    }
    
    int getLengthOfList(const ListNode *pListNode) {
        int length = 0;
        
        while (pListNode != NULL) {
              pListNode = pListNode->next;
              length ++;
        }

        return length;              
    }
    
    ListNode* FindMthToLastElement(ListNode *pHead1, int valueM) {
              
        ListNode* current = pHead1; 
       
        // traverse list A till the end of list 
        while (current != NULL) { 
              
            // if data is match then count increase 
            if (current->val == valueM) {
                return current;
            }
        
  
            // increase current pointer of first list
            current = current->next;    
        }
        
         return NULL;
    }
};

int main () {
    Solution testSolution;
    std::array<int, 12> intArray1 {{1, 3, 4, 6, 7, 10, 12, 13, 14, 15, 16, 17}};
     
    ListNode *pMHead = NULL;
    ListNode *pCurrent   = NULL;
    ListNode *pNHead = NULL;
    auto i = 0;
	
    for (i = 0; i < intArray1.size(); i ++) {
        ListNode *pTemp = new ListNode(intArray1[i]);
        
        if (i == 0) {
            pMHead = pCurrent = pTemp;
		} else {
            pCurrent->next = pTemp;
            pCurrent = pCurrent->next; // pCurrent->next == pTemp
        }
    }
	    
    // Print Linkedlist
    cout << "PrintList(pMHead)" << endl;
    testSolution.PrintList(pMHead);        
    
    int valueOfM = 4;
    
    ListNode *pTarget = testSolution.FindMthToLastElement(pMHead, valueOfM);
    
    // Print Returned Node       
    if (pTarget != NULL) {
       testSolution.PrintList(pMHead);     
    } else {
        cout << "There is no element that has value equal to " << valueOfM << endl;
    }
    
    valueOfM = 5;
    
    pTarget = testSolution.FindMthToLastElement(pMHead, valueOfM);
    
    // Print Returned Node       
    if (pTarget != NULL) {
       testSolution.PrintList(pMHead);     
    } else {
        cout << "There is no element that has value equal to " << valueOfM << endl;
    }


    return 0;
}
