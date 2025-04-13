#include <iostream>
#include <string>
#include <sstream>
using namespace std;
struct Family
{
    int id;         // 家庭 ID
    int arrival;    // 抵達時間（分鐘）
    int payment;    // 需支付金額
    int finish;     // 完成報到時間（分鐘）
    int servedby;   // 服務該家庭的機器人 ID
};
int main()
{
    for (int min = 1; min <= 7; min++)
    {
        string line;
        getline(cin, line);
        stringstream ss(line);
        string token;
        while (ss >> token)
        {
            size_t comm = token.find(',');
            Family f;
            if(comm == string::npos)
                break;
            f.id = stoi(token.substr(0, comm));
            f.payment = stoi(token.substr(comm+1));
            f.arrival = min;
            f.finish = -1;
            f.servedby = -1;
            cout<<"Family ID: " << f.id << ", Payment: " << f.payment << ", Arrival: " << f.arrival << "\n";
        }
    }
}