#include <iostream>
using namespace std;

struct ListNode {
       int val;
       ListNode *next;
       ListNode(int x) : val(x), next(NULL) {}
};
 
class Solution 
{
public:
    void PrintListNode(ListNode* inListNode)  {
         if (inListNode != NULL) {
            cout << inListNode->val << " -> ";
            PrintListNode(inListNode->next);
         } else {
            cout << "NULL" << endl;
		 }
    }

    void Test(ListNode* inListNode) {
         delete inListNode;
         inListNode = NULL;
         cout << inListNode << endl;

         PrintListNode(inListNode);
    }
    
    // Insert node at the beginning of the list
    ListNode* InsertNodeAtHead(ListNode* pHead, int value) {
         cout << "Entry: InsertNodeAtHead(value=" << value << ")" << endl;
         
         ListNode* pNewNode = new ListNode(value);
         pNewNode->next = pHead;
         
         cout << "Exit: InsertNodeAtHead" << endl;
         return pNewNode;  // New head
    }
    
    // Insert node at the end of the list
    ListNode* InsertNodeAtTail(ListNode* pHead, int value) {
         cout << "Entry: InsertNodeAtTail(value=" << value << ")" << endl;
         
         ListNode* pNewNode = new ListNode(value);
         
         // Empty list case
         if (pHead == NULL) {
            cout << "Exit: InsertNodeAtTail (empty list)" << endl;
            return pNewNode;
         }
         
         // Traverse to the end
         ListNode* pCur = pHead;
         while (pCur->next != NULL) {
            pCur = pCur->next;
         }
         
         pCur->next = pNewNode;
         
         cout << "Exit: InsertNodeAtTail" << endl;
         return pHead;
    }
    
    // Insert node after a specific node
    void InsertNodeAfter(ListNode* pPrevNode, int value) {
         cout << "Entry: InsertNodeAfter(value=" << value << ")" << endl;
         
         if (pPrevNode == NULL) {
            cout << "Error: Previous node cannot be NULL" << endl;
            return;
         }
         
         ListNode* pNewNode = new ListNode(value);
         pNewNode->next = pPrevNode->next;
         pPrevNode->next = pNewNode;
         
         cout << "Exit: InsertNodeAfter" << endl;
    }
    
    // Insert node at a specific position (0-indexed)
    ListNode* InsertNodeAtPosition(ListNode* pHead, int position, int value) {
         cout << "Entry: InsertNodeAtPosition(pos=" << position << ", value=" << value << ")" << endl;
         
         // Insert at head
         if (position == 0) {
            return InsertNodeAtHead(pHead, value);
         }
         
         // Traverse to position-1
         ListNode* pCur = pHead;
         for (int i = 0; i < position - 1 && pCur != NULL; i++) {
            pCur = pCur->next;
         }
         
         if (pCur == NULL) {
            cout << "Error: Position out of bounds" << endl;
            return pHead;
         }
         
         InsertNodeAfter(pCur, value);
         
         cout << "Exit: InsertNodeAtPosition" << endl;
         return pHead;
    }
    
    // Insert node in sorted list (ascending order)
    ListNode* InsertNodeSorted(ListNode* pHead, int value) {
         cout << "Entry: InsertNodeSorted(value=" << value << ")" << endl;
         
         ListNode* pNewNode = new ListNode(value);
         
         // Empty list or insert at head
         if (pHead == NULL || pHead->val >= value) {
            pNewNode->next = pHead;
            cout << "Exit: InsertNodeSorted (at head)" << endl;
            return pNewNode;
         }
         
         // Find insertion point
         ListNode* pCur = pHead;
         while (pCur->next != NULL && pCur->next->val < value) {
            pCur = pCur->next;
         }
         
         pNewNode->next = pCur->next;
         pCur->next = pNewNode;
         
         cout << "Exit: InsertNodeSorted" << endl;
         return pHead;
    }
    
