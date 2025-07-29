#include <bits/stdc++.h>
using namespace std;

int numPage, numFrame;

struct Node {
    int data;
    Node* next;
    Node(int d) : data(d), next(nullptr) {}
};

// FIFO
int fifo(const string& refString) {
    int miss = 0, index = 0;
    vector<int> frames(numFrame, -1);

    for (char c : refString) {
        int page = c - '0';
        bool hit = false;

        for (int f : frames) {
            if (f == page) {
                hit = true;
                break;
            }
        }

        if (!hit) {
            frames[index] = page;
            index = (index + 1) % numFrame;
            miss++;
        }
    }
    return miss;
}

// LRU
int lru(const string& refString) {
    if (refString.empty()) return 0;

    int miss = 1, hit = 0, listSize = 1;
    Node* front = new Node(refString[0] - '0');
    Node* rear = front;

    for (int i = 1; i < refString.size(); i++) {
        int page = refString[i] - '0';
        Node* temp = front;
        Node* prev = nullptr;

        bool found = false;
        while (temp) {
            if (temp->data == page) {
                found = true;
                break;
            }
            prev = temp;
            temp = temp->next;
        }

        if (!found) {
            miss++;
            Node* newNode = new Node(page);
            if (listSize < numFrame) {
                listSize++;
            } else {
                Node* oldFront = front;
                front = front->next;
                delete oldFront;
            }
            rear->next = newNode;
            rear = newNode;
        } else {
            if (temp->next == nullptr) continue; // already at rear
            if (prev) prev->next = temp->next;
            else front = front->next;
            rear->next = temp;
            temp->next = nullptr;
            rear = temp;
            hit++;
        }
    }

    // Clean up
    while (front) {
        Node* next = front->next;
        delete front;
        front = next;
    }

    return miss;
}

// LFU
int lfu(const string& refString) {
    unordered_map<int, int> freq;
    vector<int> frames(numFrame, -1);
    int index = 0, miss = 0, usedFrames = 0;

    for (char c : refString) {
        int page = c - '0';
        bool hit = false;

        for (int f : frames) {
            if (f == page) {
                freq[page]++;
                hit = true;
                break;
            }
        }

        if (!hit) {
            miss++;
            if (usedFrames < numFrame) {
                frames[index] = page;
                freq[page]++;
                usedFrames++;
                index = (index + 1) % numFrame;
            } else {
                // Find minimum frequency
                int minFreq = INT_MAX;
                for (int f : frames) minFreq = min(minFreq, freq[f]);

                // Find candidates with minFreq
                vector<int> candidates;
                for (int i = 0; i < numFrame; i++) {
                    if (freq[frames[i]] == minFreq) {
                        candidates.push_back(i);
                    }
                }

                int replaceIdx = candidates[0];
                // Use FIFO if tie
                if (candidates.size() > 1) {
                    for (int i = 0; i < numFrame; i++) {
                        if (find(candidates.begin(), candidates.end(), index) != candidates.end()) {
                            replaceIdx = index;
                            break;
                        }
                        index = (index + 1) % numFrame;
                    }
                }

                freq[frames[replaceIdx]] = 0;
                frames[replaceIdx] = page;
                freq[page]++;
                index = (replaceIdx + 1) % numFrame;
            }
        }
    }
    return miss;
}

// Main
int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: ./a.out <inputfile> <FF|LF|LR> ...\n";
        return 1;
    }

    string fileName = string(argv[1]) + ".txt";
    ifstream fin(fileName);
    if (!fin) {
        cerr << "[ERROR] Cannot open file: " << fileName << "\n";
        return 1;
    }

    string line;
    getline(fin, line);
    stringstream ss(line);
    ss >> numPage >> numFrame;

    int testCase = 1;
    while (getline(fin, line)) {
        string refString = "";
        for (char c : line) {
            if (isdigit(c)) refString += c;
        }

        cout << "\n---------------- Test Case " << testCase++ << " ----------------\n";
        cout << "Reference String: " << refString << "\n";

        for (int i = 2; i < argc; i++) {
            string algo = argv[i];
            if (algo == "FF") {
                cout << "FIFO -> Page Faults: " << fifo(refString) << '\n';
            } else if (algo == "LR") {
                cout << "LRU  -> Page Faults: " << lru(refString) << '\n';
            } else if (algo == "LF") {
                cout << "LFU  -> Page Faults: " << lfu(refString) << '\n';
            } else {
                cout << "[ERROR] Unknown algorithm: " << algo << '\n';
            }
        }
        cout << "--------------------------------------------\n";
    }

    fin.close();
    return 0;
}
