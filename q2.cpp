#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int t;
    cin >> t;

    while (t--) {
        int n, m;
        cin >> n >> m;

        vector<vector<int>> a(n, vector<int>(m));//creates a 2d matrix of size n*m
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                cin >> a[i][j];
            }
        }

        vector<int> k(m, 0); // Equivalent to [0]*m in Python
        vector<vector<int>> new_a(n, vector<int>(m)); // New matrix to store the results

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                int x = a[i][j];
                int left = (j > 0) ? a[i][j - 1] : 0;
                int right = (j < m - 1) ? a[i][j + 1] : 0;
                int up = (i > 0) ? a[i - 1][j] : 0;
                int down = (i < n - 1) ? a[i + 1][j] : 0;

                int max_neighbour = max({left, right, up, down});
                new_a[i][j] = min(x, max_neighbour);
            }
        }

        for (const auto& row : new_a) {
            for (const auto& elem : row) {
                cout << elem << " ";
            }
            cout << endl;
        }
    }

    return 0;
}
