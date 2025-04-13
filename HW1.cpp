#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// --------------------------
// Family 類別：紀錄每個家庭資料
// --------------------------
class Family {
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

    static Family* createFamily(const string &token, int id) {
        size_t commaPos = token.find(',');
        int ppl = stoi(token.substr(0, commaPos));
        int prc = stoi(token.substr(commaPos + 1));
        return new Family(id, ppl, prc);
    }
};

class FamilyQueue {
private:
    Family** arr;
    int size;
    int capacity;
public:
    FamilyQueue() {
        capacity = 1000;
        size = 0;
        arr = new Family*[capacity];
    }

    ~FamilyQueue() {
        delete[] arr;
    }

    int getSize() { return size; }

    Family* get(int index) {
        if (index < 0 || index >= size)
            return nullptr;
        return arr[index];
    }

    void push_back(Family* f) {
        if (size == capacity) {
            int newCap = capacity * 2;
            Family** newArr = new Family*[newCap];
            for (int i = 0; i < size; i++)
                newArr[i] = arr[i];
            delete[] arr;
            arr = newArr;
            capacity = newCap;
        }
        arr[size++] = f;
    }

    Family* pop_front() {
        if (size == 0)
            return nullptr;
        Family* ret = arr[0];
        for (int i = 1; i < size; i++)
            arr[i - 1] = arr[i];
        size--;
        return ret;
    }

    void remove_at(int index) {
        if (index < 0 || index >= size)
            return;
        for (int i = index + 1; i < size; i++)
            arr[i - 1] = arr[i];
        size--;
    }
};

class EnteredList {
public:
    struct EnteredFamily {
        int id;
        int people;
        int price;
    };
private:
    EnteredFamily* arr;
    int size;
    int capacity;
public:
    EnteredList() {
        capacity = 1000;
        size = 0;
        arr = new EnteredFamily[capacity];
    }

    ~EnteredList() {
        delete[] arr;
    }