    void DeleteNode(ListNode* pInHead, ListNode* pToBeDeleted)    {
         cout << "Entry void DeleteNode(ListNode* pInHead, ListNode* pToBeDeleted)\n" << endl;

         // Check NULL always
         if (pInHead == NULL || pToBeDeleted == NULL) {
            cout << "Abort void DeleteNode(ListNode* pInHead, ListNode* pToBeDeleted)\n" << endl;
            return;
         }
         
//         PrintListNode(pInHead);
         
         // Delete non-tail node including head node
         if (pToBeDeleted->next != NULL) {
            ListNode* pNext = pToBeDeleted->next;
            pToBeDeleted->val = pNext->val;
            pToBeDeleted->next = pNext->next;
            
            delete pNext;
            pNext = NULL;
         } else { // Delete tail
            ListNode* pPre = pInHead;
         
            while (pPre->next != pToBeDeleted && pPre != NULL) {
                  pPre = pPre->next;
            }
            if (pPre == NULL)
               return;
         
            pPre->next = NULL;
            delete pToBeDeleted;
            pToBeDeleted = NULL;
         }                 
         
          cout << "Exit void DeleteNode(ListNode* pInHead, ListNode* pToBeDeleted)\n" << endl;
    }
    
    // Delete node by value
    ListNode* DeleteNodeByValue(ListNode* pHead, int value) {
         cout << "Entry: DeleteNodeByValue(value=" << value << ")" << endl;
         
         if (pHead == NULL) {
            cout << "Error: Empty list" << endl;
            return NULL;
         }
         
         // Delete head node
         if (pHead->val == value) {
            ListNode* pTemp = pHead;
            pHead = pHead->next;
            delete pTemp;
            cout << "Exit: DeleteNodeByValue (deleted head)" << endl;
            return pHead;
         }
         
         // Find and delete node
         ListNode* pCur = pHead;
         while (pCur->next != NULL && pCur->next->val != value) {
            pCur = pCur->next;
         }
         
         if (pCur->next == NULL) {
            cout << "Error: Value not found" << endl;
            return pHead;
         }
         
         ListNode* pToDelete = pCur->next;
         pCur->next = pCur->next->next;
         delete pToDelete;
         
         cout << "Exit: DeleteNodeByValue" << endl;
         return pHead;
    }
   
};

void TestInsertOperations() {
    cout << "\n" << string(70, '=') << endl;
    cout << "TESTING INSERT OPERATIONS\n";
    cout << string(70, '=') << endl;
    
    Solution solution;
    ListNode* pHead = NULL;
    
    // Test 1: Insert at head (empty list)
    cout << "\n--- Test 1: Insert at head (empty list) ---" << endl;
    pHead = solution.InsertNodeAtHead(pHead, 10);
    solution.PrintListNode(pHead);
    
    // Test 2: Insert at head (non-empty list)
    cout << "\n--- Test 2: Insert at head (non-empty list) ---" << endl;
    pHead = solution.InsertNodeAtHead(pHead, 5);
    solution.PrintListNode(pHead);
    
    // Test 3: Insert at tail
    cout << "\n--- Test 3: Insert at tail ---" << endl;
    pHead = solution.InsertNodeAtTail(pHead, 20);
    pHead = solution.InsertNodeAtTail(pHead, 25);
    solution.PrintListNode(pHead);
    
    // Test 4: Insert after specific node
    cout << "\n--- Test 4: Insert after second node ---" << endl;
    solution.InsertNodeAfter(pHead->next, 15);
    solution.PrintListNode(pHead);
    
    // Test 5: Insert at position
    cout << "\n--- Test 5: Insert at position 2 ---" << endl;
    pHead = solution.InsertNodeAtPosition(pHead, 2, 12);
    solution.PrintListNode(pHead);
    
    // Clean up
    while (pHead != NULL) {
        ListNode* temp = pHead;
        pHead = pHead->next;
        delete temp;
    }
}

