#include "Tree.hpp"

/*
    Author(s): James Spaniak
*/

LDisk::LDisk(int blocks, const int blockSize_) {
    totalBlocks = blocks;
    blockSize = blockSize_;

    DNode *newHead = new DNode(0, totalBlocks-1, 0);
    head = newHead; 
}

/*
    insert new data into the LDisk LL if it will fit
    size: size of file in blocks
    takes in the LFile linked list that corresponds to the file being inserted so we can keep track of what blocks each file is in on the disk
*/
int LDisk::insert(int size, LFile* fhead) {
    DNode* temp = head;
    int lastNodeSpace, compare, blockSplit, available, origSize, diskSlotSpace;
    bool weShouldMerge;

    //first check there will be enough space to add
    available = 0;
    while(temp!=NULL) {
        if(!temp->usedBit)
            available += temp->end - temp->start + 1;

        if(available >= (size + blockSize - 1)/blockSize)
            break;

        temp=temp->next;
    }
    if(available < (size + blockSize - 1)/blockSize) 
        return 1;
    //if we are here there is enough space to add the file in question
    //reset temp to head
    temp = head;
    DNode *prev = NULL;
    while(size > 0) {
        while(temp->usedBit) {
            prev = temp;
            temp=temp->next;
            if(temp==NULL) {
                return 1;
            }
        }
        //if there is space left on the last node we want to add it there first before appending another
        lastNodeSpace = fhead->getLastNodeSpace();
        if(lastNodeSpace > 0) {
            compare = std::min(lastNodeSpace, size);
            size-=compare;
            fhead->fillLastNode(compare);
        } else {
            //append another node
            blockSplit = temp->end-temp->start+1;
            if(prev!=NULL) {
                weShouldMerge = fhead->isAddressPresent(prev->start*blockSize);
            } else {
                weShouldMerge=false;
            }
            if(size <= blockSplit*blockSize) { //bytes to add can fit in current space of open nodes
                origSize = size;
                while(size) { //add one node at a time
                    compare = std::min(size, blockSize);
                    size-=compare;
                    if(weShouldMerge)
                        fhead->appendNodeWithSize(compare, prev->start*blockSize);
                    else
                        fhead->appendNodeWithSize(compare, temp->start*blockSize);
                }
                split(temp, (origSize+blockSize-1)/blockSize, 1);
                if(weShouldMerge) {
                    prev->next = temp->next;
                    prev->end = temp->end;
                }
            } else { //we can fill this space completely but we need to look for more space after
                diskSlotSpace = blockSplit*blockSize;
                while(diskSlotSpace) {
                    fhead->appendNodeWithSize(blockSize, temp->start*blockSize);
                    diskSlotSpace-=blockSize;
                }
                size-=diskSlotSpace;
                temp->usedBit=1;
                if(weShouldMerge) {
                    prev->next = temp->next;
                    prev->end = temp->end;
                }
            }
        }
    }


    return 0;
}

//remove number of bytes (size) from file
void LDisk::remove(int size, LFile* fhead) {
    FNode* temp;
    DNode* diskNode = head;
    DNode* diskNodePrev = NULL;
    int address;
    int bytes;
    bool check;

    //dont want to set size of file to below 0
    if(size > fhead->getSize()) {
        size = fhead->getSize();
    }
    while(size > 0) {
        temp = fhead->getHead();
        diskNode = head;
        diskNodePrev = NULL;
        if(temp==NULL)
            return;
        while(temp->next!=NULL) {
            temp = temp->next;
        }
        address = temp->blockAddress/blockSize;
        if(temp==NULL)
            return;

        check=true;
        while(diskNode!=NULL && check) {
            if(address >= diskNode->start && address <= diskNode->end) {
                check=false;
            }
            diskNodePrev = diskNode;
            diskNode=diskNode->next;
        }

        if(diskNode==NULL) {
            std::cout << "Could not find block address" << std::endl;
            return;
        }

        if(temp->capacity <= size && size <= blockSize) {
            fhead->remove(temp->capacity);
            size-=temp->capacity;
        } else if(temp->capacity < blockSize && size >= blockSize) {
            fhead->remove(temp->capacity);
            size-=temp->capacity;
        } else if(size>=blockSize) {
            bytes = blockSize;
            fhead->remove(bytes);
            size-=bytes;
        } else {
            fhead->remove(size);
            size=0;
            return;
        }
        if(diskNodePrev->start==diskNodePrev->end) {
            diskNodePrev->usedBit=0;
        } else {
            split(diskNodePrev, diskNodePrev->end-diskNodePrev->start, 1);
        }
    }
}

/*
    Merges all adjacent unused nodes together
*/
void LDisk::merge() {
    DNode* temp = head;
    while(temp->next!=NULL) {
        if(!temp->usedBit && !temp->next->usedBit) {
            temp->end = temp->next->end;
            temp->next = temp->next->next;
        } else {
            temp=temp->next;
        }
    }
}

//print linked list
void LDisk::print(bool debug) {
    if(head==NULL)
        return;
    DNode *temp = head;
    int fragmentation = 0;
    int currStatus = temp->usedBit;
    int currStart=temp->start;
    int currEnd = temp->end;
    if(debug) {
        while(temp!=NULL) {
            printNode(temp);
            if(!temp->usedBit) {
                fragmentation+=temp->end-temp->start+1;
            }
            temp=temp->next;
        }
    } else {
        while(temp!=NULL) {
            if(temp->usedBit!=currStatus) {
                if(currStatus) {
                    std::cout << "Used: " << currStart << " - " << currEnd << std::endl;
                } else {
                    std::cout << "Free: " << currStart << " - " << currEnd << std::endl;
                }
            }
            currEnd=temp->end;
            temp=temp->next;
        }
    }
    std::cout << "Fragmentation: " << fragmentation << std::endl;
}

LDisk::~LDisk() {

}
/*
    Function to split a node into two nodes in the same location of the linked list
    Params:
        node: DNode to be split
        toSplit: size of node to be cut off
        toStatus: new status of node (1, 0)
*/
void LDisk::split(DNode *node, int toSplit, int toStatus) {
    int available;
    int oldEnd, oldStart;
    int newStatus = 1 - toStatus;
    DNode *newNode;

    oldStart = node->start;//28
    oldEnd = node->end;//33
    newNode = new DNode(oldStart+toSplit, oldEnd, newStatus);//28+5
    node->end = oldStart+toSplit-1;
    node->usedBit = toStatus;

    newNode->next = node->next;
    node->next = newNode;
}

void LDisk::printNode(DNode *node) {
    if(node->usedBit) {
        std::cout << "Used: " << node->start << " - " << node->end << std::endl;
    } else {
        std::cout << "Free: " << node->start << " - " << node->end << std::endl;
    }
}