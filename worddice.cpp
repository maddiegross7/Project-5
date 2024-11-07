#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

enum node_type { src, sink, dice, word };

class Edge {
public:
    class Node *to;
    class Node *from;
    Edge *reverse;
    int original;
    int residual;
    int index;
};

class Node {
public:
    int id;
    node_type type;
    vector<bool> letters;
    int visited;
    vector<Edge*> adj;
    Edge *backEdge;

    // Constructor
    Node() 
        :
          letters(26, false),  // 26 spots for each letter of the alphabet
          visited(0)  {}
};

class Graph {
public:
    vector<Node*> nodes;
    vector<int> spellingIds;
    int minNodes=0;

    int BFS();
    int canISpell();
    void deleteHalfGraph();
};

int main(int argc, char *argv[]) {

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <diceFile> <wordFile>" << endl;
        return 1;
    }

    string diceFile = argv[1];
    string wordFile = argv[2];
    int nodeIndex=0;

    cout << "dice: " << diceFile << endl;
    cout << "words: " << wordFile << endl;

    Graph ourGraph;
    Node *source = new Node;
    
    source->id = 0;
    source->type = src;

    ourGraph.nodes.push_back(source);
    ourGraph.minNodes++;

    ifstream fin(diceFile);

    if(!fin.is_open()){
		cerr << "Could not open: " << diceFile << endl;
	}

    string line;
    while (getline(fin, line)) {
        nodeIndex++;
        cout << line << endl;
        Node *ourDice = new Node;;
        ourDice->id = nodeIndex;
        ourDice->type = dice;
        for(int i=0; i < line.length(); i++){
            char letter = line[i];
            int diff = letter - 'A';
            ourDice->letters[diff]=true;
        }
        Edge *fromSource = new Edge;
        fromSource->from = source;
        fromSource->to = ourDice;
        fromSource->original = 1;
        fromSource->residual = 0;
        ourDice->backEdge = fromSource; //is this going the right direction? Do we need this yet? do we want this yet?
        source->adj.push_back(fromSource);
        ourGraph.nodes.push_back(ourDice);
        ourGraph.minNodes++;
    }

    cout << ourGraph.nodes.size() << endl;
    cout << "count of nodes" << ourGraph.minNodes << endl;

    fin.close();

    fin.open(wordFile);

    if(!fin.is_open()){
		cerr << "Could not open: " << wordFile << endl;
	}

    while(getline(fin, line)){
        Node *ourSink = new Node();
        ourSink->type = sink;
        for(int i = 0; i < line.length(); i++){
            nodeIndex++;
            cout << line[i] << endl;
            Node *aLetter = new Node();
            aLetter->id = nodeIndex;
            aLetter->type = word;
            char letter = line[i];
            int diff = letter - 'A';
            aLetter->letters[diff]=true;

            Edge *toSink = new Edge;
            toSink->from = aLetter;
            toSink->to = ourSink;
            toSink->original = 1;
            toSink->residual = 0;

            aLetter->adj.push_back(toSink);

            ourGraph.nodes.push_back(aLetter);
        }
        ourSink->id = nodeIndex + 1;
        ourGraph.nodes.push_back(ourSink);

        //Now I am gonna print out the graph to make sure it looks right

        for(int i = 0; i < ourGraph.nodes.size(); i++){
            cout << "Node " << i << " type: " << ourGraph.nodes[i]->type << " Edges to " ;
            for(int j = 0; j < ourGraph.nodes[i]->adj.size(); j++){
                cout << ourGraph.nodes[i]->adj[j]->to->id << " " ;
            }
            cout << endl;
        }

        //need to delete now the letters and the sink at the end of this loop every time
        //we can use the function in the class if we want but idk it we want to; we might want to use at the 
        //end to delete all of it but can play around with it and see
    }

    return 0;
}
