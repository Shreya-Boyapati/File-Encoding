// mymap.h
//
// Shreya Boyapati
// netid: sboyap2
//
// CS251
// Project 5 - mymap
//
// header file defines mymap class
#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

using namespace std;

template<typename keyType, typename valueType>
class mymap {
 private:
    struct NODE {
        keyType key;
        valueType value;
        NODE* left;  // pointer to left child
        NODE* right;  // pointer to right child
        int nL;  // # of nodes in left subtree
        int nR;  // # of nodes in right subtree
        bool isThreaded;
    };
    NODE* root;
    int size;


    // iterator:
    // 
    // iterator used in foreach loop to iterate through mymap item
    struct iterator {
     private:
        NODE* curr;  // points to current in-order node for begin/end

     public:
        iterator(NODE* node) {
            curr = node;
        }

        keyType operator *() {
            return curr -> key;
        }
        
        // operators == and != check equality of key/value pairs
        bool operator ==(const iterator& rhs) {
            if (curr == nullptr && rhs.curr == nullptr) {
              return this == &rhs;
            }
            return curr == rhs.curr;
        }

        bool operator !=(const iterator& rhs) {
            if (curr == nullptr && rhs.curr == nullptr) {
              return this == &rhs;
            }
            return curr != rhs.curr;
        }

        bool isDefault() {
            return !curr;
        }

        // operator++:
        //
        // advances curr to next in-order node
        iterator operator++() {
            if (curr->isThreaded == true) {
              curr = curr->right;
            } else {
              curr = curr->right;
              if (curr->left != nullptr) {
                while (curr->left != nullptr) {
                  curr = curr->left;
                }
              }
            }

            return iterator(curr);
        }
    };
    
    NODE* createNode(keyType key, valueType value) {
      NODE* n = new NODE();
      n->key = key;
      n->value = value;
      n->left = nullptr;
      n->right = nullptr;
      n->isThreaded = true;
      n->nL = 0;
      n->nR = 0;
      
      return n;
    }
    
    void insert(NODE*& prev, NODE*& n, keyType key) {
      if (prev == nullptr) {
        root = n;
      } else if (key < prev->key) {
        n->right = prev;
        prev->left = n;
      } else {
        n->right = prev->right;
        prev->isThreaded = false;
        prev->right = n;
      }
    }
    
    // leftMost()
    //
    // finds node in BST that is furthest to the left
    NODE* leftMost(NODE* n) {
      if (n == nullptr) {
        return nullptr;
      }
 
      while (n->left != nullptr) {
        n = n->left;
      }

      return n;
    }

    // postorder()
    //
    // does postorder traversal on threaded BST and deletes each node
    // deletes BST and resets size to 0
    void postorder(NODE* root) {
      if (root == nullptr) {
        return;
      }
      
      if(root->left != nullptr) {
        postorder(root->left);
      }

      if(root->right != nullptr && root->isThreaded == false) {
        postorder(root->right);
      }

      delete(root);
      size = 0;
    }
    
    // balanced()
    //
    // checks if given node is balanced, returns true/false
    bool balanced(NODE* n) {
      int balance = 2 * min(n->nL, n->nR) + 1;
      
      if (max(n->nL, n->nR) <= balance) {
        return true;
      }
      
      return false;
    }
    
 public:
    // default constructor:
    //
    // creates an empty mymap
    // BST is initialized with size 0, nullptr root
    mymap() {
        root = nullptr;
        size = 0;
    }

    // copy constructor:
    //
    // called automatically by C++ to create a copy of an existing mymap
    // new mymap is a copy of "other" mymap
    mymap(const mymap& other) {
        this->size = 0;
        this->root = nullptr;

        NODE* cur = other.root;
      
        while (cur != nullptr) {
          this->put(cur->key, cur->value);
          
          if (cur->left != nullptr) {
            cur = cur->left;
          }
          else if(cur->isThreaded == false) {
            cur = cur->right;
          }
          else {
            while(cur->right != nullptr && cur->isThreaded == true) {
                cur = cur->right;
            }    

            if(cur->right == nullptr) {
              break;
            }
            else {
              cur = cur->right;
            }
                
          }
        }
    }

    // operator=:
    //
    // clears "this" mymap and makes it a copy of "other" mymap
    // sets one mymap object equal to another, i.e. this = other;
    mymap& operator=(const mymap& other) {
        postorder(root);
        this->size = 0;

        NODE* cur = other.root;
      
        while (cur != nullptr) {
          this->put(cur->key, cur->value);
          
          if (cur->left != nullptr) {
            cur = cur->left;
          }
          else if(cur->isThreaded == false) {
            cur = cur->right;
          }
          else {
            while(cur->right != nullptr && cur->isThreaded == true) {
                cur = cur->right;
            }    

            if(cur->right == nullptr) {
              break;
            }
            else {
              cur = cur->right;
            }
                
          }
        }
        
        return *this;
    }

    // clear()
    //
    // frees memory associated with mymap
    // resets all private member variables to default values
    void clear() {
        if (root == nullptr) {
          return;
        }
       
        postorder(root);
        root = nullptr;
    }

