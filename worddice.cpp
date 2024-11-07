#include <iostream>
#include <vector>


using namespace std;

enum node_type{src, sink, dice, word};

class Edge {
    class Node *to;
    class Node *from;
    Edge *reverse;
    int original = 1;
    int residual = 0;
    int index;
};

class Node {
    int id;
    node_type type;
    vector<bool> letters;
    Node();
    int visited;
    vector<Edge*> adj;
    Edge *backEdge
};

class Graph {
    vector<Node*> nodes;
    int BFS();
    int canISpell();
    vector<int> spellingIds;
    void deleteHaldGraph();\
    int minNodes;
}

int main(int argc, char *argv[]) {
    
    string diceFile, wordFile;

    diceFile = argv[1];
    wordFile = argv[2];

    cout << "dice " <<  diceFile << endl;
    cout << "words " << wordFile << endl;

    return 0;
}
