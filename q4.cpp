

    #include<bits/stdc++.h>
    using namespace std;
    int main()
    {
     int t,x,n,m,fans,ans;
     string s,z;
     cin>>t;
     while(t--)
     {
      cin>>n>>s;
      fans=900;
      for(int i=0;i<n-1;i++)
      {
       bool ok=0;
       vector<int>v;
       for(int j=0;j<n;j++)
       {
        if(j==i){x=(s[j]-'0')*10+(s[j+1]-'0');j++;}
        else     x=s[j]-'0';
        v.push_back(x);
       }
       ans=0;
       for(auto u:v)
       {
        if(u==0 && n>2){ans=0;ok=0; break;}
        if(u>1)ans+=u;
        if(u==1)ok=1;
       }
       if(ok)ans=max(ans,1);
       fans=min(ans,fans);
      }
      cout<<fans<<endl;
     }
    }