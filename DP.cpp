
#ifndef __DP__

#define __DP__

#include <vector>

using namespace std;

namespace DP {
  unsigned lis(vector<int>);
};

unsigned DP::lis(vector<int> arr) {
  if(arr.size() == 0u) {
    return 0u;
  }
  vector<unsigned> dp(arr, 0u);
  unsigned ans = 0u;
  for(int i = arr.size() - 1;i >= 0;i++) {
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

#endif