    void push_back(int id, int people, int price) {
        if (size == capacity) {
            int newCap = capacity * 2;
            EnteredFamily* newArr = new EnteredFamily[newCap];
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
// --------------------------
// Place 類別：模擬每個場所的排隊、結帳與統計狀態
// --------------------------
class Place {
private:
    FamilyQueue queue;    // 排隊的家庭
    EnteredList entered;  // 已入場家庭紀錄

    // 重新計算保護旗標：
    // 清除所有家庭的 isProtected，然後將隊列中第一個 special family 前的所有家庭標記為受保護。
    
public:
    long long totalMoney;
    long long totalPeopleEntered;
    long long totalPeopleQuit;
    int nextFamilyId;
    
    Place() {
        totalMoney = 0;
        totalPeopleEntered = 0;
        totalPeopleQuit = 0;
        nextFamilyId = 1;
    }
    
    // 取得家庭在隊列中的位置（不計正在結帳者）
    // 若隊首正在結帳 (reg_time != -1)，則 index 1 為位置 1；否則 index 0 為位置 1
    int getPosition(int index) {
        if (queue.getSize() == 0)
            return -1;
        if (queue.get(0)->reg_time != -1)
            return index;       
        else
            return index + 1;  
    }
    
    // 結帳處理：若隊首正在結帳則倒數減 1 分鐘；結束後更新統計並讓下一家庭開始結帳
    void processRegistration() {
        if (queue.getSize() == 0)
            return;
        Family* front = queue.get(0);
        if (front->reg_time != -1) {
            front->reg_time--;
            if (front->reg_time == 0) {
                // Special 家庭免費入場（不計票價）
                if (!front->isSpecial) {
                    totalMoney += front->price;
                }
                totalPeopleEntered += front->people;
                entered.push_back(front->id, front->people, front->price);
                delete front;
                queue.remove_at(0);
                if (queue.getSize() > 0 && queue.get(0)->reg_time == -1)
                    queue.get(0)->reg_time = 2;
            }
        } else {
            // 尚未開始結帳，直接啟動
            front->reg_time = 2;
        }
    }
    
    // 更新等待時間：不在結帳中的家庭等待時間加 1 分鐘
    void updateWaiting() {
        for (int i = 0; i < queue.getSize(); i++) {
            Family* f = queue.get(i);
            if (f->reg_time == -1)
                f->waiting++;
        }
    }
    
    // 處理 GoGo 家庭影響退出：若 GoGo 家庭在後，檢查其前一家庭是否可退出（若未受保護）
    void processGoGoQuit(int X) {
        bool removed = true;
        while (removed) {
            removed = false;
            int size = queue.getSize();
            for (int i = 1; i < size; i++) {
                Family* curr = queue.get(i);
                if (curr->isGoGo) {
                    int candidateIdx = i - 1;
                    int pos = getPosition(candidateIdx);
                    Family* candidate = queue.get(candidateIdx);
                    if (candidate->isProtected)
                        continue;
                    if (candidate->reg_time == -1 && !candidate->isGoGo && !candidate->isSpecial &&
                        candidate->waiting >= 60 && pos > X) {
                        totalPeopleQuit += candidate->people;
                        delete candidate;
                        queue.remove_at(candidateIdx);
                        removed = true;
                      
                        break;
                    }
                }
            }
        }
    }
    
    // 處理隊尾退出：從隊尾開始，若家庭未受保護且符合退出條件，則退出
    void processTailQuit(int X) {
        bool changed = true;
        while (changed && queue.getSize() > 0) {
            changed = false;
            int idx = queue.getSize() - 1;
            int pos = getPosition(idx);
            Family* tail = queue.get(idx);
            if (tail->isProtected)
                break;
            if (tail->reg_time == -1 && !tail->isGoGo && !tail->isSpecial &&
                tail->waiting >= 60 && pos > X) {
                totalPeopleQuit += tail->people;
                delete tail;
                queue.remove_at(idx);
                changed = true;
            }
        }
    }
    
    // 檢查 special offer：當連續家庭票價總和剛好等於 Y，則將連續區間之後的第一個家庭標記為 special，
    // 並將該 special family 前的所有家庭設為受保護。
    // ※修改處：如果該家庭原本同時符合 GoGo 和 special，則會選擇成為 special，
    //     因此這裡在標記 special 時同時將 isGoGo 設為 false。
void checkSpecialOffer(int Y) {
    if (Y == -1) return;

    int n = queue.getSize();
    for (int i = 0; i < n; i++) {
        if (queue.get(i)->isSpecial) return;
    }

    // 第一階段：只看 queue[i~j]
    for (int i = 0; i < n - 1; i++) {
        int sum = 0;
        for (int j = i; j < n - 1; j++) {
            sum += queue.get(j)->price;
            if (sum == Y) {
                queue.get(j + 1)->isSpecial = true;
                queue.get(j + 1)->isGoGo = false;
                for (int k = 0; k <= j; k++) {
                    queue.get(k)->isProtected = true;
                }
                return;
            }
            if (sum > Y) break;
        }
    }

    // 第二階段 fallback：entered + queue sliding window
    const int MAX = 20000;
    int priceArr[MAX], idArr[MAX];
    int total = 0;

    for (int i = 0; i < entered.getSize(); i++) {
        priceArr[total] = entered.get(i).price;
        idArr[total++] = -1;
    }
    for (int i = 0; i < n; i++) {
        priceArr[total] = queue.get(i)->price;
        idArr[total++] = queue.get(i)->id;
    }

    int L = 0, R = 0, sum = 0;
    while (R < total) {
        sum += priceArr[R];
        while (sum > Y && L <= R) sum -= priceArr[L++];
        if (sum == Y) {
            for (int i = R + 1; i < total; i++) {
                if (idArr[i] == -1) continue;
                int specialId = idArr[i];
                for (int j = 0; j < n; j++) {
                    if (queue.get(j)->id == specialId) {
                        queue.get(j)->isSpecial = true;
                        queue.get(j)->isGoGo = false;
                        for (int k = 0; k < j; k++) {
                            queue.get(k)->isProtected = true;
                        }
                        return;
                    }
                }
                break;
            }
        }
        R++;
    }
}

    // 新增家庭 (由 "A,P" 字串建立)
    void addFamily(const string &token) {
        Family* newF = Family::createFamily(token, nextFamilyId++);
        if (queue.getSize() == 0)
            newF->reg_time = 2; // 若隊列空，直接開始結帳
        queue.push_back(newF);
        
    }
    
    // 讀取初始排隊資料 (一行字串)
    void addInitialFamilies(const string &line) {
        istringstream iss(line);
        string token;
        while (iss >> token) {
            if (token == "0")
                break;
            addFamily(token);
        }
    }
    
    // 模擬一分鐘更新流程：先處理結帳、更新等待、處理退出，再加入新家庭，最後檢查 special offer
    void simulateMinute(const string &updateLine, int X, int Y) {
        processRegistration();
        checkSpecialOffer(Y);
        updateWaiting();
        checkSpecialOffer(Y);
        processGoGoQuit(X);
        checkSpecialOffer(Y);
        processTailQuit(X);
        checkSpecialOffer(Y);
        checkSpecialOffer(Y);
        
        istringstream iss(updateLine);
        string token;
        while (iss >> token) {
            if (token == "0")
                break;
            addFamily(token);
        }
        checkSpecialOffer(Y);
    }
    
    // 輸出該場所統計結果與已入場家庭紀錄 (從最新到最舊)
    void printSummary(int placeIndex) {
        int currentPeople = 0;
        for (int i = 0; i < queue.getSize(); i++)
            currentPeople += queue.get(i)->people;
        cout << "Place " << placeIndex << " Summary" << "\n";
        cout << "Total Money: " << totalMoney << "\n";
        cout << "Total People Entered: " << totalPeopleEntered << "\n";
        cout << "Total People Quit: " << totalPeopleQuit << "\n";
        cout << "Current People Queuing: " << currentPeople << "\n\n";
        cout << "Entered Families :" << "\n";
        for (int i = entered.getSize() - 1; i >= 0; i--) {
            auto ef = entered.get(i);
            cout << "Family " << ef.id << " with " << ef.people << " people" << "\n";
        }
        cout << "\n";
    }
    
    // 清除尚未處理的排隊家庭
    void clearQueue() {
        while (queue.getSize() > 0) {
            Family* f = queue.pop_front();
            delete f;
        }
    }
};

//
// --------------------------
// 主程式
// --------------------------
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int F, X, Y, M;
    cin >> F >> X >> Y >> M;
    cin.ignore();
    
    // 建立 F 個 Place 物件
    Place* places = new Place[F];
    for (int i = 0; i < F; i++){
        string line;
        while(getline(cin, line)) {
            if(line.size() != 0)
                break;
        }
        places[i].addInitialFamilies(line);
    }
    
    // 從第 0 分鐘開始模擬 M 分鐘
    for (int minute = 0; minute < M; minute++){
        for (int i = 0; i < F; i++){
            string updateLine;
            while(getline(cin, updateLine)) {
                if(updateLine.size() != 0)
                    break;
            }
            places[i].checkSpecialOffer(Y);
            places[i].simulateMinute(updateLine, X, Y);
        }
    }
    
    // 輸出各場所結果
    for (int i = 0; i < F; i++){
        places[i].printSummary(i);
    }
    
    // 清除記憶體
    for (int i = 0; i < F; i++){
        places[i].clearQueue();
    }
    delete[] places;
    
    return 0;
}
