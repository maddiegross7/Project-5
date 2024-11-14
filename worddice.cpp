//Maddie Gross
//Lab 5: Worddice Lab
//Uses Endmonds-Karp algorithm to find if a word can be spelled using a specified number of dice
#include <iostream>
#include <vector>
#include <fstream>
#include <queue>

using namespace std;

enum node_type { src, sink, dice, word };

//what connects the nodes
class Edge {
public:
    class Node *to;
    class Node *from;
    Edge *reverse;
    int original;
    int residual;
    int index;
};

//sink, src, dice, letters in each word
class Node {
    public:
        int id;
        node_type type;
        vector<bool> letters;
        int visited;
        vector<Edge*> adj;
        Edge *backEdge;
        Node();   
};

Node::Node(){
    letters.assign(26, false); // 26 spots for each letter of the alphabet
    visited = 0;
    backEdge = nullptr;
    id = NULL;
}

//holds all of them
class Graph {
    public:
        vector<Node*> nodes;
        vector<int> spellingIds;
        int minNodes=0;
        int BFS();
        int canISpell();
        void deleteHalfGraph();
        void cleanupGraph();
};

//Uses breadth first search to explore various routes in order to get to the sink
int Graph::BFS() {
    queue<Node*> q;
    Node *ourSource = nodes[0];

    for (Node* node : nodes) {
        node->visited = 0;
        node->backEdge = nullptr;
    }

    ourSource->visited = 1;
    q.push(ourSource);

    while (!q.empty()) {
        Node *current = q.front();
        q.pop();

        for (Edge* edge : current->adj) {
            Node *next = edge->to;
            if (edge->from == current) {
                if (!next->visited && (edge->original - edge->residual) > 0) {
                    next->visited = 1;
                    next->backEdge = edge;
                    q.push(next);

                    if (next->type == sink) {
                        return 1;  
                    }
                }
            } else {
                //this is to explore reverse edges if we may need to change dice from orginal flow
                Edge* reverseEdge = edge; 
                if (reverseEdge->reverse) {
                    Node *previous = reverseEdge->from;

                    if (!previous->visited && reverseEdge->residual > 0) {
                        previous->visited = 1;
                        previous->backEdge = reverseEdge; 
                        q.push(previous);

                        if (previous->type == sink) {
                            return 1;  
                        }
                    }
                }
            }
        }
    }

    return 0; 
}

//calls BFS multiple times and adjusts the flows of the paths that are found each iteration
int Graph::canISpell(){
    while(BFS() == 1){
        Node *sink = nodes[nodes.size()-1];
        if (!sink->backEdge) {
            cerr << "Backedge for sink is null. Path trace may be invalid." << endl;
        } else {
            while (sink->type != src && sink->backEdge != nullptr) {

                sink->backEdge->original = 0;
                sink->backEdge->residual = 1;
                if (sink->backEdge->reverse) {
                    sink->backEdge->reverse->original = 1;
                    sink->backEdge->reverse->residual = 0;
                }
                sink = sink->backEdge->from;
            }
        }
    }

    Node *sink = nodes[nodes.size()-1];
    int canSpell = 1;

    for(int i = 0; i < sink->adj.size(); i++){
        if(sink->adj[i]->original == 0){
            canSpell = 0;
        }
    }

    return canSpell;
}

//deletes the sink and word nodes and all edges that are connected to or from them
void Graph::deleteHalfGraph() {
    for (Node *node : nodes) {
        if (node->type == dice) {
            for (auto it = node->adj.begin(); it != node->adj.end(); ) {
                Edge *edge = *it;
                if (edge->to->type == word) {
                    delete edge;
                    it = node->adj.erase(it);
                } else {
                    ++it; 
                }
            }
        }
    }

    for (int i = nodes.size() - 1; i >= minNodes; i--) {
        Node *node = nodes[i];
        
        for (Edge *edge : node->adj) {
            if (edge->reverse) {
                edge->reverse->reverse = nullptr;
            }
            delete edge;
        }
        
        delete node; 
        nodes.erase(nodes.begin() + i); 
    }

    for (Edge *edge : nodes[0]->adj) {
        edge->original = 1;
        edge->residual = 0;
    }

    for (Node *node : nodes) {
        node->visited = 0;
        node->backEdge = nullptr;
    }
}

