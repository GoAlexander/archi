#include <queue>
#include <algorithm> /* sort */
#include <fstream>
#include <cassert>
#include "node.h"
#include "verbose.h" //for debugging
#include "huffman.h"

using std::priority_queue;

//Begin of private functions
void Huffman::readBinary(std::ifstream &in, 
           Size_t        &num)
{
    num = 0;
    BYTE c = 0;
    for(int k = 3; k != -1; --k)
    {
        c = in.get();
        num += c * (1 << (8 * k));
    }
}

void Huffman::writeBinary(std::ofstream &out, 
            Size_t         num)
{
    assert(num <= 4294967295); // 2^32 - 1
  
    for(int k = 3; k != -1; --k)
    {
        out.put(static_cast<BYTE>((num >> (8 * k))  % 256));
    }
}

void Huffman::assignLength(Node const   *root, 
             vector<BYTE> &length,
             BYTE          level)
{
    if (!root->left && !root->right)
        length[root->c] = level;

    if (root->left)
        assignLength(root->left,  length, level + 1);

    if (root->right)
        assignLength(root->right, length, level + 1);
}



void Huffman::buildTable(BYTE                   max_length,
           vector<BYTE>          &numberOfCodes,
           vector<BYTE>          &startCode,
           vector<vector<BYTE> > &buckets,
           vector<vector<bool> > &table)
{
    BYTE counter = 0;
    //Build code from start code
    for (BYTE i = 1; i <= max_length; ++i)
        if (numberOfCodes[i])
        {
            counter = startCode[i];
            vector<BYTE>::const_iterator p;
            for (p = buckets[i].begin(); p != buckets[i].end(); ++p)
            {
                //Get first i bits in reversed order
                for (BYTE bit = 0; bit < i; ++bit)
                    table[*p].push_back(counter & (1 << bit));
                ++counter;
            }
        }
}

void Huffman::buildStartCode(BYTE          max_length, 
               vector<BYTE> &numberOfCodes,
               vector<BYTE> &startCode)
{
    BYTE code = 0;
    BYTE lastNumberOfCodes = 0; //for this we made vector of size max_length+1
  
    //Find start code for each length.
    for (BYTE count = max_length; count != 0; --count)
    {
        code += lastNumberOfCodes;
        code >>= 1;
        startCode[count] = code;
        lastNumberOfCodes = numberOfCodes[count];
    }
}

Node const* Huffman::buildEncodeTree(vector<Size_t> &freq)
{
    priority_queue<Node *, vector<Node *>, ByFrequency> t;
    for (int c = 0; c != MAX_SYMBOLS; ++c)
        if (freq[c])
        {
            Node *p = new Node(freq[c], static_cast<BYTE>(c));
            t.push(p);      
        }	
  
    while (t.size() > 1)
    {
        Node *L = t.top();
        t.pop();
      
        Node *R = t.top(); 
        t.pop();
      
        Node *parent = new Node(L, R); 
        t.push(parent);
    }
    return t.top();
}

Node const* Huffman::buildDecodeTree(vector<BYTE>          &symb,
                vector<vector<bool> > &table)
{
    Node *root = new Node;
    Node *cur = root;
    vector<BYTE>::iterator sit;
    for (sit = symb.begin(); sit != symb.end(); ++sit)
    {
        BYTE size = table[*sit].size();
        cur = root;
        for (int bit = static_cast<int>(size) - 1; bit != -1; --bit)
        {
            if (table[*sit][bit]) //go right
            {
                if (!cur->right)
                {
                    Node *R = new Node;
                    cur->right = R;
                }
                cur = cur->right;
            }
            else //go left
            {
                if (!cur->left)
                {
                    Node *L = new Node;
                    cur->left = L;
                }
                cur = cur->left;
            }
        }
        cur->c = (*sit); //leaf
    }
    return root;
}

void Huffman::destroyTree(Node const *root)
{
    if (root) {
        destroyTree(root->left);
        destroyTree(root->right);
        delete root;
    }
}

//Format: filesize max_length numberOfCodes[] chars_in_order 
void Huffman::writeOverhead(BYTE                   max_length,
              vector<BYTE>          &numberOfCodes,
              vector<vector<BYTE> > &buckets,
              std::ofstream         &out,
              Size_t                size)
{
    writeBinary(out, size);
    out.put(max_length);
    for (BYTE i = 1; i <= max_length; ++i)
        out.put(numberOfCodes[i]);
  
    for (BYTE i = 1; i <= max_length; ++i)
    {
        vector<BYTE>::const_iterator p;
        for (p = buckets[i].begin(); p != buckets[i].end(); ++p)
            out.put(*p);
    }
}

vector<vector<BYTE> > Huffman::readOverhead(std::ifstream  &in,
             BYTE           &max_length,
             vector<BYTE>   &numberOfCodes,
             vector<BYTE>   &symb,
	     Size_t         &size)
{
    readBinary(in, size);
    max_length = in.get();
    for (BYTE i = 1; i <= max_length; ++i)
        numberOfCodes[i] = in.get();
  
    vector<vector<BYTE> > buckets(max_length + 1, vector<BYTE>());
  
    for (BYTE l = 1; l <= max_length; ++l)
        for (BYTE i = 0; i != numberOfCodes[l]; ++i)
        {
            BYTE c;
            c = in.get();
            buckets[l].push_back(c);
            symb.push_back(c);
        }  
    return buckets;
}