    // destructor:
    //
    // called automatically by C++ to free memory associated with mymap
    ~mymap() {
        if (root == nullptr) {
          return;
        }
       
        postorder(root);
    }

    // put()
    //
    // inserts the key/value into threaded, self-balancing BST
    // updates value if duplicated key is passed
    void put(keyType key, valueType value) {
       NODE* prev = nullptr;
       NODE* cur = root;

       bool duplicate = contains(key);

       while (cur != nullptr) {
         if (key == cur->key) {
           cur->value = value;
           return;
         }

         prev = cur;

         if (key < cur->key) {
           if (duplicate == false) {
             prev->nL++;
           }
           cur = cur->left;
         } else {
           if (duplicate == false) {
             prev->nR++;
           }

           if (cur->isThreaded) {
             cur = nullptr;
           } else {
             cur = cur->right;
           }
         }
       }
       
      NODE* n = createNode(key, value);
      insert(prev, n, key);

      size++;
    }

    // contains()
    // 
    // returns true if given key is in mymap, return false if not
    bool contains(keyType key) {
      NODE* cur = root;
      
      while (cur != nullptr) {
        if (key == cur->key) {
          return true;
        }

        if (key < cur->key) {
          cur = cur->left;
        } else {
          if (cur->isThreaded) {
            cur = nullptr;
          } else {
            cur = cur->right;
          }
        }
      }

      return false;
    }

    // get()
    //
    // returns corresponding value for given key
    // if the key is not found default value, valueType(), is returned
    valueType get(keyType key) {
      NODE* cur = root;
      
      while (cur != nullptr) {
        if (key == cur->key) {
          return cur->value;
        }

        if (key < cur->key) {
          cur = cur->left;
        } else {
          cur = cur->right;
        }
      }

      return valueType();
    }

    // operator[]:
    //
    // returns corresponding value for given key
    // if the key is not found default value, valueType(), is returned
    // and (key, valueType()) is inserted into the map
    valueType operator[](keyType key) {
      NODE* prev = nullptr;
      NODE* cur = root;
      
      while (cur != nullptr) {
        if (key == cur->key) {
          return cur->value;
        }

        prev = cur;

        if (key < cur->key) {
          cur = cur->left;
        } else {
          if (cur->isThreaded) {
            cur = nullptr;
          } else {
            cur = cur->right;
          }
        }
      }

      NODE* n = createNode(key, valueType());
      insert(prev, n, key);

      size++;

      return valueType();
    }

    // size()
    //
    // returns # of key/value pairs in mymap, 0 if empty
    int Size() {
        return size;
    }

    // begin:
    //
    // returns an iterator to the first inorder NODE
    iterator begin() {
        NODE* cur = leftMost(root);

        return iterator(cur);
    }

    // end:
    //
    // returns an iterator to the last inorder NODE (given)
    iterator end() {
        return iterator(nullptr);
    }

    // toString()
    //
    // returns a string of the entire mymap in order
    // Format: "key: a value: x\nkey: b value: y\nkey: c value: z\n"
    string toString() {
        string x = "";

        NODE* cur = leftMost(root);
        
        while (cur != nullptr) {
          stringstream ss;
          string foo, f1;
        
          ss << cur->key << " " << cur->value;
          ss >> foo >> f1;

          x = x + "key: " + foo + " value: " + f1 + "\n";
      
          if (cur->isThreaded == true) {
            cur = cur->right;
          } else {
            cur = leftMost(cur->right);
          }
        }
        
        return x;
    }

    // toVector()
    //
    // returns a vector of the entire mymap in order
    // Format: {{a, x}, {b, y}, {c, z}}
    vector<pair<keyType, valueType> > toVector() {
        vector<pair<keyType, valueType> > v;

        NODE* cur = leftMost(root);
        
        while (cur != nullptr) {
          v.push_back(make_pair(cur->key, cur->value));
      
          if (cur->isThreaded == true) {
            cur = cur->right;
          } else {
            cur = leftMost(cur->right);
          }
        }

        return v;
    }

    // checkBalance()
    //
    // returns a preordered string of mymap that verifies balance of tree
    // Format: "key: 2, nL: 1, nR: 1\nkey: 1, nL: 0, nR: 0\nkey: 3, nL: 0, nR: 0\n";
    string checkBalance() {
        string x = "";

        NODE* cur = root;
      
        while (cur != nullptr) {
          stringstream ss;
          string foo, f1, f2;

          ss << cur->key << " " << cur->nL << " " << cur->nR;
          ss >> foo >> f1 >> f2;

          x = x + "key: " + foo + ", nL: " + f1 + ", nR: " + f2 + "\n";
          
          if (cur->left != nullptr) {
            cur = cur->left;
          }
          else if(cur->isThreaded == false) {
            cur = cur->right;
          }
          else {
            while(cur->right != nullptr && cur->isThreaded == true) {
                cur = cur->right;
            }    

            if(cur->right == nullptr) {
              break;
            }
            else {
              cur = cur->right;
            }
                
          }
        }

        return x;
    }
};
