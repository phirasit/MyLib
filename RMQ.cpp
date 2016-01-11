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
#define __RMQ_DEFAULT_ALGO__ __RMQ_TYPE_NLOGN__

using namespace std;

template<class Data> class RMQ {
	
	public:

		int type;
		mutable function<bool (Data, Data)> cmp;
		vector<Data> data; 

	public: 

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

		int QueryFast1(int, int);
		int QueryFast2(int, int);
		int QuerySlow(int, int);

		// for O(N) algorithm
		int group(int);

		int K;
		vector<int> candidate;
		vector<int> candidate_value;
		RMQ<Data>* summary;

		class Cluster {

			public:
				
				void BuiltCartesianTree(void);
				int ComputeHash(void);
				void ComputeRMQ(void);
				int query(int, int);

				mutable function<bool (Data, Data)> cmp;
				vector<Data> data;
				vector< vector<int> > rmq;
				vector<int> list;

				// Euler tour tree
				vector<int> EulerTour, Height, FirstPosition;
				
				int hash;

				Cluster(vector<Data> _data, function<bool (Data, Data)> _cmp) : data(_data), cmp(_cmp) {}
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

template <class Data> void RMQ<Data>::Cluster::BuiltCartesianTree(void) {
	stack<int> stk, dfs;
	vector<int> Parent(list.size(), -1);
	vector< vector<int> > child(list.size(), vector<int>());

	EulerTour.clear();
	Height.clear();
	FirstPosition.clear();
	FirstPosition.resize(list.size(), -1);

	for(int i = 0;i < list.size();i++) {
		Parent[i] = (stk.empty() ? -1 : stk.top());
		while(!stk.empty() and cmp(data[list[i]], data[list[stk.top()]]) ) {
			Parent[stk.top()] = i;
			stk.pop();
			Parent[i] = (stk.empty() ? -1 : stk.top());
		}
		stk.push(i);
	}
	int root = 0;
	for(int i = list.size()-1;i >= 0;i--) {
		if(Parent[i] != -1) {
			child[Parent[i]].push_back(i);
		}else {
			root = i;
		}
	}

	dfs.push(root);
	while(!dfs.empty()) {
		int u = dfs.top();
		EulerTour.push_back(u);
		Height.push_back(dfs.size());
		if(FirstPosition[u] == -1) {
			FirstPosition[u] = (int) EulerTour.size() - 1;
		}
		if(child[u].size() == 1 and child[u][0] > u) {
			EulerTour.push_back(-1);
			Height.push_back(list.size() + 1);
		}
		if(child[u].size() == 0) {
			dfs.pop();
		}else {
			dfs.push(child[u].back());
			child[u].pop_back();
		}
	}
}
template <class Data> int RMQ<Data>::Cluster::ComputeHash(void) {
	hash = 1 << Height.size();
	for(int i = 0;i+1 < Height.size();i++) {
		hash ^= (Height[i] < Height[i+1]) << i;
	}
	return hash;
}
template <class Data> void RMQ<Data>::Cluster::ComputeRMQ(void) {
	rmq.clear();
	rmq.resize(EulerTour.size(), vector<int>(EulerTour.size()));
	for(int i = 0;i < rmq.size();i++) {
		for(int j = i;j < rmq[i].size();j++) {
			if(EulerTour[j] == -1) {
				rmq[i][j] = (i == j ? -1 : rmq[i][j-1]);
			}else if(rmq[i][j-1] == -1) {
				rmq[i][j] = EulerTour[j];
			}else {
				rmq[i][j] = (cmp(data[list[EulerTour[j]]], data[list[rmq[i][j-1]]]) ? EulerTour[j] : rmq[i][j-1]);
			}
		}
	}
}
template <class Data> int RMQ<Data>::Cluster::query(int idx1, int idx2) {
	if(FirstPosition[idx1] > FirstPosition[idx2]) swap(idx1, idx2);
	return list[rmq[FirstPosition[idx1]][FirstPosition[idx2]]];
}
template <class Data> void RMQ<Data>::initFast1(void) {	

	if(data.empty()) return;

	// set parameter
	K = MyMath::log2( (int) data.size()) >> 3;

	if(K == 0) {
		K = 1;
	}

	// summary 
	for(int i = 0;i < data.size();i++) {
		int g = group(i);
		if(i % K == 0) candidate.push_back(i);
		else if(cmp(data[i], data[candidate[g]])) {
			candidate[g] = i;
		}
	}
	for(int id : candidate) candidate_value.push_back(data[id]);
	summary = new RMQ<Data>(candidate_value, __RMQ_TYPE_NLOGN__, cmp);

	// cluster
	cluster.clear();
	cluster.resize(group(data.size()-1) + 1, Cluster(data, cmp));
	memmory.clear();
	memmory.resize(1 << (K << 2));
	check.clear();
	check.resize(memmory.size(), 0);
	for(int i = 0;i < data.size();i += K) {
		vector<int> V;
		for(int j = 0;i+j < data.size() and j < K;j++) {
			V.push_back(i+j);
		}
		int g = group(i);
		cluster[g].list = V;
		cluster[g].BuiltCartesianTree();
		if(cluster[g].list.size() == K and check[cluster[g].ComputeHash()]) {
			check[cluster[g].hash] = 1;
			cluster[g].rmq = memmory[cluster[g].hash];
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
	if(g1 < g2) {
		int ans = idx1;
		if(g1 + 1 <= g2 - 1) {
			int id1 = candidate[summary->query(g1+1, g2-1)];
			if(cmp(data[id1], data[ans])) {
				ans = id1;
			}
		}
		int id2 = cluster[g1].query(idx1 % K, K-1);
		int id3 = cluster[g2].query(0, idx2 % K);
		if(cmp(data[id2], data[ans])) {
			ans = id2;
		}
		if(cmp(data[id3], data[ans])) {
			ans = id3;
		}
		return ans;
	}else {
		return cluster[g1].query(idx1 % K, idx2 % K);
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
	if(idx1 < 0) idx1 = 0;
	if(idx2 >= data.size()) idx2 = (int) data.size() - 1;
	switch(type) {
		case __RMQ_TYPE_N__ :
			return QueryFast1(idx1, idx2);
		case __RMQ_TYPE_NLOGN__:
			return QueryFast2(idx1, idx2);
		case __RMQ_TYPE_SLOW__ :
			return QuerySlow(idx1, idx2);
	}
}

template<class Data> Data RMQ<Data>::operator[](int idx) {
	return data[idx];
}

#endif
