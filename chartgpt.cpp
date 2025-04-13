#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class Family
{
public:
    int id;
    int people;
    int price;
    int waiting;
    int reg_time;
    bool isSpecial;
    bool isGoGo;
    bool isProtected;

    Family(int _id, int _people, int _price)
        : id(_id), people(_people), price(_price), waiting(0), reg_time(-1),
          isSpecial(false), isGoGo(false), isProtected(false)
    {
        if (_people >= 100)
            isGoGo = true;
    }

    static Family *createFamily(const string &token, int id)
    {
        size_t commaPos = token.find(',');
        int ppl = stoi(token.substr(0, commaPos));
        int prc = stoi(token.substr(commaPos + 1));
        return new Family(id, ppl, prc);
    }
};

class FamilyQueue
{
private:
    Family **arr;
    int size;
    int capacity;

public:
    FamilyQueue()
    {
        capacity = 1000;
        size = 0;
        arr = new Family *[capacity];
    }

    ~FamilyQueue()
    {
        delete[] arr;
    }

    int getSize() { return size; }

    Family *get(int index)
    {
        if (index < 0 || index >= size)
            return nullptr;
        return arr[index];
    }

    void push_back(Family *f)
    {
        if (size == capacity)
        {
            int newCap = capacity * 2;
            Family **newArr = new Family *[newCap];
            for (int i = 0; i < size; i++)
                newArr[i] = arr[i];
            delete[] arr;
            arr = newArr;
            capacity = newCap;
        }
        arr[size++] = f;
    }

    Family *pop_front()
    {
        if (size == 0)
            return nullptr;
        Family *ret = arr[0];
        for (int i = 1; i < size; i++)
            arr[i - 1] = arr[i];
        size--;
        return ret;
    }

    void remove_at(int index)
    {
        if (index < 0 || index >= size)
            return;
        for (int i = index + 1; i < size; i++)
            arr[i - 1] = arr[i];
        size--;
    }
};

class EnteredList
{
public:
    struct EnteredFamily
    {
        int id;
        int people;
        int price;
    };

private:
    EnteredFamily *arr;
    int size;
    int capacity;

public:
    EnteredList()
    {
        capacity = 1000;
        size = 0;
        arr = new EnteredFamily[capacity];
    }

    ~EnteredList()
    {
        delete[] arr;
    }

    void push_back(int id, int people, int price)
    {
        if (size == capacity)
        {
            int newCap = capacity * 2;
            EnteredFamily *newArr = new EnteredFamily[newCap];
            for (int i = 0; i < size; i++)
                newArr[i] = arr[i];
            delete[] arr;
            arr = newArr;
            capacity = newCap;
        }
        arr[size].id = id;
        arr[size].people = people;
        arr[size].price = price;
        size++;
    }

    int getSize() { return size; }

    EnteredFamily get(int index) { return arr[index]; }
};

class Place
{
private:
    FamilyQueue queue;
    EnteredList entered;

    void updateProtection()
    {
        int n = queue.getSize();
        for (int i = 0; i < n; i++)
        {
            queue.get(i)->isProtected = false;
        }
        int specialIndex = -1;
        for (int i = 0; i < n; i++)
        {
            if (queue.get(i)->isSpecial)
            {
                specialIndex = i;
                break;
            }
        }
        if (specialIndex > 0)
        {
            for (int i = 0; i < specialIndex; i++)
            {
                queue.get(i)->isProtected = true;
            }
        }
    }

public:
    long long totalMoney;
    long long totalPeopleEntered;
    long long totalPeopleQuit;
    int nextFamilyId;

    Place()
    {
        totalMoney = 0;
        totalPeopleEntered = 0;
        totalPeopleQuit = 0;
        nextFamilyId = 1;
    }

    int getPosition(int index)
    {
        if (queue.getSize() == 0)
            return -1;
        if (queue.get(0)->reg_time != -1)
            return index;
        else
            return index + 1;
    }

    void processRegistration()
    {
        if (queue.getSize() == 0)
            return;
        Family *front = queue.get(0);
        if (front->reg_time != -1)
        {
            front->reg_time--;
            if (front->reg_time == 0)
            {

                totalMoney += front->price;

                totalPeopleEntered += front->people;
                entered.push_back(front->id, front->people, front->price);
                delete front;
                queue.remove_at(0);
                updateProtection();
                if (queue.getSize() > 0 && queue.get(0)->reg_time == -1)
                    queue.get(0)->reg_time = 2;
            }
        }
        else
        {
            front->reg_time = 2;
        }
    }

    void updateWaiting()
    {
        for (int i = 0; i < queue.getSize(); i++)
        {
            Family *f = queue.get(i);
            if (f->reg_time == -1)
                f->waiting++;
        }
    }

