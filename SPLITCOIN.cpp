#include<iostream>
#include<vector>
#include<map>
#include<queue>
#include<set>
#include<fstream>
#include<string>
#include <tuple>

using namespace std;

class Edge
{
public:
    int from;
    int to;
    int flow;
    int capacity;
    Edge* rev;

    Edge(int from, int to, int flow, int capacity):from(from), to(to), flow(flow), capacity(capacity), rev(NULL)
    {
    }
};

class Graph
{
public:
    int V;
    vector <int> level;
    vector<pair<int, int>>edgeSet;
    vector <vector<Edge>> adj;
    Graph(int V)
    {
        this->V = V;
        adj.resize(V);
        level.resize(V);
    }
    void addEdge(int, int, int);
    int maximumFlow(int, int);
    void print(vector<tuple<string, string, int>>&);
    bool bfs(int, int);
    int sendFlow(int, int, int, vector<int>&);
};

void Graph :: addEdge(int from, int to, int amount)
{
    Edge e1(from, to, 0, amount);
    Edge e2(to, from, 0, 0);
    e1.rev = &e2;
    e2.rev = &e1;
    adj[from].push_back(e1);
    adj[to].push_back(e2);
    edgeSet.push_back(make_pair(from, to));
}

void Graph :: print(vector<tuple<string, string, int>>&data)
{
    cout << "Given transactions\n\n";
    for(auto i: data)
    {
        cout << get<0>(i) << " owes " << get<1>(i) << " $" << get<2>(i) << endl;
    }
}

bool Graph :: bfs(int source, int sink)
{
    for (int i = 0 ; i < V ; i++)
        level[i] = -1;
    level[source] = 0;
    queue <int> q;
    q.push(source);
    while(!q.empty())
    {
        int u = q.front();
        q.pop();
        for(auto i : adj[u])
        {
            // unvisited & not saturated
            if(level[i.to] < 0 && i.flow < i.capacity)
            {
                level[i.to] = level[u] + 1;
                q.push(i.to);
            }
        }
    }
    // path available
    return level[sink] != -1;
}

int Graph :: sendFlow(int current, int flow, int sink, vector<int>&start)
{
    if(current == sink)
        return flow;

    // dfs-find augmenting path
    for(; start[current] < adj[current].size(); start[current]++)
    {
        Edge &e = adj[current][start[current]];
        if(level[e.to] == level[current]+1 && e.flow < e.capacity)
        {
            int currentFlow = min(flow, e.capacity-e.flow);
            int bottleNeck = sendFlow(e.to, currentFlow, sink, start);
            if(bottleNeck > 0)
            {
                e.flow += bottleNeck;
                e.rev->flow -= bottleNeck;
                return bottleNeck;
            }
        }
    }
    return 0;
}

int Graph :: maximumFlow(int source, int sink)
{
    if(source == sink)
        return -1;

    int maxFlow = 0;
    while(bfs(source, sink))
    {
        vector<int>start(V, 0);
        while(int flow = sendFlow(source, INT_MAX, sink, start))
            maxFlow += flow;
    }
    return maxFlow;
}

set<tuple<string, string, int>> initialfn(vector<tuple<string, string, int>>data)
{
    map<string, int>indices;
     int counter = 1;

    // assign ids to each person
    for(auto x:data)
    {
        if(!indices[get<0>(x)])
            indices[get<0>(x)] = counter++;
        if(!indices[get<1>(x)])
            indices[get<1>(x)] = counter++;
    }
    // create graph
    Graph g(counter);
    for(auto x:data)
    {
        g.addEdge(indices[get<0>(x)], indices[get<1>(x)], get<2>(x));
    }

    g.print(data);

     for(auto &i: g.edgeSet)
     {
         int u = i.first;
         int v = i.second;
         int m = g.maximumFlow(u, v);

         // create a new edge
         if(m > 0)
        {
             for(auto &j: g.adj[u])
             {
                 if(j.to == v)
                 {
                    j.flow = 0;
                    j.capacity = m;
                 }
             }
        }
     }
    set<tuple<string, string, int>>finalSet;
    cout << endl << "Final set of transactions after simplification: \n\n";
    string fname = "";
    string sname = "";
    for(int i = 0; i < g.V; i++)
    {
        for(auto j : g.adj[i])
        {
            if(j.flow < j.capacity)
            {
                for(auto x:indices)
                {

                    if(x.second == j.from)
                        fname = x.first;
                    if(x.second == j.to)
                        sname = x.first;
                }
                finalSet.insert(make_tuple(fname, sname, j.capacity-j.flow));
            }
        }
    }
    for(auto i:finalSet)
        cout << get<0>(i) << " owes " << get<1>(i) << " $" << get<2>(i) << endl;
    return finalSet;

}
string user = "";
string filename = "";

void findAllUserTransactions(set<tuple<string, string, int>>finalSet)
{
    for(auto trans: finalSet)
    {
        if(get<0>(trans) == user || get<1>(trans) == user)
            cout << get<0>(trans) << " owes " << get<1>(trans) << " amount " << get<2>(trans) << "\n\n";
    }
}

