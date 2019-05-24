#include "Tree.h"


Tree::Tree()
{
	this->root = new Node('-',512,0,nullptr,true);
	this->nullNode = this->root;
}

void Tree::print()
{
	printUtil(this->root, 0);
}

void Tree::printUtil(Node * root, int space)
{
	if (root == nullptr) {
		return;
	}

	space += COUNT;

	printUtil(root->right, space);


	std::cout << std::endl;

	for (int i = COUNT; i < space; i++)
		std::cout << " ";
	root->print();
	std::cout << "\n";

	printUtil(root->left, space);

}

void Tree::swap(Node * n1, Node * n2)
{
	char c_temp = n1->c;

	n1->c = n2->c;
	n2->c = c_temp;

	int weight_temp = n1->weight;
	n1->weight = n2->weight;
	n2->weight=weight_temp;


	Node* left_temp = n1->left;
	n1->left = n2->left;
	n2->left = left_temp;

	Node* right_temp = n1->right;
	n1->right = n2->right;
	n2->right = right_temp;



	if (n1->left != nullptr) {
		n1->left->parent = n1;
	}
	if (n1->right != nullptr) {
		n1->right->parent = n1;
	}

	if (n2->left != nullptr) {
		n2->left->parent = n2;
	}
	if (n2->right != nullptr) {
		n2->right->parent = n2;
	}

}

bool Tree::getNode(Node* node,char c,Node* &p)
{
	if (node == nullptr) {
		return false;
	}
	
	if (node->isleaf()) {
		if (!node->nullNode) {
			if (node->c == c) {
				p = node;
				return true;
			}
		}
	}
	return (getNode(node->left, c, p) || getNode(node->right, c, p));
	
	
}

bool Tree::getCode(Node * node, char c, std::string s,std::string &out)
{
	if (node == nullptr) {
		return false;
	}

	if (node->isleaf()) {
		if (node->c == c) {
			if (!node->nullNode) {

				out = s;
				return true;
			}
		}
	}

	bool left=false, right=false;

	s.push_back('0');
	 left = getCode(node->left, c, s,out);
	s.pop_back();


	if (left) {
		return true;
	}

	s.push_back('1');
	 right = getCode(node->right, c, s,out);
	s.pop_back();

	if (right) {
		return true;
	}
	
	return false;
}

bool Tree::getNullNodeCode(Node * node, std::string s, std::string & out)
{
	if (node == nullptr) {
		return false;
	}

	if (node->isleaf()) {
		if (node->nullNode) {

			out = s;
			return true;
		}
	}

	bool left = false, right = false;

	s.push_back('0');
	left = getNullNodeCode(node->left, s, out);
	s.pop_back();


	if (left) {
		return true;
	}

	s.push_back('1');
	right = getNullNodeCode(node->right, s, out);
	s.pop_back();

	if (right) {
		return true;
	}

	return false;

}

void Tree::getMaxNode(Node* node,int weight, int numOrder, Node*  &max)
{
	if (node == nullptr) {
		return;
	}

	if(node!=root){

		if (node->weight == weight) {

			if (max == nullptr) {
				if (node->order > numOrder) {
					max = node;
				
				}

			}
			else {


				if (node->order > max->order) {
					
					max = node;
				}
			}
			
		}

	}
	getMaxNode(node->left, weight, numOrder, max);

	getMaxNode(node->right, weight, numOrder, max);

	
}

void Tree::updateTree(char value)
{
	Node* NodeValue=nullptr;
	bool found =  getNode(this->root,value, NodeValue);



	Node* nodeChain;


	if (found) {

		nodeChain = NodeValue;
	}
	else{

		Node* oldnullNode = this->nullNode;
		Node* left = new Node('-', (this->nullNode->order - 2), 0, oldnullNode, true);
		Node* right = new Node(value, (this->nullNode->order - 1), 1, oldnullNode, false);

		left->nullNode = true;
		right->nullNode = false;
		oldnullNode->nullNode = false;

		oldnullNode->left = left;
		oldnullNode->right = right;

		this->nullNode = left;

		nodeChain = oldnullNode;
	}


	Node *max = nullptr;
	
	
	while (nodeChain!=nullptr) {

		getMaxNode(this->root, nodeChain->weight, nodeChain->order, max);

		if (max == nullptr || max== nodeChain->parent) {

			nodeChain->update();
			nodeChain = nodeChain->parent;

		}
		else {

			
			swap(max, nodeChain);
			max->update();
			nodeChain = max->parent;
		}

		max = nullptr;
	}

}
