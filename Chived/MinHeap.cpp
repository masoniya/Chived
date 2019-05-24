#include <cstdlib>

#include "MinHeap.h"


MinHeap::MinHeap( std::map<char, int> freq)
{
	init(freq);
}

void MinHeap::init(std::map<char , int> freq)
{
	this->size = (unsigned int)freq.size();

	arr = (HeapNode**)malloc(sizeof(struct HeapNode*)*this->size);

	int i = 0;
	for (auto itr = freq.begin(); itr != freq.end(); ++itr) {
		arr[i] = createNewNode(itr->second, itr->first);
		i++;
	}

	buildMinHeap();
}

void MinHeap::swap_nodes(int i, int j)
{
	swapNodes(arr[i], arr[j]);
}

HeapNode * MinHeap::extractMin()
{
	HeapNode* temp = arr[0];
	arr[0] = arr[size - 1];
	--this->size;
	minHeapify(0);

	return temp;
}

void MinHeap::buildMinHeap()
{
	int n = this->size - 1;
	int i;

	for (i = (n - 1) / 2; i >= 0; --i) {
		minHeapify(i);
	}
}

bool MinHeap::isEmpty()
{
	return size == 0;
}

void MinHeap::insertNode(HeapNode * node)
{
	++this->size;
	int i = this->size - 1;

	while (i && node->freq < arr[(i - 1) / 2]->freq) {
		arr[i] = arr[(i - 1) / 2];
		i = (i - 1) / 2;

	}
	arr[i] = node;
}

HeapNode * MinHeap::createNewNode(unsigned freq,char c)
{
	HeapNode* node = (HeapNode*)malloc(sizeof(HeapNode));

	node->freq = freq;
	node->left = nullptr;
	node->right = nullptr;
	node->c = c;

	return node;
}

HeapNode * MinHeap::buildTree()
{
	while (!isSizeone()) {
		HeapNode* n1 = extractMin();
		HeapNode* n2 = extractMin();

		HeapNode* parent = createNewNode(n1->freq + n2->freq, '#');
		parent->left = n1;
		parent->right = n2;

		insertNode(parent);
	}

	return extractMin();
}

void MinHeap::swapNodes(HeapNode* &n1, HeapNode* &n2)
{
	struct HeapNode* temp = n1;
	n1 = n2;
	n2 = temp;
}

void MinHeap::minHeapify(int i)
{
	int smallest = i;

	int left = 2 * i + 1;
	int right = 2 * i + 2;

	if (left < size && arr[left]->freq < arr[smallest]->freq) {
		smallest = left;
	}

	if (right < size && arr[right]->freq < arr[smallest]->freq) {
		smallest = right;
	}

	if (smallest != i) {
		swapNodes(arr[i], arr[smallest]);
		minHeapify(smallest);
	}
}

bool MinHeap::isSizeone()
{
	return (this->size == 1);
}
