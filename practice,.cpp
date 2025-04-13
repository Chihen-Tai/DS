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
        if (head == nullptr)
        {
            head = tail = node;
        }
        else
        {
            tail->next = node;
            tail = node;
        };
        size++;
    }
    // 移除第 pos (0-indexed) 個節點，並回傳該節點歌名
    string removeAt(int pos)
    {
        Node*cur=head;
        string ret;
        if(pos == 0)
        {
            ret = head ->song;
            head = head ->next;
            delete cur;
            size --;
            if(size == 0)
                tail = nullptr;
            return ret;
        }
        Node *prev = nullptr;
        for(int i=0;i<pos;i++)
        {
            prev = cur;
            cur = cur->next;
        }
        ret = cur->song;
        prev->next = cur->next;
        if(cur ==tail)
        {
            tail = prev;
        }
        delete cur;
        size --;
        return ret;
    }

    
    void insertAt(int pos,const string&s)
    {
        Node*node = new Node(s);
        if(pos == 0)
        {
            node ->next = head;
            head = node;
            if(size == 0)
            {
                tail = node;
            }
            size ++;
            return;
        }

        Node *cur = head;
        Node *prev = nullptr;

        for(int i=0;i<pos;i++)
        {
            prev = cur;
            cur=cur->next;
        }

        prev->next = node;
        node->next = cur;
        if(node->next ==nullptr)
        {
            tail = node;
        }
        size++;
    }
    
    void printlist(int playlistnumber)
    {
        if(!head)
        {
            cout<<"Playlist "<<playlistnumber<<" is empty\n";
            return;
        }
        cout<<"Playlist "<<playlistnumber<<":\n";
        Node *cur=head;
        while(cur)
        {
            cout<<"> "<<cur->song<<"\n";
            cur=cur->next;
        }
    }

    void reversegroup(int k)
    {
        if(!head || k<=1)
            return;
        
        int count = size;
        int start = 0;
        while(start<count)
        {
            int end = min(start+k-1,count-1);
            reverserange(start,end);
            start+=k;
        }
    }
    private:
        void reverserange(int first,int end)
        {
            if(first>=end||!head||!head->next)
                return;
            Node*dummy = new Node("");
            dummy->next=head;
            Node*left=dummy;
            Node*now=head;

            int idx=0;
            //reverse the range [first,end]
            //dummy->next = head;
            for(;now!=nullptr&&idx<first;idx++)
            {
                left = now;
                now=now->next;
            }
            while(now&&now->next&&idx<end)
            {
                Node*next = now->next;
                now->next = next->next;
                next->next = left->next;
                left->next =next;
                idx++;
            }
            //reset head and tail
            // head = dummy->next;
            head = dummy->next;
            tail=head;
            while(tail&&tail->next)
            {
                tail=tail->next;
            }
        }
};

int main()
{
    int s;
    cin >>s;
    cin.ignore();

    vector<LinkedList*> playLists(s);
    for(int i=0;i<s;i++)
    {
        playLists[i] = new LinkedList();
    }

    string typesong;
    int cursor = 0;

    string line;
    while(getline(cin,line))
    {
        if(line.empty())
            continue;
        if(line.size()==1 && isalnum(line[0]))
        {
            //isalnum is a function that checks if the character is a letter or a number
            //if it is, it means the user is typing a song name
            //add the song to the current playlist
            typesong.insert(typesong.begin()+cursor,line[0]);
            //string.insert is a function that inserts a character at the position of cursor
            cursor++;
        }
        else if(line.rfind("ADD ",0)==0)
        {
            //rfind is a function that checks if the string starts with "ADD "
            //if it does, it means the user is adding a song to the playlist
            //ADD <playlist number> <song name>
            //the song name is the rest of the line after "ADD "
            int ls = stoi(line.substr(4))-1;
            //stoi is a function that converts a string to an integer
            //substr is a function that returns the substring of the string from the position of 4 to the end of the strings
            playLists[ls]->push_back(typesong);
            typesong.clear();
            cursor=0;
        }
        else if(line.rfind("CURSOR ",0)==0)
        {
            istringstream iss(line.substr(7));
            //iss is a stringstream object that is used to read the line after "CURSOR "
            int i;
            char d;
            iss>>i>>d;
            //iss>>i is a function that reads the integer from the stringstream object
            if(d=='L')
            {
                cursor=max(0,cursor - i);
            }
            else if(d=='R')
            {
                cursor = min((int)typesong.size(),cursor+i);
            }
        }
        else if(line.rfind("MOVE ",0)==0)
        {
            
        }
    }
}