#include <iostream>
#include <list>

// ===========================================================================

// A SIMPLE LINKED LIST CLASS
// (note: it's ok that all the member variables are public for this tiny class)

class Node {
public:
  int value;
  Node* ptr;
};


// ===========================================================================

// a helper function to print the contents of a linked list of Nodes
void print_linked_list(const std::string &name, Node *my_linked_list) {
  // print the name of this list
  std::cout << name;
  // use a temporary pointer to walk down the list
  Node *tmp = my_linked_list;
  while (tmp != NULL) {
    // print out each element
    std::cout << " " << tmp->value;
    tmp = tmp->ptr;
  }
  std::cout << std::endl;
}


// YOU NEED TO WRITE THIS FUNCTION
Node* make_linked_list_from_STL_list(const std::list<int> &lst)
{
	Node* head = new Node;
	Node* node = head; 
	std::list<int>::const_iterator it = lst.begin(); 

	while (it != lst.end()) {
		node->value = *it;
		node->ptr = new Node; 
		node = node->ptr; 
		it++; 
	}
	node->ptr = NULL; 

	return head; 
}


// YOU NEED TO WRITE THIS FUNCTION
Node* reverse_nodes_in_linked_list(Node* input)
{
	if (input == NULL || (input != NULL && input->ptr == NULL))
		return input; 

	Node* node = input->ptr; 
	Node* prev = input; 
	prev->ptr = NULL; 

	while (node != NULL) {
		Node* next = node->ptr; 
		node->ptr = prev;
		prev = node; 
		node = next; 
	} 
	return prev; 
}


// ===========================================================================

int main() {

  // manually create a linked list of notes with 4 elements
  Node* a = new Node; 
  a->value = 6; 
  a->ptr = new Node;
  a->ptr->value = 7;
  a->ptr->ptr = new Node;
  a->ptr->ptr->value = 8;
  a->ptr->ptr->ptr = new Node;
  a->ptr->ptr->ptr->value = 9;
  a->ptr->ptr->ptr->ptr = NULL;
  // print out this list
  print_linked_list("a",a);

  // create an STL list with 4 elements
  std::list<int> b;
  b.push_back(10);
  b.push_back(11);
  b.push_back(12);
  b.push_back(13);

  // use the STL list as input to a creator function that creates
  // linked lists with the same data
  Node* c = make_linked_list_from_STL_list(b);
  // print that data
  print_linked_list("c",c);

  // CUSTOM TEST CASE
  std::list<int> m; 
  Node* n = NULL; 

  for (int c = 0; c < 20; c++) {
    m.push_back(c*2); 
  }

  n = make_linked_list_from_STL_list(m); 
  print_linked_list("n", n); 


  // reverse a linked list of nodes
  Node* d = reverse_nodes_in_linked_list(c);
  // print this data
  print_linked_list("d",d);

  Node* p = reverse_nodes_in_linked_list(n); 
  print_linked_list("p", p); 
}

// ===========================================================================
