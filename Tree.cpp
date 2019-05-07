#include "Tree.hpp"

/*
    Author(s): James Spaniak
*/

//struct for use with Breadth-first search order
struct GNodeLevel {
    GNode *data;
    int level;

    GNodeLevel(GNode *d, int l) {
        level = l;
        data = d;
    }
};

//constructors
Tree::Tree(int bs, int b) {
    numBlocks = b;
    blockSize = bs;
    root = NULL;
    disk = NULL;
    currentDir = NULL;
}

Tree::Tree(int bs, int b, LDisk *d) {
    numBlocks = b;
    blockSize = bs;
    root = NULL;
    disk = d;
    currentDir = NULL;
}

/*
    Adds a 'GNode' to the directory tree, if the node to be added is a file then size >= 0
    if it is a directory it will have a size==-1.

    name should always be the full expected path not a relative path
*/
void Tree::addNode(int size, std::string name) {
    if(name.length()==0) {
        std::cout << "File name length must be greater than 0" << std::endl;
    }
    int length, counter, count, i;
    int check = 1;
    bool found;
    GNode* parent;
    GNode* curr;
    size_t pos;
    std::list<GNode*>::iterator it;
    pos=name.find_last_of("/");

    if(pos==name.length()-1) {
        name = name.substr(0, pos);
    }
    
    if(root==NULL) {
        root = new GNode(name, -1, -1, NULL);
        currentDir = root;
        return;
    }

    curr = lookUp(name);
    if(curr==NULL) {
        std::cout << "Unknown directory: " << name << std::endl;
        return;
    }
    pos=name.find_last_of("/");
    if(pos!=std::string::npos) {
        name = name.substr(pos+1);
    }
    parent = curr;

    //initialize the linked list for the file
    found = false;
    counter=0;
    length=parent->children.size();
    //check to make sure name isnt a duplicate
    for (it = parent->children.begin(); it != parent->children.end(); std::advance(it, 1)) {
        if(!(*it)->name.compare(name)) {
            std::cout << "File / Directory already exists" << std::endl;
            return;
        }
        counter++;
        if(counter>=length) {
            break;
        }
    }
    //if size > -1 it is not a file and should be initialized with a LFile as a member
    if(size > -1) {
        curr = new GNode(name, -1, size, parent, size/blockSize, blockSize);
    } else {
        curr = new GNode(name, -1, size, parent);
    }
    //allocate diskspace if size of file is greater then 0
    if(size > 0) {
        check = disk->insert(size, curr->file);
    } else {
        check=0;
    }
    if(size == -1 || check==0) {
        (parent->children).push_back(curr);
    } else {
        std::cout << "Out of space" << std::endl;
    }
    
}

/*
    Print entire tree directory
    Prints tree in DFS fashion if isDFS is set, else print in BFS fashion
*/
void Tree::prePrint(bool isDFS) {
    disk->print(blockSize);
    if(isDFS)
        printDFS(root, 0);
    else
        printStage(root);
        
}

//print directory in depth-first search order
void Tree::printDFS(GNode* curr, int level) {
    if(curr==NULL)
        return;
    std::list<GNode*>::iterator it;
    std::string indent;
    int new_level = level+1;
    for(int i = 0; i < level; i++) indent = indent + " ";
    if(curr->size > -1) {
        std::cout << indent << "+: " << curr->name << std::endl;;
        curr->file->print(indent);
    } else {
        std::cout << indent << "+: " << curr->name << "/" << std::endl;
    }
    int size = curr->children.size();
    int counter = 0;
    for (it = curr->children.begin(); it != curr->children.end(); std::advance(it, 1)) {
        printDFS((*it), new_level);
        counter++;
        if(counter>=size) {
            break;
        }
    }


}

//print directory in breadth-first search order
void Tree::printStage(GNode* curr) {
    if(curr==NULL)
        return;
    std::list<GNode*>::iterator it;
    std::list<GNodeLevel*> queue;
    GNodeLevel* temp;
    std::string indent;
    int size, counter;
    temp = new GNodeLevel(curr, 0);
    queue.push_back(temp);
    while(!queue.empty()) {
        temp = queue.front();
        indent = "";
        for(int i = 0; i < temp->level; i++) indent = indent + " ";
        if(temp->data->size > -1) {
            std::cout << indent << "+: " << temp->data->name << std::endl;
        } else {
            std::cout << indent << "+: " << temp->data->name << "/" << std::endl;
        }
        queue.pop_front();
        size = temp->data->children.size();
        counter = 0;
        for (it = temp->data->children.begin(); it != temp->data->children.end(); std::advance(it, 1)) {
            queue.push_back(new GNodeLevel(*it, (temp->level)+1));
            counter++;
            if(counter>=size) {
                break;
            }
        }
    }
}

