#include "Tree.hpp"

/*
    Author(s): James Spaniak
*/

LFile::LFile(int ts, int bs) {
    totalSize = ts;
    blockSize = bs;
    start=NULL;
}

FNode* LFile::getHead() {
    return start;
}

//Append a node to the end of the list with the capacity (numBytes)
void LFile::appendNodeWithSize(int numBytes, int startingAddress) {
    if(start==NULL) {
        start=new FNode(startingAddress, numBytes, NULL, NULL);
        totalSize=numBytes;
        return;
    }
    FNode* temp;
    int tempBy;
    double tempVal;
    totalSize+=numBytes;

    temp = start;
    while(temp->next != NULL) {
        temp=temp->next;
    }
    temp->next = new FNode(startingAddress, numBytes, temp, NULL);
}

//traverse to last node of the list and add numBytes to its capacity
void LFile::fillLastNode(int numBytes) {
    FNode* temp = start;
    while(temp->next != NULL) {
        temp=temp->next;
    }
    totalSize+=numBytes;
    temp->capacity+=numBytes;
}

//Get the remaining space left in the last node of the list
int LFile::getLastNodeSpace() {
    if(start==NULL) {
        return 0;
    }
    FNode* temp = start;
    while(temp->next != NULL) {
        temp=temp->next;
    }
    return blockSize-temp->capacity;
}

//remove numBytes from last node of list
void LFile::remove(int numBytes) {
    if(start==NULL)
        return;
    FNode* curr;
    FNode* prev;
    
    curr = start;
    while(curr->next!=NULL) {
        prev = curr;
        curr = curr->next;
    }

    if(totalSize<=blockSize && numBytes >= totalSize) {
        start = NULL;
        totalSize=0;
        return;
    }
    if(prev==NULL) {
        start->capacity-=numBytes;
        totalSize-=numBytes;
        if(start->capacity==0) {
            start=NULL;
            totalSize=0;
            return;
        }
    }

    if(curr->capacity <= numBytes) {
        prev->next = NULL;
        totalSize = totalSize - curr->capacity;
    } else {
        curr->capacity = curr->capacity - numBytes;
        totalSize = totalSize - numBytes;
    }
}

//print list
void LFile::print(std::string indent) {
    FNode* temp = start;
    while(temp!=NULL) {
        std::cout << indent << "\t" << "Block Address: " << temp->blockAddress << " Capacity: " << temp->capacity << std::endl;
        temp=temp->next;
    }
}

//get size of file
int LFile::getSize() {
    return totalSize;
}

/*
    Check if starting address is present in list
    Returns true if address is found, else false
*/
bool LFile::isAddressPresent(int startingAddress) {
    if(start==NULL)
        return false;
    FNode* temp = start;
    while(temp!=NULL) {
        if(temp->blockAddress==startingAddress)
            return true;
        temp=temp->next;
    }
    return false;
}

LFile::~LFile() {

}