    void processGoGoQuit(int X)
    {
        bool removed = true;
        while (removed)
        {
            removed = false;
            int size = queue.getSize();
            for (int i = 1; i < size; i++)
            {
                Family *curr = queue.get(i);
                if (curr->isGoGo)
                {
                    int candidateIdx = i - 1;
                    int pos = getPosition(candidateIdx);
                    Family *candidate = queue.get(candidateIdx);
                    if (candidate->isProtected)
                        continue;
                    if (candidate->reg_time == -1 && !candidate->isGoGo && !candidate->isSpecial &&
                        candidate->waiting >= 60 && pos > X)
                    {
                        totalPeopleQuit += candidate->people;
                        delete candidate;
                        queue.remove_at(candidateIdx);
                        removed = true;
                        updateProtection();
                        break;
                    }
                }
            }
        }
    }

    void processTailQuit(int X)
    {
        bool changed = true;
        while (changed && queue.getSize() > 0)
        {
            changed = false;
            int idx = queue.getSize() - 1;
            int pos = getPosition(idx);
            Family *tail = queue.get(idx);
            if (tail->isProtected)
                break;
            if (tail->reg_time == -1 && !tail->isGoGo && !tail->isSpecial &&
                tail->waiting >= 60 && pos > X)
            {
                totalPeopleQuit += tail->people;
                delete tail;
                queue.remove_at(idx);
                changed = true;
                updateProtection();
            }
        }
    }

    void checkSpecialOffer(int Y)
    {
        if (Y == -1)
            return;
        int n = queue.getSize();
        for (int i = 0; i < n; i++)
        {
            if (queue.get(i)->isSpecial)
                return;
        }
        const int MAX_COMB = 20000;
        int priceArr[MAX_COMB];
        int idArr[MAX_COMB];
        int total = 0;
        for (int i = 0; i < entered.getSize(); i++)
        {
            priceArr[total] = entered.get(i).price;
            idArr[total] = entered.get(i).id;
            total++;
        }
        for (int i = 0; i < n; i++)
        {
            Family *f = queue.get(i);
            priceArr[total] = f->price;
            idArr[total] = f->id;
            total++;
        }
        int L = 0, R = 0, sum = 0;
        while (R < total)
        {
            sum += priceArr[R];
            while (sum > Y && L <= R)
            {
                sum -= priceArr[L];
                L++;
            }
            if (sum == Y)
            {
                int targetIndex = R + 1;
                if (targetIndex < total)
                {
                    int specialId = idArr[targetIndex];
                    for (int i = 0; i < n; i++)
                    {
                        Family *f = queue.get(i);
                        if (f->id == specialId)
                        {
                            f->isSpecial = true;
                            f->isGoGo = false;
                            for (int j = 0; j < i; j++)
                            {
                                queue.get(j)->isProtected = true;
                            }
                            return;
                        }
                    }
                }
                break;
            }
            R++;
        }
    }

    void addFamily(const string &token)
    {
        Family *newF = Family::createFamily(token, nextFamilyId++);
        if (queue.getSize() == 0)
            newF->reg_time = 2;
        queue.push_back(newF);
    }

    void addInitialFamilies(const string &line)
    {
        istringstream iss(line);
        string token;
        while (iss >> token)
        {
            if (token == "0")
                break;
            addFamily(token);
        }
    }

    void simulateMinute(const string &updateLine, int X, int Y)
    {
        processRegistration();
        checkSpecialOffer(Y);
        updateWaiting();
        processGoGoQuit(X);
        processTailQuit(X);
        updateProtection();
        istringstream iss(updateLine);
        string token;
        while (iss >> token)
        {
            if (token == "0")
                break;
            addFamily(token);
        }
        checkSpecialOffer(Y);
        updateProtection();
    }

    void printSummary(int placeIndex)
    {
        int currentPeople = 0;
        for (int i = 0; i < queue.getSize(); i++)
            currentPeople += queue.get(i)->people;
        cout << "Place " << placeIndex << " Summary" << "\n";
        cout << "Total Money: " << totalMoney << "\n";
        cout << "Total People Entered: " << totalPeopleEntered << "\n";
        cout << "Total People Quit: " << totalPeopleQuit << "\n";
        cout << "Current People Queuing: " << currentPeople << "\n\n";
        cout << "Entered Families :" << "\n";
        for (int i = entered.getSize() - 1; i >= 0; i--)
        {
            auto ef = entered.get(i);
            cout << "Family " << ef.id << " with " << ef.people << " people" << "\n";
        }
        cout << "\n";
    }

    void clearQueue()
    {
        while (queue.getSize() > 0)
        {
            Family *f = queue.pop_front();
            delete f;
        }
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int F, X, Y, M;
    cin >> F >> X >> Y >> M;
    cin.ignore();
    Place *places = new Place[F];
    for (int i = 0; i < F; i++)
    {
        string line;
        while (getline(cin, line))
        {
            if (line.size() != 0)
                break;
        }
        places[i].addInitialFamilies(line);
    }
    for (int minute = 0; minute < M; minute++)
    {
        for (int i = 0; i < F; i++)
        {
            string updateLine;
            while (getline(cin, updateLine))
            {
                if (updateLine.size() != 0)
                    break;
            }
            places[i].simulateMinute(updateLine, X, Y);
        }
    }
    for (int i = 0; i < F; i++)
    {
        places[i].printSummary(i);
    }
    for (int i = 0; i < F; i++)
    {
        places[i].clearQueue();
    }
    delete[] places;
    return 0;
}
