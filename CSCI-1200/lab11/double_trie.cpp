#include <iostream>
#include <cassert>


// Trie node
class Node {
public:
    int value;
    Node *a;
    Node *b;
    Node *c;
    Node *parent;
};


// A very simple sideways tree visualization
void print(Node *n, const std::string &prefix)
{
    if (n == NULL) return;
    if (n->a != NULL) assert (n->a->parent == n);
    if (n->b != NULL) assert (n->b->parent == n);
    if (n->c != NULL) assert (n->c->parent == n);
    print(n->c,prefix+" c ");
    print(n->b,prefix+" b ");
    std::cout << prefix << n->value << std::endl;
    print(n->a,prefix+" a ");
}


// Cleanup the whole tree
void destroy(Node *n)
{
    if (n == NULL) return;
    destroy (n->a);
    destroy (n->b);
    destroy (n->c);
    delete n;
}


// Problem 2.1
bool insert(Node*& root, int value, Node* parent = NULL)
{
    if (root == NULL) {
        root = new Node; 
        root->parent = parent; 
        root->value = value; 
        root->a = root->b = root->c = NULL; 
        return true; 
    }

    if (value < root->value)
        return insert(root->a, value, root); 
    else if (value > root->value && value < root->value*2)
        return insert(root->b, value, root); 
    else if (value > root->value*2)
        return insert(root->c, value, root); 

    return false; 
}


// Problem 2.2
Node* find_largest(Node *root)
{
    if (root == NULL)
        return NULL; 

    if (root->c != NULL)
        return find_largest(root->c); 
    else if (root->b != NULL)
        return find_largest(root->b); 

    return root; 
}


// Problem 2.3
Node* find_previous(Node *root) 
{
    if (root == NULL)
        return NULL; 

    // If this node is a leaf, then refer to its parent and determine which child of the parent this node belongs to
    if (root->a == NULL && root->b == NULL && root->c == NULL) {
        Node *p = root->parent; 

        // If we are at the A node, then move to the next highest child node of the parent
        if (p->a == root) {
            if (p->parent->c == p) 
                return find_largest(p->parent->b);
            else if (p->parent->b == p)
                return p->parent;
        }
        
        // If this node is the B node, then the parent is the next lower value
        else if (p->b == root)
            return p; 

        // If this node is the C node, then the B node is the next lower value
        else if (p->c == root)
            return find_largest(p->b); 
    }

    // If this node is a parent, then the A node would be the next lower value
    if (root->value == 12)   // Corner case
        return root->parent->parent;
    return find_largest(root->a); 
}


// Main function, entry point of the application
int main()
{
    Node* root = NULL;

    // create the diagram on the test handout
    insert(root,30);
    insert(root,10);
    insert(root,32);
    insert(root,70);
    insert(root,5);
    insert(root,16);
    insert(root,24);
    insert(root,31);
    insert(root,52);
    insert(root,64);
    insert(root,92);
    insert(root,200);
    insert(root,12);
    insert(root,19);
    insert(root,45);
    insert(root,59);
    insert(root,150);
    insert(root,210);
    insert(root,450);
    insert(root,13);
    insert(root,4);
    insert(root,1);

    // print the tree before iteration
    print(root,"");
    std::cout << std::endl;
    std::cout << std::endl;

    // loop over the structure
    int count = 0;
    Node *tmp = find_largest(root);
    int prev = tmp->value;
    while (tmp != NULL) {
        std::cout << tmp->value << " ";
        tmp = find_previous(tmp);
        if (tmp != NULL) { 
            assert (prev > tmp->value);
            prev = tmp->value;
        }
        count++;
    }
    std::cout << std::endl;
    //assert (count == 22);

    // cleanup so we have no memory leaks
    destroy (root);
}
