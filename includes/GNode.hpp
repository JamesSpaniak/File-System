#ifndef _GNode_HPP_
#define _GNode_HPP_

#include <list>
#include <cstdio>
#include <string>
#include "LFile.hpp"

struct GNode {
    GNode(std::string nm, int ts, int sz, GNode* pt) {
        name=nm;
        size=sz;
        parent=pt;
        file = NULL;
        timeStamp=ts;
    }

    GNode(std::string nm, int ts, int sz, GNode* pt, int tsize, int bsize) {
        name=nm;
        size=sz;
        parent=pt;
        file = new LFile(tsize, bsize);
        timeStamp=ts;
    }

    ~GNode() {
        delete file;
    }

    LFile* file;
    std::string name; 
    int timeStamp;
    int size;
    std::list<GNode*> children;
    GNode* parent;
};

#endif
