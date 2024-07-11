#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    int t;
    cin >> t;

    while (t--) {
        int n, m, k;
        cin >> n >> m >> k;
        vector<vector<int>> matrix(n + 1, vector<int>(m + 1));
        vector<vector<int>> prefix_sum(n + 1, vector<int>(m + 1, 0));
        vector<vector<bool>> binary_matrix(n + 1, vector<bool>(m + 1));
        
        int total_sum = 0;
        int gcd_value = 0;

        // Read matrix values
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                cin >> matrix[i][j];
            }
        }

        // Read binary values and calculate prefix sums
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                char ch;
                cin >> ch;
                int value = (ch == '1' ? matrix[i][j] : -matrix[i][j]);//value is positive or negative depending on presence of snowy peak
                total_sum += value;//current diffference
                prefix_sum[i][j] = prefix_sum[i-1][j] + prefix_sum[i][j-1] - prefix_sum[i-1][j-1] + (ch == '1' ? 1 : -1);
            }
        }

        // Calculate the greatest common divisor for submatrices
        for (int i = 1; i <= n - k + 1; i++) {
            for (int j = 1; j <= m - k + 1; j++) {
                int submatrix_sum = abs(prefix_sum[i+k-1][j+k-1] - prefix_sum[i-1][j+k-1] - prefix_sum[i+k-1][j-1] + prefix_sum[i-1][j-1]);
                gcd_value = __gcd(gcd_value, submatrix_sum);
            }
        }

        // Output result based on the gcd and total_sum
        if ((gcd_value && !(total_sum % gcd_value)) || !total_sum) {
            cout << "YES\n";
        } else {
            cout << "NO\n";
        }
    }

    return 0;
}
