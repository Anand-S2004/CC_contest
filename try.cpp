#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

typedef int64_t ll;

void solve() {
    ll n;
    cin >> n;
    vector<vector<ll>> a(3, vector<ll>(n));

    // Read input for all three arrays
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> a[i][j];
        }
    }

    ll sum = 0;
    for (ll x : a[0]) {
        sum += x;
    }
    ll target = sum / 3 + (sum % 3 != 0);

    int perm[3] = {0, 1, 2};
    pair<int, int> res[3];

    do {
        int i = 0;
        bool ok = true;
        for (int k = 0; k < 3; ++k) {
            int id = perm[k];
            res[id].first = i + 1;
            ll s = 0;
            while (s < target && i < n) {
                s += a[id][i];
                ++i;
            }
            res[id].second = i;
            if (s < target) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;

        for (int k = 0; k < 3; ++k) {
            cout << res[k].first << " " << res[k].second << " ";
        }
        cout << endl;
        return;

    } while (next_permutation(perm, perm + 3));

    cout << "-1\n";
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}
