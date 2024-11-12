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
        Node();   
};

Node::Node(){
    letters.assign(26, false); // 26 spots for each letter of the alphabet
    visited = 0;
    backEdge = nullptr;
    id = NULL;
}

class Graph {
    public:
        vector<Node*> nodes;
        vector<int> spellingIds;
        int minNodes=0;
        int BFS();
        int canISpell();
        void deleteHalfGraph();
};

//okay gotta make this work all the way and not just till 
//we find a route
int Graph::BFS(){
    queue<Node*> q;
    Node* ourSource = nodes[0];
    cout << "we in the BFS" << endl;
    for(int i = 0; i < nodes.size(); i++){
        nodes[i]->visited = 0;
        nodes[i]->backEdge = nullptr;
    }
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
                
                q.push(next);
                

                if(next->type == sink){
                    return 1;
                }
            }//else if the reverse has some kind of original flow then add that to the q maybe?

        }
    }
    return 0;
}

int Graph::canISpell(){
    cout << "we in Can I Spell" << endl;
    while(BFS() == 1){
        //cout << "nodes length " << nodes.size() << " " << endl;
        Node* sink = nodes.back();
        cout << "BFS = 1" << endl;
        
        cout << sink->type;
        while(sink->type != src){
            sink->backEdge->original = 0;
            sink->backEdge->residual = 1;
            sink->backEdge->reverse->original = 1;
            sink->backEdge->reverse->residual = 0;
            sink = sink->backEdge->from;
            cout << " to " << sink->id;
        }
        cout <<  endl;
    }
}

void Graph::deleteHalfGraph() {
    //delete word nodes
    cout << "hello? " << endl;
    for (int i = nodes.size() - 1; i >= minNodes; i--) {
        Node* node = nodes[i];
        for (Edge* edge : node->adj) {
            //delete edge->reverse;
            edge->reverse=nullptr;
            delete edge;
        }
        delete node;
        nodes.erase(nodes.begin() + i);
    }
    cout << "deleting word nodes good" << endl;

    //delete edges from dice nodes
    for(int i = 0; i < nodes.size(); i++){
        if(nodes[i]->type == dice){
            for (Edge* edge : nodes[i]->adj) {
                if(edge->to->type = word){
                    delete edge;
                }
            }   
            nodes[i]->adj.clear();
        }
    }

    cout << "deleting edges from dice nodes" << endl;

    for(int i = 0; i < nodes.size(); i++){
        nodes[i]->backEdge = nullptr;
        nodes[i]->visited = 0;
    }

    cout << "hmm is this the problem" << endl;

    for(int i = 0; i < nodes[0]->adj.size(); i++){
        nodes[0]->adj[i]->original = 1;
        nodes[0]->adj[i]->residual = 0;
    }

    cout << "or this? " << endl;
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

        Edge *toSource = new Edge;
        toSource->from = ourDice;
        toSource->to = source;
        toSource->original = 0;
        toSource->residual = 1;

        fromSource->reverse = toSource;

        ourDice->backEdge = fromSource; //is this going the right direction? Do we need this yet? do we want this yet?
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
    
    while(getline(fin, line)){
        cout << "okay so the word file is open" << endl;
        nodeIndex = ourGraph.minNodes;
        Node *ourSink = new Node();
        ourSink->type = sink;

        for(int i = 0; i < ourGraph.nodes.size(); i++){
            cout << "Node " << ourGraph.nodes[i]->id << " ";
            for(int j = 0; j < ourGraph.nodes[i]->letters.size(); j++){
                if(ourGraph.nodes[i]->letters[j]){
                    char letter = 'A' + j;
                    cout << letter ;
                }
            }
            cout << " Edges to " ;
            for(int j = 0; j < ourGraph.nodes[i]->adj.size(); j++){
                cout << ourGraph.nodes[i]->adj[j]->to->id << " "  ;
            }
            cout << endl ;
        }
        cout << endl << endl;
        for(int i = 0; i < line.length(); i++){
            cout << "creating nodes for letters" << endl;
            Node* aLetter = new Node();
            cout << "tell me its this stupud node index" << endl;
            aLetter->id = nodeIndex;
            cout << "hmm here?" << endl;
            aLetter->type = word;
            char letter = line[i];
            int diff = letter - 'A';
            cout << "or here? " << endl;
            aLetter->letters[diff]=true;
            nodeIndex++;

            for(int j = 1; j < ourGraph.minNodes; j++){
                if(ourGraph.nodes[j]->letters[diff] == true){
                    cout << "creating edges for words" << endl;
                    Edge *toWord = new Edge;
                    toWord->to = aLetter;
                    toWord->from = ourGraph.nodes[j-1];
                    toWord->residual = 0;
                    toWord->original = 1;

                    Edge *toDice = new Edge;
                    toDice->from = aLetter;
                    toDice->to = ourGraph.nodes[j];
                    toDice->original = 0;
                    toDice->residual = 1;

                    toWord->reverse = toDice;
                    ourGraph.nodes[j]->adj.push_back(toWord);
                    aLetter->adj.push_back(toDice);
                }
            }
            cout << "I am getting sick of this" << endl;
            Edge *toSink = new Edge;
            toSink->from = aLetter;
            toSink->to = ourSink;
            toSink->original = 1;
            toSink->residual = 0;

            Edge *fromSink = new Edge;
            fromSink->from = ourSink;
            fromSink->to = aLetter;
            fromSink->original = 1;
            fromSink->residual = 0;

            toSink->reverse = fromSink;

            aLetter->adj.push_back(toSink);
            ourSink->adj.push_back(fromSink);

            ourGraph.nodes.push_back(aLetter);
        }
        ourSink->id = nodeIndex;
        ourGraph.nodes.push_back(ourSink);

        //Now I am gonna print out the graph to make sure it looks right

        for(int i = 0; i < ourGraph.nodes.size(); i++){
            cout << "Node " << ourGraph.nodes[i]->id << " ";
            for(int j = 0; j < ourGraph.nodes[i]->letters.size(); j++){
                if(ourGraph.nodes[i]->letters[j]){
                    char letter = 'A' + j;
                    cout << letter ;
                }
            }
            cout << " Edges to " ;
            for(int j = 0; j < ourGraph.nodes[i]->adj.size(); j++){
                cout << ourGraph.nodes[i]->adj[j]->to->id << " "  ;
            }
            cout << endl ;
        }
        cout << endl << endl;

        //This is where we should do the BFS and path finding
        //cout << " do we even get here? " << endl;
        ourGraph.canISpell();    
        ourGraph.deleteHalfGraph();

    }

    return 0;
}
