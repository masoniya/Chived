#include "AdaHuffman.h"


AdaHuffman::AdaHuffman()
{
}

void AdaHuffman::compress(std::string in, std::string & out)
{
	Tree * tree = new Tree();
	Node* temp;
	std::string code="";
	std::string tempString = "";

	std::string asci = "";
	for (int i = 0; i < in.size(); i++) {

		

		if (tree->getNode(tree->root, in[i], temp)) {

			tree->getCode(tree->root, in[i], tempString, code);
			out += code;
			tree->updateTree(in[i]);
		}
		else {

			tree->getNullNodeCode(tree->root, tempString, code);
			asciToBits(in[i], asci);
			out += code;
			out += asci;
			tree->updateTree(in[i]);
		}

		


		code = "";
		tempString = "";
		asci = "";
	}

	//tree->print();
	
}

void AdaHuffman::decompress(std::string in, std::string & out)
{
	Tree * tree = new Tree();

	int i = 0;
	Node* node = tree->root;
	char l;
	while (i <= in.size()) {
		if (node->isleaf()) {

			if (node->nullNode) {
				bitstoAsci(in.substr(i, i + 8), l);
				out += l;

				tree->updateTree(l);

				i += 8;
			}
			else {

				out += node->c;
				tree->updateTree(node->c);
			}

			
			node = tree->root;
			//tree->print();



		}
		else {

			char c = in[i];
			node = c == '0' ? node->left : node->right;
			i++;
		}


		
	}

	
}
