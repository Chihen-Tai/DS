#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// 節點結構，儲存單一首歌的字串資料
struct Node {
    string song;
    Node* next;
    Node(const string& s) : song(s), next(nullptr) {}
};

// 自訂的 linked list，用來管理一個播放清單
class LinkedList {
public:
    Node* head;
    Node* tail;
    int size;

    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    ~LinkedList() {
        while (head) {
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
    }

    void push_back(const string& s) {
        Node* node = new Node(s);
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        size++;
    }

    string removeAt(int pos) {
        Node* cur = head;
        string ret;
        if (pos == 0) {
            ret = head->song;
            head = head->next;
            delete cur;
            size--;
            if (size == 0)
                tail = nullptr;
            return ret;
        }
        Node* prev = nullptr;
        for (int i = 0; i < pos; i++) {
            prev = cur;
            cur = cur->next;
        }
        ret = cur->song;
        prev->next = cur->next;
        if (cur == tail)
            tail = prev;
        delete cur;
        size--;
        return ret;
    }

    void insertAt(int pos, const string& s) {
        Node* node = new Node(s);
        if (pos == 0) {
            node->next = head;
            head = node;
            if (size == 0)
                tail = node;
            size++;
            return;
        }
        Node* cur = head;
        Node* prev = nullptr;
        for (int i = 0; i < pos; i++) {
            prev = cur;
            cur = cur->next;
        }
        prev->next = node;
        node->next = cur;
        if (node->next == nullptr)
            tail = node;
        size++;
    }

    void printList(int playlistNumber) {
        if (!head) {
            cout << "Playlist " << playlistNumber << " is empty\n";
            return;
        }
        cout << "Playlist " << playlistNumber << ":\n";
        Node* cur = head;
        while (cur) {
            cout << "> " << cur->song << "\n";
            cur = cur->next;
        }
    }

    void reverseGroups(int k) {
        if (!head || k <= 1)
            return;

        int count = size;
        int start = 0;
        while (start < count) {
            int end = min(start + k - 1, count - 1);
            reverseRange(start, end);
            start += k;
        }
    }

private:
    void reverseRange(int first, int last) {
        if (first >= last || !head || !head->next)
            return;
        Node* dummy = new Node("");
        dummy->next = head;
        dummy->next = head;
        Node* left = dummy;
        Node* now = head;
        int idx = 0;

        for (; now != nullptr && idx < first; idx++) {
            left = now;
            now = now->next;
        }

        while (now && now->next && idx < last) {
            Node* next = now->next;
            now->next = next->next;
            next->next = left->next;
            left->next = next;
            idx++;
        }

        head = dummy->next;
        tail = head;
        while (tail && tail->next)
            tail = tail->next;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int s;
    cin >> s;
    cin.ignore();

    vector<LinkedList*> playlists(s);
    for (int i = 0; i < s; i++) {
        playlists[i] = new LinkedList();
    }

    string typedSong;
    int cursor = 0;

    string line;
    while (getline(cin, line)) {
        if (line.empty())
            continue;
        if (line.size() == 1 && isalnum(line[0])) {
            typedSong.insert(typedSong.begin() + cursor, line[0]);
            cursor++;
        } else if (line.rfind("ADD ", 0) == 0) {
            int ls = stoi(line.substr(4)) - 1;
            playlists[ls]->push_back(typedSong);
            typedSong.clear();
            cursor = 0;
        } else if (line.rfind("CURSOR ", 0) == 0) {
            istringstream iss(line.substr(7));
            int i;
            char d;
            iss >> i >> d;
            if (d == 'L') 
                cursor = max(0, cursor - i);
            else if (d == 'R') 
                cursor = min((int)typedSong.size(), cursor + i);
        } else if (line.rfind("MOVE ", 0) == 0) {
            istringstream iss(line.substr(5));
            int ls, p, i;
            char d;
            iss >> ls >> p >> i >> d;
            ls--;
            string songToMove = playlists[ls]->removeAt(p - 1);
            int newPos = (d == 'L') ? (p - 1) - i : (p - 1) + i;
            playlists[ls]->insertAt(newPos, songToMove);
        } else if (line.rfind("REVERSE ", 0) == 0) {
            istringstream iss(line.substr(8));
            int ls, k;
            iss >> ls >> k;
            ls--;
            if (playlists[ls]->size == 0) {
                cout << "Playlist " << ls + 1 << " is empty\n";
            } else {
                playlists[ls]->reverseGroups(k);
            }
        } else if (line.rfind("SAVE ", 0) == 0) {
            int ls = stoi(line.substr(5)) - 1;
            playlists[ls]->printList(ls + 1);
        }
    }

    for (auto plist : playlists) {
        delete plist;
    }

    return 0;
}
