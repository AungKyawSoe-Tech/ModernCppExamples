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
    
    ListNode* FindFirstCommonNode(ListNode *pHead1, ListNode *pHead2) {
              
        ListNode* current1 = pHead1; 
        ListNode* current2 = pHead2; 
    
        // traverse list A till the end of list 
        while (current1 != NULL) { 
  
            // traverse list B till the end of list 
            while (current2 != NULL) { 
              
            // if data is match then count increase 
            if (current1->val == current2->val) {
           
                return current1;
            }
  
            // increase current pointer for next node 
            current2 = current2->next; 
        } 
  
        // increase current pointer of first list
        current1 = current1->next; 
  
        // initialize starting point for second list 
        current2 = pHead2;             
        }
    }
};

int main () {
    Solution testSolution;
    std::array<int, 12> intArray1 {{1, 3, 4, 6, 7, 10, 12, 13, 14, 15, 16, 17}};
    std::array<int, 10>	intArray2 = {2, 4, 5, 6, 7, 8, 11, 13 , 14, 16};
    
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
	
	for (i = 0; i < intArray2.size(); i ++) {
        ListNode *pTemp = new ListNode(intArray2[i]);
        
        if (i == 0) {
            pNHead = pCurrent = pTemp;
		} else {
            pCurrent->next = pTemp;
            pCurrent = pCurrent->next; // pCurrent->next == pTemp
        }
    }
    
    // Print Linkedlist
    cout << "PrintList(pMHead)" << endl;
    testSolution.PrintList(pMHead);        
	
	cout << "PrintList(pNHead)" << endl;
    testSolution.PrintList(pNHead);
    
    ListNode *pTarget = testSolution.FindFirstCommonNode(pMHead, pNHead);
    
    // Print Returned Node       
    if (pTarget != NULL)
       cout << "pTarget->val = " << pTarget->val << endl;
    cout << endl;

    return 0;
}
