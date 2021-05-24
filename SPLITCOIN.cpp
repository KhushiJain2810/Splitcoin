#include<iostream>
#include<vector>
#include<map>
#include<queue>
#include<set>

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
            if(level[i.to] < 0 && i.flow < i.capacity)
            {
                level[i.to] = level[u] + 1;
                q.push(i.to);
            }
        }
    }
    return level[sink] != -1;
}

int Graph :: sendFlow(int current, int flow, int sink, vector<int>&start)
{
    if(current == sink)
        return flow;

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

int main()
{
     int N;
     int amount;
     string creditor, debtor;
     vector<tuple<string, string, int>>data;

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

     /*data.push_back(make_tuple("Gabe", "Bob", 30));
     data.push_back(make_tuple("Gabe", "David", 10));
     data.push_back(make_tuple("Fred", "Bob", 10));
     data.push_back(make_tuple("Fred", "Charlie", 30));
     data.push_back(make_tuple("Fred", "David", 10));
     data.push_back(make_tuple("Fred", "Ema", 10));
     data.push_back(make_tuple("Bob", "Charlie", 40));
     data.push_back(make_tuple("Charlie", "David", 20));
     data.push_back(make_tuple("David", "Ema", 50));*/

     /*data.push_back(make_tuple("gargi", "kriti", 2500));
     data.push_back(make_tuple("khushi", "gargi", 2000));
     data.push_back(make_tuple("khushi", "kriti", 1000));*/

     /*data.push_back(make_tuple("ken", "rosh", 120));
     data.push_back(make_tuple("rosh", "joe", 340));
     data.push_back(make_tuple("joe", "clare", 245));
     data.push_back(make_tuple("clare", "rosh", 345));
     data.push_back(make_tuple("ken", "dash", 230));
     data.push_back(make_tuple("dash", "rosh", 542));
     data.push_back(make_tuple("rosh", "ken", 310));
     data.push_back(make_tuple("clare", "dash", 245));
     data.push_back(make_tuple("joe", "rosh", 345));
     data.push_back(make_tuple("ken", "clare", 230));
     data.push_back(make_tuple("lily", "rosh", 3542));*/

     map<string, int>indices;
     int counter = 1;

    for(auto x:data)
    {
        if(!indices[get<0>(x)])
            indices[get<0>(x)] = counter++;
        if(!indices[get<1>(x)])
            indices[get<1>(x)] = counter++;
    }
    Graph g(counter+1);
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
    return 0;
}
