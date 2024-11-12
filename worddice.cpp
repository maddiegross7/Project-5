#include <iostream>
#include <vector>
#include <fstream>
#include <queue>

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
          visited(0),
          backEdge(nullptr)  {}
};

class Graph {
public:
    vector<Node*> nodes;
    vector<int> spellingIds;
    int minNodes=0;
    
    //okay gotta make this work all the way and not just till 
    //we find a route
    int BFS(){
        queue<Node*> q;
        Node* ourSource = nodes[0];
        ourSource->visited = 1;

        q.push(ourSource);

        while (!q.empty()){
            Node* current = q.front();
            q.pop();

            for(int i = 0; i < current->adj.size(); i++){
                Node* next = current->adj[i]->to;

                //let it work if someday the flow is not just 1
                if(!next->visited && (current->adj[i]->original - current->adj[i]->residual) > 0){
                    next->visited = 1;
                    next->backEdge = current->adj[i];
                    current->adj[i]->original = 0;
                    current->adj[i]->residual = 1;
                    q.push(next);
                    cout << current->id << " to " << next->id << endl;

                    // if(next->type == sink){
                    //     return 1;
                    // }
                }
            }
        }
        return 0;
        
    }

    int canISpell();

    void deleteHalfGraph() {
        //delete word nodes
        for (int i = nodes.size() - 1; i >= minNodes; i--) {

            for (Edge* edge : nodes[i]->adj) {
                delete edge;
            }
            nodes[i]->adj.clear();

            if (nodes[i]->backEdge != nullptr) {
                nodes[i]->backEdge = nullptr;
            }

            delete nodes[i];
            nodes[i] = nullptr;

            nodes.erase(nodes.begin() + i);
        }

        //delete edges from dice nodes
        for(int i = 0; i < nodes.size(); i++){
            if(nodes[i]->type == dice){
                for (Edge* edge : nodes[i]->adj) {
                    delete edge;
                }   
                nodes[i]->adj.clear();
            }
        }

        for(int i = 0; i < nodes.size(); i++){
            nodes[i]->backEdge = nullptr;
            nodes[i]->visited = 0;
        }

        for(int i = 0; i < nodes[0]->adj.size(); i++){
            nodes[0]->adj[i]->original = 1;
            nodes[0]->adj[i]->residual = 0;
            //nodes[0]->adj[i].
        }
    }
};




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
        ourDice->backEdge = fromSource; //is this going the right direction? Do we need this yet? do we want this yet?
        source->adj.push_back(fromSource);
        ourGraph.nodes.push_back(ourDice);
        ourGraph.minNodes++;
    }

    fin.close();

    fin.open(wordFile);

    if(!fin.is_open()){
		cerr << "Could not open: " << wordFile << endl;
	}
    
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
                if(ourGraph.nodes[j]->letters[diff] == true){
                    Edge *toWord = new Edge;
                    toWord->to = aLetter;
                    toWord->from = ourGraph.nodes[j-1];
                    toWord->residual = 0;
                    toWord->original = 1;
                    ourGraph.nodes[j]->adj.push_back(toWord);
                }
            }

            Edge *toSink = new Edge;
            toSink->from = aLetter;
            toSink->to = ourSink;
            toSink->original = 1;
            toSink->residual = 0;

            aLetter->adj.push_back(toSink);

            ourGraph.nodes.push_back(aLetter);
        }
        ourSink->id = nodeIndex;
        ourGraph.nodes.push_back(ourSink);

        //Now I am gonna print out the graph to make sure it looks right

        for(int i = 0; i < ourGraph.nodes.size(); i++){
            cout << "Node " << i << " ";
            for(int j = 0; j < ourGraph.nodes[i]->letters.size(); j++){
                if(ourGraph.nodes[i]->letters[j]){
                    char letter = 'A' + j;
                    cout << letter ;
                }
            }
            cout << " Edges to " ;
            for(int j = 0; j < ourGraph.nodes[i]->adj.size(); j++){
                cout << ourGraph.nodes[i]->adj[j]->to->id << " " ;
            }
            cout << endl ;
        }
        cout << endl << endl;

        //This is where we should do the BFS and path finding

        while(ourGraph.BFS() == 1){
            cout << "path found" << endl;
            
        }     


    
        ourGraph.deleteHalfGraph();

    }

    return 0;
}
