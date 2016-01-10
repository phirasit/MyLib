/*
RMQ (Range Minimum Query)

Problem :
input A1, A2, ..., An
each query return the minimum number in range i and j

This algorithm requires O(n) precomputation and memmory
and can answer each query in O(1)

This code is for C++11 or newer version
*/

#ifndef __MYLIB__RMQ__

#define __MYLIB__RMQ__

#include "MyMath.cpp"
#include <vector>
#include <stack>
#include <functional>

#define __RMQ_TYPE_ERROR__ 0
#define __RMQ_TYPE_N__ 1
#define __RMQ_TYPE_NLOGN__ 2
#define __RMQ_TYPE_SLOW__ 3
#define __RMQ_TYPE_SMALL__ 4
#define __RMQ_DEFAULT_ALGO__ __RMQ_TYPE_NLOGN__

using namespace std;

template<class Data> class RMQ {
	
	public:

		int type;
		mutable function<bool (Data, Data)> cmp;
		vector<Data> data; 

		RMQ(vector<Data>, int, function<bool (Data, Data)>);
		RMQ(vector<Data>, function<bool (Data, Data)>);
		RMQ(void);

		int query(int, int);
		Data operator[](int);

	private:
		
		Data error(void);
		void initFast1(void);
		void initFast2(void);
		void initSlow(void);
		void initSmall(void);

		int QueryFast1(int, int);
		int QueryFast2(int, int);
		int QuerySlow(int, int);
		int QuerySmall(int, int);

		// for O(N) algorithm
		int group(int);

		int K;
		RMQ< pair<Data, int> > summary;
		class Cluster {

			public:
				
				void BuiltCartesianTree(void);
				int ComputeHash(void);
				void ComputeRMQ(void);

				vector< vector<int> > rmq;
				vector<Data> list;

				// Euler tour tree
				vector<Data> EulerTour;
				vector<int> Height, FirstPosition;
				
				int hash;

		};

		vector<Cluster> cluster;
		vector< vector< vector<int> > > memmory;
		vector<int> check;

		// for O(N log N) algorithm
		vector< vector<int> > sparse_table;
};

// Initialize constructor and helper function
template <class Data> RMQ<Data>::RMQ(vector<Data> _data, int algorithm = __RMQ_DEFAULT_ALGO__,
	function<bool (Data, Data)> _cmp = [](Data D1, Data D2) { return D1 < D2; }) {
	
	data.assign(_data.begin(), _data.end());
	cmp = _cmp;
	type = algorithm;

	switch(type) {
		case __RMQ_TYPE_N__:
			initFast1();
		case __RMQ_TYPE_NLOGN__:
			initFast2();
			break;
		case __RMQ_TYPE_SLOW__:
			initSlow();
			break;
		case __RMQ_TYPE_SMALL__:
			initSmall();
			break;
		default:
			// error
			break;
	}
}

template <class Data> RMQ<Data>::RMQ(vector<Data> _data, 
	function<bool (Data, Data)> _cmp = [](Data D1, Data D2) { return D1 < D2; }) {
	RMQ(_data, __RMQ_DEFAULT_ALGO__, _cmp);
}

template <class Data> RMQ<Data>::RMQ(void) {
	type = __RMQ_TYPE_ERROR__;
}

// Error function
template<class Data> Data RMQ<Data>::error(void) {
	return Data();
}

// Preprocess functions, the most important and complicated parts of the program

template <class Data> int RMQ<Data>::group(int idx) {
	return idx / K;
}

#include <iostream>
template <class Data> void RMQ<Data>::Cluster::BuiltCartesianTree(void) {
	stack<int> stk, dfs;
	vector<int> Parent(list.size());
	vector< vector<int> > child(list.size(), vector<int>());
	for(int i = 0;i < list.size();i++) {

		while(!stk.empty() and cmp(data[list[i]], data[list[stk.top()]])) {
			Parent[stk.top()] = i;
			stk.pop();
			Parent[i] = (stk.empty() ? -1 : stk.top());
		}
		stk.push(i);
	}
	for(int i = 0;i < list.size();i++) {
		child[Parent[i]].push_back(i);
		FirstPosition[i] = -1;
	}

	dfs.push(0);
	while(!dfs.empty()) {
		int u = dfs.top();
		EulerTour.push_back(u);
		Height.push_back(dfs.size());
		if(FirstPosition[u] == -1) {
			FirstPosition[u] = (int) EulerTour.size() - 1;
		}
		if(child[u].size() == 0) {
			dfs.pop();
		}else {
			dfs.push(child[u].back());
			child[u].pop_back();
		}
	}

	for(int val : list) {
		cout << val << " ";
	}
	cout << endl;
	for(int e : EulerTour) {
		cout << e << " ";
	}
	cout << endl;
	for(int h : Height) {
		cout << h << " ";
	}
	cout << endl;
}
template <class Data> int RMQ<Data>::Cluster::ComputeHash(void) {
	return 0;	
}
template <class Data> void RMQ<Data>::Cluster::ComputeRMQ(void) {
	
}