/*
    function for when user runs command cd <name>
    2 cases:
        1. starts with / -> cd from root
        2. cd from relative path
*/
void Tree::cd(std::string name) {
    int check;
    GNode* saveCurrent = currentDir;
    size_t pos = name.find_last_of("/");
    if(pos==name.length()-1) name = name.substr(0, pos);
    pos = name.find_first_of("/");
    //from root
    if(pos==0) {
        name = name.substr(1);
        currentDir = root;
        check = cdFromRoot(name);
        if(check==1) {
            std::cout << "Unknown Directory" << std::endl;
            currentDir = saveCurrent;
            return;
        }
        return;
    }
    //relative
    std::string token;
    while ((pos = name.find("/")) != std::string::npos) {
        token = name.substr(0, pos);
        name.erase(0, pos + 1);
        check = cdHelper(token);
        if(check==1) {
            std::cout << "Unknown Directory" << std::endl;
            currentDir = saveCurrent;
            return;
        }
    }
    check = cdHelper(name);
    if(check==1) {
        std::cout << "Unknown Directory" << std::endl;
        currentDir = saveCurrent;
    }
}

//cd helper function
int Tree::cdHelper(std::string name) {
    if(!name.compare("..")) {
        if(currentDir->parent!=NULL) {
            currentDir = currentDir->parent;
            return 0;
        }
    } else {
        int size = currentDir->children.size();
        int counter = 0;
        std::list<GNode*>::iterator it;
        for (it = currentDir->children.begin(); it != currentDir->children.end(); std::advance(it, 1)) {
            if(!(*it)->name.compare(name)) {
                if((*it)->size==-1) {
                    currentDir = (*it);
                    return 0;
                }
            }
            counter++;
            if(counter>=size) {
                break;
            }
        }
    }
    return 1;
}

// runs with cd.. command
void Tree::cdOut() {
    cd("..");
}

//helper function for cd'in from root, ie when user runs cd /<path>
int Tree::cdFromRoot(std::string name) {
    int check;
    size_t pos = name.find_last_of("/");
    std::string token;
    while ((pos = name.find("/")) != std::string::npos) {
        token = name.substr(0, pos);
        name.erase(0, pos + 1);
        check = cdHelper(token);
        if(check==1) {
            return 1;
        }
    }
    return cdHelper(name);
}

//ls command
void Tree::ls() {
    int size = currentDir->children.size();
    int counter = 0;
    std::list<GNode*>::iterator it;
    for (it = currentDir->children.begin(); it != currentDir->children.end(); std::advance(it, 1)) {
        if((*it)->size == -1) {
            std::cout << (*it)->name << "/" << std::endl;
        } else {
            std::cout << (*it)->name << std::endl;
        }
        counter++;
        if(counter>=size) {
            break;
        }
    }
}

//mkdir command
void Tree::mkdir(std::string name) {
    //before adding node determine if name is a relative path or absolute path
    //if it is a relative path prepend the absolute path
    size_t pos = name.find_first_of("/");
    if(pos==0) {
        name = root->name + name;
        addNode(-1, name);
    } else {
        addNode(-1, getCurrentWorkingDirectory() + name);
    }
}
void Tree::create(std::string name) {
    //before adding node determine if name is a relative path or absolute path
    //if it is a relative path prepend the absolute path
    size_t pos = name.find_first_of("/");
    if(pos==0) {
        name = root->name + name;
        addNode(0, name);
    } else {
        addNode(0, getCurrentWorkingDirectory() + name);
    }
}

/*
    Similar to addNode except when destination is reached just add bytes instead of creating another node
*/
void Tree::append(std::string name, int bytes) {
    std::cout << bytes << std::endl;
    if(root==NULL) {
        std::cout << "Null Root" << std::endl;
        return;
    }
        
    int length, counter;
    GNode* curr = root;
    std::list<GNode*>::iterator it;
    //before adding node determine if name is a relative path or absolute path
    //if it is a relative path prepend the absolute path
    size_t pos = name.find_first_of("/");
    if(pos==0) {
        name = root->name + name;
    } else {
        name = getCurrentWorkingDirectory() + name;
    }

    curr = lookUp(name);
    if(curr==NULL) {
        std::cout << "Unknown Directory: " << name << std::endl;
        return;
    }

    pos=name.find_last_of("/");
    if(pos!=std::string::npos) {
        name = name.substr(pos+1);
    }
    counter=0;
    length=curr->children.size();
    //check to make sure name isnt a duplicate
    for (it = curr->children.begin(); it != curr->children.end(); std::advance(it, 1)) {
        if(!(*it)->name.compare(name)) {
            if((*it)->file==NULL) {
                std::cout << "Error: No file list found: " << (*it)->name << std::endl;
                return;
            }
            int check = disk->insert(bytes, (*it)->file);
            if(check!=0) {
                std::cout << "Out of space" << std::endl;
                return;
            } else {
                (*it)->size += bytes;
                return;
            }
        }
        counter++;
        if(counter>=length) {
            break;
        }
    }
}

