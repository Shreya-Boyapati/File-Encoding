// util.h
//
// Shreya Boyapati
// netid: sboyap2
//
// CS251
// Project 6 - File Compression II
//
// file defines all functions for file modification/compression

#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <queue>          // std::priority_queue
#include <vector>         // std::vector
#include <functional>     // std::greater
#include <string>
#include <cmath>
#include "bitstream.h"
#include "util.h"
#include "hashmap.h"
#include "mymap.h"

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

// freeTree()
//
// frees memory allocated for Huffman tree
void freeTree(HuffmanNode* node) {
    if (node == nullptr) {
      return;
    }

    if (node->zero != nullptr) {
      freeTree(node->zero);
    }

    if (node->one != nullptr) {
      freeTree(node->one);
    }

    delete(node);
}

// buildFrequencyMap()
//
// builds frequency map
// if isFile is true, reads from file
// if isFile is false, reads from string
void buildFrequencyMap(string filename, bool isFile, hashmap &map) {
    if (isFile == true) {
      ifstream  infile;
      infile.open(filename);

      char x;

      while (infile >> noskipws >> x) {
        if (map.containsKey(x) == false) {
          map.put(x, 1);
        } else {
          map.put(x, map.get(x)+1);
        }
      }
      map.put(PSEUDO_EOF, 1);
    } else {
      for (unsigned i = 0; i < filename.length(); i++) {
        char x = filename[i];
        if (map.containsKey(x) == false) {
          map.put(x, 1);
        } else {
          map.put(x, map.get(x)+1);
        }
      }
      map.put(PSEUDO_EOF, 1);
    }
}

// Comparison object to be used to order the heap
struct comp {
	bool operator()(HuffmanNode* l, HuffmanNode* r) {
		return l->count > r->count;
	}
};

// builds encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmap &map) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, comp> pq;

    for (auto key : map.keys()) {
      HuffmanNode *n = new HuffmanNode();
      n->character = key;
      n->count = map.get(key);
      n->zero = nullptr;
      n->one = nullptr;

      pq.push(n);
    }

    while (pq.size() != 1) {
      HuffmanNode *zero = pq.top(); pq.pop();
      HuffmanNode *one = pq.top();	pq.pop();

      int sum = zero->count + one->count;

      HuffmanNode *n = new HuffmanNode();
      n->character = NOT_A_CHAR;
      n->count = sum;
      n->zero = zero;
      n->one = one;
      pq.push(n);
	}

    HuffmanNode* root = pq.top();

    return root;
}

void encode(HuffmanNode* root, string str, mymap <int, string> &encodingMap) {
	if (root == nullptr) {
    return;
  }

	if (!root->zero && !root->one) {
		encodingMap.put(root->character, str);
	}

	encode(root->zero, str + "0", encodingMap);
	encode(root->one, str + "1", encodingMap);
}

//
// *This function builds the encoding map from an encoding tree.
//
mymap <int, string> buildEncodingMap(HuffmanNode* tree) {
    mymap <int, string> encodingMap;

    encode(tree, "", encodingMap);

    return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, mymap <int, string> &encodingMap,
              ofbitstream& output, int &size, bool makeFile) {
    string str = "";
    char x;

    while (input >> noskipws >> x) {
      str += encodingMap.get(x);
    }
    str += encodingMap.get(PSEUDO_EOF);

    if (makeFile == true) {
      for (auto e : str) {
        if (e == '1') {
          output.writeBit(1);
        }
        else {
          output.writeBit(0);
        }
      }
      size = str.size();
    }

    return str;
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
  string result = "";
  HuffmanNode* cur = encodingTree;
  
  int bit = input.readBit();
  
  while (!input.eof()) {
    if (bit == 1) {
      cur = cur->one;
    }
    else {
      cur = cur->zero;
    }
    
    if (cur->character != NOT_A_CHAR) {
      if (cur->character == PSEUDO_EOF) {
        break;
      }
      result += cur->character;
      cur = encodingTree;
    }
    
    bit = input.readBit();
  }
  
  for (auto r : result) {
    output.put(r);
  }

  return result;
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    hashmap frequencyMap;
    HuffmanNode* encodingTree = nullptr;
    mymap <int, string> encodingMap;

    buildFrequencyMap(filename, true, frequencyMap);
    encodingTree = buildEncodingTree(frequencyMap);
    encodingMap = buildEncodingMap(encodingTree);

    ofbitstream output(filename + ".huf");
    ifstream input(filename);

    output << frequencyMap;
    
    int size = 0;
    string codeStr = encode(input, encodingMap, output, size, true);

    freeTree(encodingTree);

    return codeStr;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
    HuffmanNode* encodingTree;
    string result = "";
     size_t pos = filename.find(".huf");
    if ((int)pos >= 0) {
            filename = filename.substr(0, pos);
        }
        pos = filename.find(".");
        string ext = filename.substr(pos, filename.length() - pos);
        filename = filename.substr(0, pos);
        ifbitstream input(filename + ext + ".huf");
        ofstream output(filename + "_unc" + ext);
        
        hashmap fmap;
        input >> fmap;  // get rid of frequency map at top of file
        encodingTree = buildEncodingTree(fmap);
        result = decode(input, encodingTree, output);
    freeTree(encodingTree);
    
    return result;
}
