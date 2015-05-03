
#ifndef __DP__

#define __DP__

#include <vector>

using namespace std;

namespace DP {
  template<typename> unsigned lis(vector<int>);
  template<typename> unsigned lis_nlogn(vector<int>);
};

template<typename Data> unsigned DP::lis(vector<Data> arr) {
  if(arr.size() == 0u) {
    return 0u;
  }
  vector<unsigned> dp(arr.size(), 0u);
  unsigned ans = 0u;
  for(int i = arr.size() - 1;i >= 0;i--) {
    dp[i] = 1u;
    for(int j = i+1;j < arr.size();j++) {
      if(arr[j] > arr[i]) {
        dp[i] = max(dp[i], dp[j] + 1u);
      }
    }
    ans = max(ans, dp[i]);
  }
  return ans;
}
template<typename Data> unsigned DP::lis_nlogn(vector<Data> arr) {
  vector<Data> dp;
  for(Data val : arr) {
    if(dp.size() == 0u) {
      dp.push_back(val);
    }else if(val <= dp[0]) {
      dp[0] = val;
    }else {
      int a = 0, b = dp.size() - 1;
      while(a < b) {
        int mid = (a + b) / 2 + 1;
        if(val >= dp[mid]) {
          a = mid;
        }else {
          b = mid - 1;
        }
      }
      if(a+1 == dp.size()) {
        dp.push_back(val);
      }else {
        dp[a+1] = val;  
      }
    }
  }
  return dp.size();
}
#endif