void Huffman::encode(std::ifstream         &in,
       std::ofstream         &out,
       vector<vector<bool> > &table)
{
    BYTE count = 0; 
    BYTE buf   = 0;
    in.clear(); 
    in.seekg(0);

    while (!!in)
    { 
        BYTE c = in.get();
        vector<bool>::reverse_iterator rit;
        for(rit = table[c].rbegin(); rit != table[c].rend(); ++rit)
        {
            buf |=  (*rit) << (CHAR_BITS - 1 - count);
            ++count;   
            if (count == CHAR_BITS) // it's time to write 
            {
                count = 0;
                out.put(buf);
                buf = 0;
            } 
        }
    }
    if (count) // incomplete byte
        out.put(buf);
}

void Huffman::decode(std::ifstream &in, 
       std::ofstream &out, 
       Node const    *root, 
       Size_t         size)
{
    Node const *p = root;
    int count = 0; 
    Size_t filesize = 0;
    BYTE byte; 
    byte = in.get();
  
    while (!!in)
    {   
        if (filesize == size)
            break;

        bool b = byte & (1 << (CHAR_BITS - 1 - count)); 
        if (b)
            p = p->right; 
        else
            p = p->left;
      
        if (!p->left && !p->right) 
        {
            out.put(p->c); 
            ++filesize;
            p = root;
        }
        ++count;
        if (count == CHAR_BITS)
        {
            count = 0; 
            byte = in.get();
        }
    }
  
}
//End of private functions


//Begin of public functions
void Huffman::huffmanEncodeFile(std::ifstream &in, 
                  std::ofstream &out, 
                  Size_t        size)
{
    vector<Size_t> freq(MAX_SYMBOLS, 0);
    while (!!in)
    { 
        BYTE c = in.get();
        ++freq[c];
    }
    //Uncomment this line and write VERBOSE 1 instead of VERBOSE 0 in verbose.h file for debugging mode
    //printFrequences(freq);
  
    Node const *root = buildEncodeTree(freq);
    //Uncomment this line and write VERBOSE 1 instead of VERBOSE 0 in verbose.h file for debugging mode
    //printTree(root, 0);
 
    vector<BYTE> length(MAX_SYMBOLS, 0);
    assignLength(root, length, 0);
    //Uncomment this line and write VERBOSE 1 instead of VERBOSE 0 in verbose.h file for debugging mode 
    //printCodeLength(length);

 
    vector<BYTE> numberOfCodes(MAX_SYMBOLS, 0);  
    for (int c = 0; c != MAX_SYMBOLS; ++c)
        if (freq[c])
            ++numberOfCodes[length[c]];

    Size_t max_length = 0;
    for (int i = 0; i != MAX_SYMBOLS; ++i)
        if (numberOfCodes[i])
            max_length = i;
  
    vector<vector<BYTE> > buckets(max_length + 1,
				vector<BYTE>());
    //Sort chars by code length
    for (int i = 0; i != MAX_SYMBOLS; ++i)
        if (freq[i])
            buckets[length[i]].push_back(static_cast<BYTE>(i));
  
    //Sort each bucket by ascii code
    for (BYTE i = 1; i <= max_length; ++i)
        std::sort(buckets[i].begin(), buckets[i].end());
  
    vector<BYTE> startCode(MAX_SYMBOLS, 0);
    buildStartCode(max_length, numberOfCodes, startCode);
    //Uncomment this line and write VERBOSE 1 instead of VERBOSE 0 in verbose.h file for debugging mode
    //printStartCode(startCode, max_length); 

    vector<vector<bool> > table(MAX_SYMBOLS, vector<bool>());
    buildTable(max_length, 
	     numberOfCodes, 
	     startCode, 
	     buckets,
	     table);
    //Uncomment this line and write VERBOSE 1 instead of VERBOSE 0 in verbose.h file for debugging mode
    //printReversedCode(table);

    writeOverhead(max_length, numberOfCodes, buckets, out, size);
  
    encode(in, out, table);
    destroyTree(root);  
}

void Huffman::huffmanDecodeFile(std::ifstream &in, 
                  std::ofstream &out)
{
    vector<BYTE> numberOfCodes(MAX_SYMBOLS, 0);  
    BYTE max_length = 0; 
    vector<BYTE> symb;
    Size_t size = 0;
  
    vector<vector<BYTE> > buckets;
    buckets = readOverhead(in, max_length, numberOfCodes, symb, size);

    vector<BYTE> startCode(MAX_SYMBOLS, 0); 
    buildStartCode(max_length, numberOfCodes, startCode);
    //Uncomment this line and write VERBOSE 1 instead of VERBOSE 0 in verbose.h file for debugging mode
    //printStartCode(startCode, max_length);
  
    vector<vector<bool> > table(MAX_SYMBOLS, vector<bool>());  
    buildTable(max_length, 
	     numberOfCodes, 
	     startCode, 
	     buckets,
	     table);
    //Uncomment this line and write VERBOSE 1 instead of VERBOSE 0 in verbose.h file for debugging mode
    //printReversedCode(table);

    Node const *root = buildDecodeTree(symb, table);
    decode(in, out, root, size);
    destroyTree(root);
}
//End of public functions
