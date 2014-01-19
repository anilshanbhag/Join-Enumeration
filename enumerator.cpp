#include <cstdio>
#include <map>
#include <iostream>
#include <list>
#include <cassert>
#include <cstring>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

struct node;
struct eq_class;
eq_class* buildtree(string inp);

#define DEBUG true

struct eq_class {
    string relations;
    int num_relations;
    vector<node*> nodes;

    eq_class() {
        num_relations = 0;
    }
};

struct node {
    eq_class* left;
    eq_class* right;
    bool expanded;

    node() {
        expanded = false;
    }
};

map<int, eq_class*> memo;

int createHash(string p) {
    int tot = 0;
    int multiplier = 1;
    for(int i=0; i<p.size(); i++) {
        tot += (p[i] - 'a') * multiplier;
        multiplier *= 26;
    }
    return tot;
}

int leftAssoc(eq_class* ABC, eq_class* A, eq_class* BC) {
    // RS-B1
    if (DEBUG) cout<<"LeftAssoc "<<A->relations<<" "<<BC->relations<<endl;

    if (!(BC->num_relations == 1)) {
        // LA
        int k = BC->nodes.size();
        for (int j=0; j<k; j++) {
            node* q = BC->nodes[j];

            bool f1 = false, f2 = false;
            for(int i=0; i < ABC->nodes.size(); i++) {
                if (ABC->nodes[i]->left->relations == q->left->relations ||
                    ABC->nodes[i]->right->relations == q->left->relations) {
                    f1 = true;
                }
                if (ABC->nodes[i]->left->relations == q->right->relations ||
                    ABC->nodes[i]->right->relations == q->right->relations) {
                    f2 = true;
                }

                if (f1 && f2) break;
            }

            if(!f1) {
                eq_class* left = q->left;
                string relright = q->right->relations + A->relations;
                eq_class* right = buildtree(relright);

                node* option = new node();
                option->left = left;
                option->right = right;
                ABC->nodes.push_back(option);

                if (DEBUG) cout<<"New Node "<<left->relations<<" "<<right->relations<<" "<<relright<<endl;
            }

            if(!f2) {
                eq_class* left = q->right;
                string relright = q->left->relations + A->relations;
                eq_class* right = buildtree(relright);

                node* option = new node();
                option->left = left;
                option->right = right;
                ABC->nodes.push_back(option);

                if (DEBUG) cout<<"New Node "<<left->relations<<" "<<right->relations<<" "<<relright<<endl;
            }
        }
    }

    return 0;
}

int expandDAG(eq_class* eq) {
    if (DEBUG) cout<<"ExpandDAG "<<eq->relations<<endl;

    for(int i=0; i < eq->nodes.size(); i++) {
        node* p = eq->nodes[i];
        if (!p->expanded) {
            expandDAG(p->left);
            expandDAG(p->right);

            // RS-B1
            leftAssoc(eq, p->left, p->right);
            // after commutativity at node
            leftAssoc(eq, p->right, p->left);

            p->expanded = true;
        }
    }

    return 0;
}

bool exists(int hash) {
    return memo.find(hash) != memo.end();
}

eq_class* buildtree(string inp) {
    assert(inp.size() != 0);

    // Sort Input
    sort(inp.begin(), inp.end());

    int hash = createHash(inp);
    if (exists(hash)) {
        return memo[hash];
    } else {
        // inp eq class does not exist
        // if inp is base relation
        if(inp.size() == 1) {
            eq_class* base = new eq_class();
            base->relations = inp;
            base->num_relations = 1;
            memo[hash] = base;
            return base;
        } else {
            eq_class* left;
            string buf = inp.substr(0,1);
            int lefthash = createHash(buf);
            if (exists(lefthash)) {
                left = memo[lefthash];
            } else {
                left = new eq_class();
                left->num_relations = 1;
                left->relations = buf;
                memo[lefthash] = left;
            }

            eq_class* right = buildtree(inp.substr(1, inp.size() - 1));

            eq_class* base = new eq_class();
            base->num_relations = right->num_relations + left->num_relations;
            base->relations = inp;
            node* option = new node();
            option->left = left;
            option->right = right;

            base->nodes.push_back(option);
            memo[hash] = base;
            return base;
        }
    }
}

int main() {
    string input = "ABCDEF";
    string left = "AB";
    string right = "CDEF";

    /* Create AB ⋈ CDEF */
    printf("Building Left Tree\n");
    eq_class* left_e = buildtree(left);
    printf("Building Right Tree\n");
    eq_class* right_e = buildtree(right);

    node* base_node = new node();
    base_node->left = left_e;
    base_node->right = right_e;

    /* Create root eq node ABCDEF */
    eq_class* base = new eq_class();
    int hash = createHash(input);
    base->relations =  input;
    base->num_relations = 6;
    base->nodes.push_back(base_node);
    memo[hash] = base;

    /* Start Exploration from Base */
    printf("STARTING EXPLORATION\n");
    expandDAG(base);
    // exploreRSB2CF(base);
    // exploreRSB3CF(base);
    printf("EXPLORATION DONE\n");

    /* Print Memo Table Size */
    printf("MEMO SIZE %d\n", (int)memo.size());
    return 0;
}