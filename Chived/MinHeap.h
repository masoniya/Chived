#pragma once

#include <map>
#include <iostream>


struct HeapNode {
	unsigned freq;

	char c;

	HeapNode *left, *right;

	void print() {
		
		std::cout << freq << " ";
	}

	bool isLeaf() {
		return !(left) && !(right);
	}
};

class MinHeap
{
public:
	MinHeap(std::map<char, int> freq);

	void swap_nodes(int i, int j);
	HeapNode* extractMin();
	void buildMinHeap();
	bool isEmpty();
	void insertNode(HeapNode* node);
	struct HeapNode* createNewNode(unsigned freq,char c);
	HeapNode* buildTree();

private:

	void init(std::map<char, int> freq);
	void swapNodes(struct HeapNode* &n1, struct HeapNode* &n2);
	void minHeapify(int i);
	bool isSizeone();
	
	unsigned size; 	//current size

	HeapNode **arr;
};