template <class Data> void RMQ<Data>::initFast1(void) {	

	if(data.empty()) return;

	// set parameter
	K = MyMath::log2(data.size()) >> 2;

	if(K == 0) {
		RMQ(data, cmp, __RMQ_TYPE_NLOGN__);
		return;
	}

	// summary 
	vector< pair<Data, int> > candidate(group(data.size()-1) + 1);
	for(int i = 0;i < data.size();i++) {
		int g = group(i);
		if(i % K == 0 or cmp(data[i], candidate[g].first)) {
			candidate[g] = {data[i], i};
		}
	}
	summary = RMQ< pair<Data, int> >(candidate, __RMQ_TYPE_NLOGN__, 
		[=](pair<Data, int> p1, pair<Data, int> p2) {
			if(cmp(p1.first, p2.first) or cmp(p2.first, p1.first)) return cmp(p1.first, p2.first);
			return p1.second < p2.second;
		});

	// cluster
	cluster.resize(group(data.size()-1));
	memmory.resize(1 << (K << 1), vector< vector<int> > (K, vector<int>(K)));
	check.resize(memmory.size(), 0);
	for(auto& val : check) val = 0;
	for(int i = 0;i < data.size();i += K) {
		vector<int> V;
		for(int j = 0;i+j < data.size() and j < K;j++) {
			V.push_back(i+j);
		}
		int g = group(i);
		cluster[g].list = V;
		cluster[g].BuiltCartesianTree();
		if(cluster[g].size() == K and check[cluster[g].ComputeHash()] == 0) {
			check[cluster[g].hash] = 1;
			cluster.rmq = memmory[cluster[g].hash];
		}else {
			cluster[g].ComputeRMQ();
			memmory[cluster[g].hash] = cluster[g].rmq;
		}
	}
}

template<class Data> void RMQ<Data>::initFast2(void) {
	sparse_table.resize(MyMath::log2((int) data.size()) + 1, vector<int>(data.size(), 0));
	for(int i = 0;i < sparse_table.size();i++) {
		for(int j = 0;j < sparse_table[i].size();j++) {
			if(i == 0) {
				sparse_table[i][j] = j;
			}else {
				int nxt = min((int) data.size()-1, j + (1 << (i-1)));
				sparse_table[i][j] = !cmp(data[sparse_table[i-1][nxt]], data[sparse_table[i-1][j]]) ? sparse_table[i-1][j] : sparse_table[i-1][nxt];
			} 
		}
	}
}

template<class Data> void RMQ<Data>::initSlow(void) {
	// do nothing
}

// These parts are query functions of each algorithm

template<class Data> int RMQ<Data>::QueryFast1(int idx1, int idx2) {
	int g1 = group(idx1), g2 = group(idx2);
	if(g1+1 <= g2-1) {
		int ans = idx1;
		pair<Data, int> C1 = summary.query(g1+1, g2-1);
		int id2 = cluster[g1].rmq[idx1 % K][K-1];
		int id3 = cluster[g2].rmq[0][idx2 % K];
		if(cmp(C1.first, data[ans])) {
			ans = C1.second;
		}
		if(cmp(data[id2], data[ans])) {
			ans = id2;
		}
		if(cmp(data[id3], data[ans])) {
			ans = id3;
		}
		return ans;
	}else if(g1 < g2) {
		int ans = idx1;
		int id2 = cluster[g1].rmq[idx1 % K][K-1];
		int id3 = cluster[g2].rmq[0][idx2 % K];
		if(cmp(data[id2], data[ans])) {
			ans = id2;
		}
		if(cmp(data[id3], data[ans])) {
			ans = id3;
		}
		return ans;
	}else {
		return cluster[g1].rmq[idx1 % K][idx2 % K];
	}
}

template<class Data> int RMQ<Data>::QueryFast2(int idx1, int idx2) {
	int len = idx2 - idx1 + 1;
	int x = MyMath::log2(len);
	int id1 = sparse_table[x][idx1];
	int id2 = sparse_table[x][idx2 - (1 << x) + 1];
	return !cmp(data[id2], data[id1]) ? id1 : id2;
}

template<class Data> int RMQ<Data>::QuerySlow(int idx1, int idx2) {
	int ans = idx1;
	for(int i = idx1+1;i <= idx2;i++) {
		ans = cmp(data[ans], data[i]) ? ans : i; 
	}
	return ans;
}

template<class Data> int RMQ<Data>::query(int idx1, int idx2) {
	if(idx1 > idx2) swap(idx1, idx2);
	if(idx1 < 0 or idx2 >= data.size()) return error();
	switch(type) {
		case __RMQ_TYPE_N__ :
			return QueryFast1();
		case __RMQ_TYPE_NLOGN__:
			return QueryFast2(idx1, idx2);
		case __RMQ_TYPE_SLOW__ :
			return QuerySlow(idx1, idx2);
		case __RMQ_TYPE_SMALL__:
			return QuerySmall(idx1, idx2);
		default :
			return error();
	}
	return error();
}

template<class Data> Data RMQ<Data>::operator[](int idx) {
	return data[idx];
}

#endif

#include <iostream>

int main(int argc, char const *argv[])
{
	RMQ<int> R({4, 5, 2, 4, 3}, __RMQ_TYPE_NLOGN__);
	for(int i = 0;i < R.data.size();i++) {
		for(int j = i;j < R.data.size();j++) {
			cout << i << " " << j << " " << " : " << R[R.query(i, j)] << endl;
		}
	}
	return 0;
}