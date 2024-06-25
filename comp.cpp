#include <iostream>
#include <algorithm>
using namespace std;

int main() {
    int t;
    cin >> t;
    while (t > 0) {
        t--;
        int arr[3];
        cin >> arr[0] >> arr[1] >> arr[2];
        sort(arr, arr + 3); // Correct the sort call to use range [arr, arr + 3]
        int a = arr[2] - arr[0];
        cout << a << endl; // Add newline character for better output formatting
    }
    return 0;
}