int main()
{
     int N;
     int amount;
     string creditor, debtor;
     vector<tuple<string, string, int>>data;

    OPTION1:
    int opt, flag = 0;
    cout << "Welcome to SPLITCOIN\n\n";
    cout << "Press 1 to login\nPress 2 to signup\nPress 3 to exit\n\n";
    cout << "Choose an option:  ";
    cin >> opt;
    string db = "";
    switch(opt)
    {
    case 1:
    {
        string line;
        cout << "Enter username:" << endl;
        cin >> user;
        cout << "Welcome back " << user << "! These are your previous transactions:" << endl;
        filename = user + ".txt";
        vector<tuple<string, string, int>>d;
        ifstream fin;
        fin.open(filename);
        while (fin)
        {
            if(fin.eof())
                break;
            getline(fin, line);
            string l[3];
            int j = 0;
            string s = "";
            for(int i = 0; i < line.length(); i++)
            {
                if(isspace(line[i]))
                {
                    l[j] = s;
                    j++;
                    s = "";
                }
                else
                {
                    s += line[i];
                }
            }
            l[2]=s;

            if(!l[1].empty())
            d.push_back(make_tuple(l[0], l[1], stoi(l[2])));
        }
        fin.close();
        for(auto i: d)
            cout << get<0>(i) << " owes " << get<1>(i) << " $" << get<2>(i) << endl;
        cout << "Would you like to delete these transactions? Press Y to delete and N to keep it"<< endl;
        char c;
        cin >> c;
        /*cout << "Enter number of new transactions:  ";
        cin >> N;
        cout << "\nEnter first transaction in $\n";
        for(int i = 0; i < N; i++)
        {
            cout << "Enter debtor name: ";
            cin >> debtor;
            cout << "Enter creditor name: ";
            cin >> creditor;
            cout << "Enter debt amount: ";
            cin >> amount;
            data.push_back(make_tuple(debtor, creditor, amount));
            cout << "\n\nEnter next transaction in $\n\n";
        }*/

        data.push_back(make_tuple("william", "clare", 250));
        data.push_back(make_tuple("ken", "clare", 200));

        switch(c)
        {
            case 'Y':
                remove(const_cast<char*>(filename.c_str()));
                break;
            case 'N':
                int flag = 0;
                for(auto i: d)
                {
                    for(int j=0; j<data.size();j++)
                    {
                        if(get<0>(i) == get<0>(data[j]) && get<1>(i) == get<1>(data[j]))
                        {
                            flag = 1;
                            get<2>(data[j]) = get<2>(data[j]) + get<2>(i);
                        }
                    }
                    if(flag == 0)
                    {
                        data.push_back(i);
                    }
                }
        }
        break;
    }

    case 2:
        cout << "Enter username:" << endl;
        cin >> user;
        filename = user + ".txt";
         /**
         cout << "Enter number of transactions:  ";
         cin >> N;
         cout << "\nEnter first transaction in $\n";
         for(int i = 0; i < N; i++)
         {
             cout << "Enter debtor name: ";
             cin >> debtor;
             cout << "Enter creditor name: ";
             cin >> creditor;
             cout << "Enter debt amount: ";
             cin >> amount;
             data.push_back(make_tuple(debtor, creditor, amount));
             cout << "\n\nEnter next transaction in $\n\n";
         }
         **/
         data.push_back(make_tuple("joe", "clare", 245));
         data.push_back(make_tuple("joe", "rosh", 345));
         data.push_back(make_tuple("joe", "dash", 3542));
         data.push_back(make_tuple("clare", "rosh", 345));
         data.push_back(make_tuple("clare", "dash", 245));
         data.push_back(make_tuple("ken", "dash", 230));
         data.push_back(make_tuple("ken", "clare", 230));
         data.push_back(make_tuple("dash", "rosh", 542));
         data.push_back(make_tuple("rosh", "ken", 310));
         data.push_back(make_tuple("lily", "rosh", 3542));
         data.push_back(make_tuple("lily", "william", 3542));
         data.push_back(make_tuple("dash", "ken", 3542));
         data.push_back(make_tuple("william", "rosh", 3542));

         break;
    case 3:
        cout << "Thank you for using SPLITCOIN";
        break;

    default:
        cout << "Enter a valid option\n\n";
        goto OPTION1;
    }

    // STORE SIMPLIFIED TRANSACTIONS IN db AND DISPLAY ON CONSOLE
    set<tuple<string, string, int>>finalSet = initialfn(data);
    if(opt == 1)
    {
        ofstream fout;
        fout.open("temp.txt");

        for(auto i:finalSet)
        {
            string str = get<0>(i) + " " + get<1>(i) + " " + to_string(get<2>(i));
            fout << str << endl;
        }

        fout.close();
        remove(const_cast<char*>(filename.c_str()));
        rename("temp.txt", const_cast<char*>(filename.c_str()));
    }
    else if(opt == 2)
    {
        ofstream fout;
        fout.open(filename);

        for(auto i:finalSet)
        {
            string str = get<0>(i) + " " + get<1>(i) + " " + to_string(get<2>(i));
            fout << str << endl;
        }

        fout.close();
    }

    // MAX TRANSACTION
    int maxTransaction = INT_MIN;
    tuple<string, string, int> maxTransactionEdge;
    for(auto transaction:finalSet)
    {
        if(maxTransaction < get<2>(transaction))
        {
            maxTransaction = get<2>(transaction);
            maxTransactionEdge = transaction;

        }
    }
    cout << "Maximum transaction:  " << get<0>(maxTransactionEdge) << " owes " << get<1>(maxTransactionEdge) << " amount " << get<2>(maxTransactionEdge) << "\n\n";

    // display user's transactions
    cout << "Your transactions are:  \n";
    findAllUserTransactions(finalSet);

    return 0;
}
