#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

struct Node
{
    string song;
    Node *next;
    Node(const string &s) : song(s), next(nullptr) {}
};

class LinkedList
{
public:
    Node *head;
    Node *tail;
    int size;
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    void push_back(const string &s)
    {
        Node *node = new Node(s);
        if (!head)
            head = tail = node;
        else
            tail = tail->next = node;
        size++;
    }

    string removeAt(int pos)
    {

        Node *cur = head;
        Node *prev = nullptr;

        if (pos == 0)
        {
            string song = head->song;
            Node *temp = head;
            head = head->next;
            if (temp == tail)
                tail = nullptr;
            delete temp;
            size--;
            return song;
        }

        for (int i = 0; i < pos; ++i)
        {
            prev = cur;
            cur = cur->next;
        }

        string song = cur->song;
        prev->next = cur->next;
        if (cur == tail)
            tail = prev;
        delete cur;
        size--;
        return song;
    }

    void insertAt(int pos, const string &s)
    {

        Node *node = new Node(s);

        if (pos == 0)
        {
            node->next = head;
            head = node;
            if (!tail)
                tail = node;
            size++;
            return;
        }

        Node *cur = head;
        for (int i = 0; i < pos - 1; ++i)
        {
            cur = cur->next;
        }

        node->next = cur->next;
        cur->next = node;
        if (!node->next)
            tail = node;
        size++;
    }

    void reversegroup(int k)
    {
        if (!head || k <= 1)
            return;

        int count = size;
        int start = 0;
        while (start < count)
        {
            int end = min(start + k - 1, count - 1);
            reverserange(start, end);
            start += k;
        }
    }

    void printlist(int playlistnumber)
    {
        if (!head)
        {
            cout << "Playlist " << playlistnumber << " is empty\n";
            return;
        }
        cout << "Playlist " << playlistnumber << ":\n";
        Node *cur = head;
        while (cur)
        {
            cout << "> " << cur->song << "\n";
            cur = cur->next;
        }
    }

private:
    void reverserange(int first, int end)
    {
        if (first >= end || !head || !head->next)
            return;

        Node *dummy = new Node("");
        dummy->next = head;
        Node *prev = dummy, *cur = head;
        for (int i = 0; i < first; ++i)
        {
            prev = cur;
            cur = cur->next;
        }

        for (int i = 0; i < end - first; ++i)
        {
            if (!cur || !cur->next)
                break;
            Node *next = cur->next;
            cur->next = next->next;
            next->next = prev->next;
            prev->next = next;
        }

        head = dummy->next;
        tail = head;
        while (tail && tail->next)
            tail = tail->next;
    }
};

int main()
{
    int s;
    cin >> s;
    cin.ignore();

    vector<LinkedList *> playlists(s);
    for (int i = 0; i < s; ++i)
        playlists[i] = new LinkedList();

    string typesong;
    int cursor = 0;
    string line;

    while (getline(cin, line))
    {
        if (line.empty())
            continue;

        istringstream iss(line);
        string token;
        iss >> token;

        if (token == "ADD")
        {
            int ls;
            iss >> ls;
            if (ls <= 0 || ls > s)
                continue;
            playlists[ls - 1]->push_back(typesong);
            typesong.clear();
            cursor = 0;
        }
        else if (token == "CURSOR")
        {
            int i;
            char d;
            iss >> i >> d;
            if (d == 'L')
                cursor = max(0, cursor - i);
            else if (d == 'R')
                cursor = min((int)typesong.size(), cursor + i);
        }
        else if (token == "MOVE")
        {
            int ls, p, i;
            char d;
            iss >> ls >> p >> i >> d;
            int pos = p - 1;
            string song = playlists[ls - 1]->removeAt(pos);
            int newpos = (d == 'L') ? (pos - i) : (pos + i);
            playlists[ls - 1]->insertAt(newpos, song);
        }
        else if (token == "REVERSE")
        {
            int ls, k;
            iss >> ls >> k;
            if (ls <= 0 || ls > s)
                continue;
            if (playlists[ls - 1]->size == 0)
                cout << "Playlist " << ls << " is empty\n";
            else
                playlists[ls - 1]->reversegroup(k);
        }
        else if (token == "SAVE")
        {
            int ls;
            iss >> ls;
            playlists[ls - 1]->printlist(ls);
        }
        else
        {
            if (cursor > typesong.size())
                cursor = typesong.size();
            typesong.insert(typesong.begin() + cursor, line[0]);
            cursor++;
        }
    }
}
