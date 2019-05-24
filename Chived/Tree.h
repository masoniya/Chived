#pragma once

#include <iostream>
#include <bitset>
#include <sstream>

#define COUNT 10


static void asciToBits( char c , std::string &out)
{
	char b = c;
	char one = 1;
	std::string s = "";

	for ( int i = 0; i < 8; i++) {

		( (int)(b & one) == 1) ? s += '1' : s += '0';
		b = b >> 1;

	}

	for (int i = 7; i >= 0; i--) {
		out += s[i];

	}

}

static void bitstoAsci(std::string s, char &out)
{
		std::stringstream sstream(s);

		std::bitset<8> bits;
		sstream >> bits;
		out = char(bits.to_ulong());
	
}

class Node {

public:
	char c;

	int order;
	int weight;

	Node* left;
	Node* right;
	Node* parent;

	bool nullNode;

	Node(char c,int order,int weight,Node* parent, bool isnullNode) {
		this->c = c;
		this->order = order;
		this->weight = weight;

		this->left = nullptr;
		this->right = nullptr;
		this->parent = parent;

		this->nullNode = isnullNode;
		(nullNode == true) ? "Yes" : "No";
	}

	void print() {
		std::cout << "(" << this->c << "," << this->order << ","<<this->weight <<"," << (nullNode == true ? "Yes" : "No") <<")";
	}

	bool isleaf() {
		return ((this->left == nullptr) && (this->right == nullptr));
	}
	void update() {
		if (isleaf()) {

			this->weight++;
		}
		else {

			int value1 = this->left->weight;
			int value2 = this->right->weight;


			this->weight = value1 + value2;

		}
	}

};


class Tree
{
public:

	Tree();
	void print();

	Node* root;
	Node* nullNode;

	void swap(Node* n1, Node* n2);
	bool getNode(Node* node,char c,Node* &p);
	void getMaxNode(Node* node,int weight, int numOrder, Node* &max);
	void updateTree(char value);

	bool getCode(Node* node, char c, std::string s,std::string &out);
	bool getNullNodeCode(Node* node, std::string s, std::string &out);

private:
	
	void printUtil(Node* root , int space);
	

};
