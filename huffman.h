#pragma once
#include "node.h"
#include <fstream>


class Huffman {
private:
	void readBinary(std::ifstream &in, Size_t &num);

	void writeBinary(std::ofstream &out, Size_t num);

	//writes length of node 
	void assignLength(Node const *root, vector<BYTE> &length, BYTE level);

	void buildTable(BYTE max_length, vector<BYTE> &numberOfCodes, vector<BYTE> &startCode,
		vector<vector<BYTE> > &buckets, vector<vector<bool> > &table);

	void buildStartCode(BYTE max_length, vector<BYTE> &numberOfCodes, vector<BYTE> &startCode);

	//Build tree for encoding
	Node const* buildEncodeTree(vector<Size_t> &freq);

	//Build tree for decoding
	Node const* buildDecodeTree(vector<BYTE> &symb, vector<vector<bool> > &table);

	void destroyTree(Node const *root);

	void writeOverhead(BYTE max_length, vector<BYTE> &numberOfCodes, vector<vector<BYTE> > &buckets, 
		std::ofstream &out, Size_t size);

	vector<vector<BYTE> > readOverhead(std::ifstream &in, BYTE &max_length, vector<BYTE> &numberOfCodes,
		vector<BYTE> &symb, Size_t &size);

	void encode(std::ifstream &in, std::ofstream &out,
		vector<vector<bool> > &table);


	void decode(std::ifstream &in, std::ofstream &out, Node const *root, 
    	Size_t size);


public:
	void huffmanEncodeFile(std::ifstream& in, std::ofstream& out, Size_t size);
	void huffmanDecodeFile(std::ifstream& in, std::ofstream& out);
};