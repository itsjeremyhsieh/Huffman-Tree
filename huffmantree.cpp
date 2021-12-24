#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <vector>

using namespace std;


class bitChar{
    public:
        unsigned char* c;
        int shift_count;
        string BITS;

        bitChar()
        {
            shift_count = 0;
            c = (unsigned char*)calloc(1, sizeof(char));
        }

        void setBITS(string X)
        {
            BITS = X;
        }

        int insertBits(ofstream& outf)
        {
            int total = 0;

            while(BITS.length())
            {
                if(BITS[0] == '1')
                    *c |= 1;
                *c <<= 1;
                ++shift_count;
                ++total;
                BITS.erase(0, 1);

                if(shift_count == 7 )
                {
                    if(BITS.size()>0)
                    {
                        if(BITS[0] == '1')
                            *c |= 1;
                        ++total;
                        BITS.erase(0, 1);
                    }

                    writeBits(outf);
                    shift_count = 0;
                    free(c);
                    c = (unsigned char*)calloc(1, sizeof(char));
                }
            }

            if(shift_count > 0)
            {
                *c <<= (7 - shift_count);
                writeBits(outf);
                free(c);
                c = (unsigned char*)calloc(1, sizeof(char));
            }
            outf.close();
            return total;
        }

        void writeBits(ofstream& outf)
        {
            outf << *c;
        }
};

class Node {
    public:
        int value;
        char character;
        Node* left_child;
        Node* right_child;

        Node(int val = 0, char ch = 0, Node* lchild = nullptr, Node* rchild = nullptr) {
            value = val;
            character = ch;
            left_child = lchild;
            right_child = rchild;
        }
};

class comp {
    public:
        bool operator() (Node* a, Node* b) {
            return a->value > b->value;
        }
};

class HuffmanCoding {
    private:
        unordered_map<char, int> fraquency;
        unordered_map<char, string> code_word;
        priority_queue<Node*, vector<Node*>, comp> nodes;
        vector<char> plaintext; 
        int countWord = 0;
        int countByte = 0;
    public:
        HuffmanCoding(string filepath) {
            //read the file and count the fraquency of each character
            ifstream fs(filepath, fstream::in);
            char ch;
            while(fs.get(ch)) {
                if(ch == '\n')
                        continue;
                countWord++;
                plaintext.push_back(ch);
                if(fraquency.count(ch))
                    fraquency[ch]++;
                else
                    fraquency[ch] = 1;
            }
            fs.close();
        }
        void pushIntoQueue() {
            //push the element in fracquency into priority queue
            for(auto i = fraquency.begin() ; i != fraquency.end() ; i++) {
                nodes.push(new Node((*i).second, (*i).first));
            }
        }
        Node* huffmanTreeCreate() {
            //create the tree and return the root's address
            for(int i = 0 ; i < fraquency.size()-1 ; i++) {
                Node* left = nodes.top();
                nodes.pop();
                Node* right = nodes.top();
                nodes.pop();
                Node* parent = new Node(left->value+right->value, 0, left, right);
                nodes.push(parent);
            }
            return nodes.top();
        }
        void show() {
            //show the huffman code on the terminal
            for(auto i = code_word.begin() ; i != code_word.end() ; i++) {
                cout << (*i).first << " -> " << (*i).second << endl;
            }
        }
        void dictionary(Node& cur, string code) {
            //make the dictionary
            if(!cur.left_child && !cur.right_child) {
                code_word[cur.character] = code;
                return;
            }
            if(cur.left_child)
                dictionary(*(cur.left_child), code+"0");
            if(cur.right_child)
                dictionary(*(cur.right_child), code+"1");
        }
        void outputToTxt(string filepath = "output.txt") {
            //output a txt file
            ofstream fs(filepath, fstream::out);
            for(auto i = 0 ; i < plaintext.size() ; i++) {
                string str = code_word[plaintext[i]];
                fs<<str;
            }
            fs.close();
        }
        
        void outputToBin(string filepath = "output_bin.dat") {
            ofstream myFile;
            myFile.open(filepath, ios::binary | ios::out);
            string str = "";
            for(auto i = 0 ; i < plaintext.size() ; i++) {
                str += code_word[plaintext[i]];
            }
            countByte = str.length();
            bitChar bchar;
            bchar.setBITS(str);
            bchar.insertBits(myFile);
            myFile.close();
        }
        //TODO a method to display the tree(optional)

        void compressRatio() {
			float ratio = (float)countByte / (countWord * 8);
			cout << "Before compress: " << countWord * 8 << "bits" << endl;
			cout << "After compress: " << countByte << "bits" << endl;
			cout << "Compress Ratio = " << ratio * 100 << "%" << endl;
        }
};


//TODO need a menu function
int main() {
    Node* root;
    string filepath;
    cout<<"Input the file path:";
    cin>>filepath;
    HuffmanCoding huffmantree(filepath);
    huffmantree.pushIntoQueue();
    root = huffmantree.huffmanTreeCreate();
    huffmantree.dictionary(*root, "");
    huffmantree.show();
    huffmantree.outputToTxt();
    huffmantree.outputToBin();
    huffmantree.compressRatio();
}