//Remove bytes from file as specified
void Tree::remove(std::string name, int bytes) {
    size_t pos;
    int length, counter, count, i;
    bool found;
    std::string token;
    GNode* curr = root;
    std::list<GNode*>::iterator it;

    pos = name.find_first_of("/");
    if(pos==0) {
        name = root->name + name;
    } else {
        name = getCurrentWorkingDirectory() + name;
    }

    curr = lookUp(name);
    if(curr==NULL) {
        std::cout << "Unknown Directory: " << name << std::endl;
        return;
    }
    pos=name.find_last_of("/");
    if(pos!=std::string::npos) {
        name = name.substr(pos+1);
    }

    length=curr->children.size();
    //check to make sure name isnt a duplicate
    for (it = curr->children.begin(); it != curr->children.end(); std::advance(it, 1)) {
        if(!(*it)->name.compare(name)) {
            if((*it)->file==NULL) {
                std::cout << "Error: No file list found: " << (*it)->name << std::endl;
                return;
            }
            disk->remove(bytes, (*it)->file);
            disk->merge();
            
        }
        counter++;
        if(counter>=length) {
            break;
        }
    }
}

void Tree::deleteNode(std::string name) {
    std::string orig_name = name;
    size_t pos;
    int length, counter, count, i;
    bool found;
    std::string token;
    GNode* curr = root;
    std::list<GNode*>::iterator it;
    //before adding node determine if name is a relative path or absolute path
    //if it is a relative path prepend the absolute path
    pos = name.find_first_of("/");
    if(pos==0) {
        name = root->name + name;
    } else {
        name = getCurrentWorkingDirectory() + name;
    }

    curr = lookUp(name);
    if(curr==NULL) {
        std::cout << "Unknown Directory: " << name << std::endl;
        return;
    }
    pos=name.find_last_of("/");
    if(pos!=std::string::npos) {
        name = name.substr(pos+1);
    }

    length=curr->children.size();
    counter=0;
    //find file and delete
    for (it = curr->children.begin(); it != curr->children.end(); std::advance(it, 1)) {
        if(!(*it)->name.compare(name)) {
            //if file size is -1 it is a directory
            if((*it)->size > -1) {
                //remove bytes from LDisk and appropiate LFile as well
                remove(orig_name, (*it)->size);
                curr->children.remove(*it);
                return;
            } else {
                if((*it)->children.size() > 0) {
                    std::cout << "Directory is not empty" << std::endl;
                } else {
                    curr->children.remove(*it);
                }
                return;
            }
        }
        counter++;
        if(counter>=length) {
            break;
        }
    }
}

void Tree::dir() {
    printStage(root);
}

//print disk footprint
void Tree::printDisk() {
    disk->print(true);
}

//print file footprint
void Tree::printFiles() {
    printDFS(root, 0);
}

Tree::~Tree() {

}

//set the current directory as the root for setup purposes
void Tree::setHandleAsRoot() {
    currentDir = root;
}

//Return current working directory as a string of the path
std::string Tree::getCurrentWorkingDirectory() {
    std::string dir = "";
    GNode* temp = currentDir;
    while(temp->parent!=NULL) {
        dir = temp->name + "/" + dir;
        temp = temp->parent;
    }
    return root->name + "/" + dir;
}



//private methods
void Tree::pFile(GNode * dir) {

}

/*
    Searches for node in directory tree using name which is a path to the desired location

    For use with addNode, append, remove, and delete
*/
GNode* Tree::lookUp(std::string name) {
    int length, counter, count, i;
    bool found;
    std::string token;
    GNode* curr = root;
    std::list<GNode*>::iterator it;
    //remove first portion of path because we are already at root
    size_t pos = name.find("/");
    token = name.substr(0, pos);
    name.erase(0, pos + 1);
    //traverse "directory string" by splitting names at '/'
    while ((pos = name.find_first_of("/")) != std::string::npos) {
        token = name.substr(0, pos);
        name.erase(0, pos + 1);
        length = curr->children.size();
        counter = 0;
        found = false;
        count=0;
        for(i=0; i<name.length(); i++) if(name[i]=='/') count++;
        //iterate through the subdirectories of the current node using the iterator it
        for (it = curr->children.begin(); it != curr->children.end(); std::advance(it, 1)) {
            /*
                For some reason the iterator keeps going past the length of the list so this counter is needed to
                avoid a seg fault, unsure on why this is happening
            */
            if(counter>=length || found)
                break;
            std::string temp = (*it)->name;
            if(!temp.compare(token)) {
                curr=(*it);
                found=true;
            }
            counter++;
        }
        if(!found && count>1) {
            return NULL;
        }
    }
    return curr;
}