void TestInsertSorted() {
    cout << "\n" << string(70, '=') << endl;
    cout << "TESTING INSERT IN SORTED LIST\n";
    cout << string(70, '=') << endl;
    
    Solution solution;
    ListNode* pHead = NULL;
    
    // Insert values in random order
    int values[] = {30, 10, 50, 20, 40};
    
    for (int val : values) {
        cout << "\nInserting " << val << " into sorted list:" << endl;
        pHead = solution.InsertNodeSorted(pHead, val);
        solution.PrintListNode(pHead);
    }
    
    // Clean up
    while (pHead != NULL) {
        ListNode* temp = pHead;
        pHead = pHead->next;
        delete temp;
    }
}

void TestDeleteOperations() {
    cout << "\n" << string(70, '=') << endl;
    cout << "TESTING DELETE OPERATIONS\n";
    cout << string(70, '=') << endl;
    
    Solution testSolution;
    int count = 5;
 
    for (int k = 0; k <= count; k ++) {
        ListNode* pHead = NULL;
        ListNode* pCur = NULL;
        ListNode* pDel = NULL;
        
        cout << "\n--- Creating list and deleting node at position " << k << " ---" << endl;
        
        for (int i = 0; i < count; i ++) {
            ListNode* pTemp = new ListNode(i);
            
            if (i == 0) {
                pHead = pCur = pTemp;
			}
            else {
                pCur->next = pTemp;
                pCur = pCur->next; // pCur->next == pTemp
            }
            
            if (i == k) 
               pDel = pTemp;
        }
           
        cout << "Original list: ";
        testSolution.PrintListNode(pHead);        
         
        testSolution.DeleteNode(pHead, pDel);

        cout << "After deletion: ";
        testSolution.PrintListNode(pHead);
        
        // Clean up remaining nodes
        while (pHead != NULL) {
            ListNode* temp = pHead;
            pHead = pHead->next;
            delete temp;
        }
    }
}

void TestDeleteByValue() {
    cout << "\n" << string(70, '=') << endl;
    cout << "TESTING DELETE BY VALUE\n";
    cout << string(70, '=') << endl;
    
    Solution solution;
    ListNode* pHead = NULL;
    
    // Create list: 10 -> 20 -> 30 -> 40 -> 50
    pHead = solution.InsertNodeAtTail(pHead, 10);
    pHead = solution.InsertNodeAtTail(pHead, 20);
    pHead = solution.InsertNodeAtTail(pHead, 30);
    pHead = solution.InsertNodeAtTail(pHead, 40);
    pHead = solution.InsertNodeAtTail(pHead, 50);
    
    cout << "\nOriginal list: ";
    solution.PrintListNode(pHead);
    
    // Delete middle node
    cout << "\nDeleting 30:" << endl;
    pHead = solution.DeleteNodeByValue(pHead, 30);
    solution.PrintListNode(pHead);
    
    // Delete head node
    cout << "\nDeleting 10 (head):" << endl;
    pHead = solution.DeleteNodeByValue(pHead, 10);
    solution.PrintListNode(pHead);
    
    // Delete tail node
    cout << "\nDeleting 50 (tail):" << endl;
    pHead = solution.DeleteNodeByValue(pHead, 50);
    solution.PrintListNode(pHead);
    
    // Clean up
    while (pHead != NULL) {
        ListNode* temp = pHead;
        pHead = pHead->next;
        delete temp;
    }
}

int main () {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║           LINKED LIST INSERT AND DELETE OPERATIONS              ║\n";
    cout << "║                                                                  ║\n";
    cout << "║  Demonstrates various insertion and deletion techniques          ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    TestInsertOperations();
    TestInsertSorted();
    TestDeleteOperations();
    TestDeleteByValue();
    
    cout << "\n" << string(70, '=') << endl;
    cout << "ALL TESTS COMPLETED\n";
    cout << string(70, '=') << endl;
    cout << endl;
    
    return 0;
}
