#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int T;
    cin >> T;
    
    while (T--) {
        int n, m;
        cin >> n >> m;
        
        string s;
        cin >> s;
        
        vector<int> ind(m);
        for (int i = 0; i < m; ++i) {
            cin >> ind[i];
        }
        
        string c;
        cin >> c;
        sort(c.begin(), c.end()); // Sort string c lexicographically
        
        // Sort and remove duplicates from ind
        sort(ind.begin(), ind.end());
        ind.erase(unique(ind.begin(), ind.end()), ind.end());//iterator that goes from start to end of all unique eles, and then ind.end removes all ele from that
        
        m = ind.size(); // Update m after removing duplicates
        
        // Update string s based on sorted ind and sorted string c
        for (int i = 0; i < m; ++i) {
            s[ind[i] - 1] = c[i];
        }
        
        cout << s << endl;
    }
    
    return 0;
}
