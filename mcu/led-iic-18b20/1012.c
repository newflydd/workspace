#include <stdio.h>
#include <stdlib.h>

char *A, *B;

int main()
{
    unsigned short n,m,K;
    scanf("%hd %hd %hd", &n, &m, &K);
    int *f,*temp, s[n][m];
    A = (char*)malloc( (n+1) * sizeof(n));
    B = (char*)malloc( (m+1) * sizeof(m));
    scanf("%s", A);
    scanf("%s", B);

    /*
     * ACM比赛中，1s对应的计算量大约是4*10^8
     * 因此O(n*m*K) = 1000 * 200 * 1000是可以接受的
     */
    unsigned short k,i,j;
    for(k = 1; k<=K; k++){
        f = (int*)malloc(n * m * sizeof(int));
        for(i = 0; i<n; i++)
            for(j = k-1; j<=i && j<m; j++){

                if(i < j || j < (k-1)){
                    f[i*m + j] = 0;
                    s[i][j] = 0;
                    continue;
                }

                if(j == 0 && k ==1){
                    s[i][j] = A[i] == B[j] ? 1 : 0;
                    f[i * m + j] = (long long)(s[i][j] + ((k <= 0 || i<1 || j<0 || i<j || j<(k-1))?0:f[(i-1)*m + j])) % 1000000007;
                    continue;
                }

                if(A[i] == B[j]){
                    if(i>0 && j>0){
                        if(A[i-1] == B[j-1])
                            s[i][j] = (long long)((j<k?0:s[i-1][j-1]) + ((k<=1)?0:temp[(i-1)*m + j-1])) % 1000000007;
                        else
                            s[i][j] = (k<=1)?0:temp[(i-1) * m + j - 1];
                    }
                    f[i * m + j] = (long long)(s[i][j] + ((k <= 0 || i<1 || j<0 || i<=j || j<(k-1))?0:f[(i-1) * m + j]) ) % 1000000007;
                }else{
                    f[i * m + j] = i==0?0:f[(i - 1) * m + j];
                }
            }
        free(temp);
        temp = f;
    }

    printf("%d\n", f[n * m - 1]);

    return 0;
}

