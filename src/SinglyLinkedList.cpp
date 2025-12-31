#include <iostream>
#include <string>
#include <string.h>
 /*
      pHead
       |
       |
    +---+------+     +---+-------+       +----+------+
    | 1  | next ----->|  2  | next-----> |  3 | NULL |
    +---+------+     +---+-------+       +----+------+
*/
extern "C" {
struct info;
typedef struct info{
    char name[20];
    int age;
    info *next;
} personal_info_t;

info* pHead = NULL; //by default first node will be point to nothing

extern "C" void deleteNode(info* p); //Forward declaration

extern "C" void deleteNodeByName(info** pH, const char* name) {
    info *prevNode, *currNode = *pH;

    while(currNode->next != NULL) {

        if(strcmp(currNode->name, name) == 0) {
            std::cout << "found the match, will delete node named " <<
currNode->name << std::endl;
            deleteNode(prevNode);
            return;
        }
        prevNode = currNode;
        currNode = currNode->next;
    }

    if(currNode->next == NULL) {
       if(strcmp(currNode->name, name) == 0) {
            std::cout << currNode->name << " is the last node...." << std::endl;
            memset((void*) currNode, 0, sizeof(info));
            free((info *) currNode);
            prevNode->next = NULL;
        }
        else {
            std::cout << "cannot find the matching name " << name << std::endl;
        }
   }
}

extern "C" void deleteNode(info* p) {
    info* pNode = p->next;
    std::cout << p->name << " is followed by " << pNode->name << std::endl;
    p->next = pNode->next;
    memset((void*) pNode, 0, sizeof(info));
    free((info *) pNode);
    pNode = p->next;
    std::cout << p->name << " is followed by " << pNode->name << std::endl;
}
extern "C" void createNode(info** pH, const char* name, const int age) {
    info* pNewNode = (info*) malloc(sizeof(info));
    if(pNewNode != NULL) {
        strcpy(pNewNode->name, name);
        pNewNode->age = age;
        pNewNode->next= NULL;
    }
    else {
        return;
    }

    if(*pH == NULL) {
        pNewNode->next= NULL;
        *pH = pNewNode;
        std::cout << (*pH)->name << " is Head, pointing to memory address 0x" << std::hex << std::to_string((size_t)pH) << std::endl;
    }
    else {
        info* pNode = *pH;
        while(pNode->next != NULL) { pNode = pNode->next; }
        pNode->next= pNewNode;
    }
}
extern "C" const char* getName(const info* i) { return i->name; }
extern "C" const int getAge(const info* i) { return i->age;}

};

void print(info* pH) {
    std::cout << getName(pH) << " is aged " << std::to_string(getAge(pH)) << " followed by:" << std::endl;

    info* pNode = pH;
    while(pNode->next != NULL){
        pNode = pNode->next;
        std::cout << getName(pNode) << " aged " << getAge(pNode) << std::endl;
    }
}

int main() {
    createNode(&pHead, "AungBu", 20);
    createNode(&pHead, "Sar Oo", 30);
    createNode(&pHead, "AhBang", 25);
    createNode(&pHead, "AhLain", 27);
    createNode(&pHead, "Ahmedi", 28);

    print(pHead);

    deleteNodeByName(&pHead, "Sar Oo");
    deleteNodeByName(&pHead, "AhLain");

    print(pHead);

    return 0;

}
