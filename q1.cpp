#include <iostream>
#include <vector>
using namespace std;

int main() {
    int t;
    cin >> t;
    while(t--){
        int n,m;
        cin>>n>>m;
        vector<vector<int>> a(n, vector<int>(m));
        vector<int> colsuma(n,0);
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                int t;
                cin>>t;
                a[i][j]=t;
                colsuma[i] += t;
            }
        }
        vector<vector<int>> b(n, vector<int>(m));
        vector<int> colsumb(n,0);
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                int t;
                cin>>t;
                b[i][j]=t;
                colsumb[i] += t;
            }
        }
        int possible = 1;
        for(int i=0;i<n;i++){
            if(abs(colsuma[i]-colsumb[i])%3!=0){
                possible = 0;
                break;
            }
        }
        if(possible){
            cout<<"YES"<<endl;
        }
        else{
            cout<<"NO"<<endl;
        }
    }
    return 0;
}