//deletes the whole graph, edges and nodes all of them
void Graph::cleanupGraph() {
    // Delete all edges and nodes in the graph
    for (Node* node : nodes) {
        for (Edge* edge : node->adj) {
            // Only delete the edge if edge exists
            if (edge) {
                delete edge;
            }
        }
        // Delete the node
        delete node;
    }

    // Clear the whole nodes vector 
    nodes.clear();
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <diceFile> <wordFile>" << endl;
        return 1;
    }

    string diceFile = argv[1];
    string wordFile = argv[2];
    int nodeIndex=0;

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
    //this should create a node for each dice so this is each line of the dice file
    while (getline(fin, line)) {
        nodeIndex++;
        Node *ourDice = new Node;
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

        //creating reverse edges 
        Edge *toSource = new Edge;
        toSource->from = ourDice;
        toSource->to = source;
        toSource->original = 0;
        toSource->residual = 1;

        fromSource->reverse = toSource;
        toSource->reverse = fromSource;

        ourDice->backEdge = fromSource; 
        source->adj.push_back(fromSource);
        ourDice->adj.push_back(toSource);
        ourGraph.nodes.push_back(ourDice);
        ourGraph.minNodes++;
    }

    fin.close();
    fin.open(wordFile);

    if(!fin.is_open()){
		cerr << "Could not open: " << wordFile << endl;
	}
    int wordCount = 0;
    //this will create a node for each letter in each word that is given so 
    //multiple nodes per line that is read
    while(getline(fin, line)){
        nodeIndex = ourGraph.minNodes;
        Node *ourSink = new Node();
        ourSink->type = sink;

        for(int i = 0; i < line.length(); i++){
            Node *aLetter = new Node();
            aLetter->id = nodeIndex;
            aLetter->type = word;
            char letter = line[i];
            int diff = letter - 'A';
            aLetter->letters[diff]=true;
            nodeIndex++;

            for(int j = 1; j < ourGraph.minNodes; j++){
                if (ourGraph.nodes[j]->letters[diff] == true) {
                    // Create edges for words
                    Edge *toWord = new Edge;
                    toWord->to = aLetter;
                    toWord->from = ourGraph.nodes[j];
                    toWord->residual = 0;
                    toWord->original = 1;

                    Edge *toDice = new Edge;
                    toDice->from = aLetter;
                    toDice->to = ourGraph.nodes[j];
                    toDice->original = 0;
                    toDice->residual = 1;

                    toWord->reverse = toDice;
                    toDice->reverse = toWord;

                    // Add edges to adjacency list
                    ourGraph.nodes[j]->adj.push_back(toWord);
                    aLetter->adj.push_back(toDice);
                }
            }
            Edge *toSink = new Edge;
            toSink->from = aLetter;
            toSink->to = ourSink;
            toSink->original = 1;
            toSink->residual = 0;

            Edge *fromSink = new Edge;
            fromSink->from = ourSink;
            fromSink->to = aLetter;
            fromSink->original = 0;
            fromSink->residual = 1;

            toSink->reverse = fromSink;
            fromSink->reverse = toSink;

            aLetter->adj.push_back(toSink);
            ourSink->adj.push_back(fromSink);

            ourGraph.nodes.push_back(aLetter);
        }
        ourSink->id = nodeIndex;
        ourGraph.nodes.push_back(ourSink);

        // if(wordCount > 0){
        //     cout << endl;
        // }
        wordCount++;
        //if the word can be spelled printing out the order of the dice 
        //that is used for each letter in the words
        if(ourGraph.canISpell() == 1){
            for(int i = ourGraph.minNodes; i < ourGraph.nodes.size()-1; i++){
                if(i > ourGraph.minNodes){
                        cout << ",";
                }
                for(int j = 0; j < ourGraph.nodes[i]->adj.size(); j++){
                    if(ourGraph.nodes[i]->adj[j]->to->type == dice && ourGraph.nodes[i]->adj[j]->original == 1){
                        cout << (ourGraph.nodes[i]->adj[j]->to->id - 1);
                    }
                }
            }
            cout << ": " << line << endl;;
        }else{
            cout << "Cannot spell "  << line << endl;
        }

        //after each word has been determined we will delete the part 
        //of the graph that corresponds to that word
        ourGraph.deleteHalfGraph();

    }

    //delete whole graph no memory leaks please
    ourGraph.cleanupGraph();
    return 0;
}
