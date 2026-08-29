// bench.cpp - comprehensive lxySort vs std::sort benchmark
// UTF-8 源码(中文)。注意: exe 输出为 UTF-8, 中文控制台请 chcp 65001
// Usage: bench [N]      N = array size, default 100000
// Build: g++ -O2 -std=c++17 -fopenmp bench.cpp -o bench

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
struct Clock { static double ms() {
    static LARGE_INTEGER f; static bool init=false;
    if(!init){ QueryPerformanceFrequency(&f); init=true; }
    LARGE_INTEGER c; QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)f.QuadPart;
}};
#else
#include <chrono>
struct Clock { static double ms() {
    return (double)duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count()/1e6;
}};
#endif

#include <algorithm>
#include <vector>
#include <string>
#include <random>
#include <cstdio>
#include <cstdlib>
#include <functional>
using namespace std;

#include "lxy_sort.hpp"

static mt19937 rng(20240);

// ============ timing (best-of-N) ============
template<typename T>
static double timeLxy(vector<T>& v, const char** alg=nullptr) {
    int reps = v.size()<1000 ? 400 : 12;
    bool tiny = v.size()<1000;
    double best=1e300, sum=0;
    for (int r=0;r<reps;r++){ vector<T> c=v; const char* a=nullptr;
        double s=Clock::ms(); lxySortTrace(c,&a); double e=Clock::ms();
        double t=e-s; if (t<best) best=t; if (tiny) sum+=t;
        if (alg && r==0) *alg=a;
    }
    return tiny ? sum/reps : best;
}
template<typename T>
static double timeStd(vector<T>& v) {
    int reps = v.size()<1000 ? 400 : 12;
    bool tiny = v.size()<1000;
    double best=1e300, sum=0;
    for (int r=0;r<reps;r++){ vector<T> c=v;
        double s=Clock::ms(); sort(c.begin(),c.end()); double e=Clock::ms();
        double t=e-s; if (t<best) best=t; if (tiny) sum+=t; }
    return tiny ? sum/reps : best;
}
template<typename T, typename Cmp>
static double timeStdCmp(vector<T>& v, Cmp cmp) {
    int reps = v.size()<1000 ? 400 : 12;
    bool tiny = v.size()<1000;
    double best=1e300, sum=0;
    for (int r=0;r<reps;r++){ vector<T> c=v;
        double s=Clock::ms(); sort(c.begin(),c.end(),cmp); double e=Clock::ms();
        double t=e-s; if (t<best) best=t; if (tiny) sum+=t; }
    return tiny ? sum/reps : best;
}

static int gWins=0, gTotal=0;

template<typename T>
static void runCase(const char* name, vector<T> v) {
    vector<T> ref=v; sort(ref.begin(), ref.end());
    const char* alg=nullptr;
    double t1=timeLxy(v,&alg);
    double t2=timeStd(v);
    vector<T> ck=v; lxySort(ck); bool ok=(ck==ref);
    bool win=t1<t2; gWins+=win; gTotal++;
    printf("[%s]\n", name);
    printf("  lxySort  : %10.6fms  [%s]\n", t1, alg?alg:"-");
    printf("  std::sort: %10.6fms\n", t2);
    printf("  -> %s  |  %.2fx %s\n", ok?"OK":"WRONG",
        (t1>0&&t2>0)?(win?(double)t2/t1:(double)t1/t2):0,
        win?"faster":"slower");
    printf("\n");
}
template<typename T, typename Cmp>
static void runCaseCmp(const char* name, vector<T> v, Cmp cmp) {
    vector<T> ref=v; sort(ref.begin(),ref.end(),cmp);
    vector<T> ck=v; lxySort(ck,cmp); bool ok=(ck==ref);
    double t1=timeLxy(v);
    double t2=timeStdCmp(v,cmp);
    bool win=t1<t2; gWins+=win; gTotal++;
    printf("[%s] (descending)\n", name);
    printf("  lxySort  : %8.3fms\n", t1);
    printf("  std::sort: %8.3fms\n", t2);
    printf("  -> %s  |  %.2fx %s\n\n", ok?"OK":"WRONG",
        (t1>0&&t2>0)?(win?(double)t2/t1:(double)t1/t2):0, win?"faster":"slower");
}

int main(int argc, char** argv) {
    int N = (argc > 1) ? atoi(argv[1]) : 100000;
    printf("=== lxySort  vs  std::sort  (N=%d) ===\n\n", N);

    // ================= int =================
    printf("---------- int ----------\n\n");
    auto R_wide  =[&](int n){vector<int>a(n);for(auto&x:a)x=(int)(rng()%1000000000);return a;};       // 0..1e9
    auto R_999999=[&](int n){vector<int>a(n);for(auto&x:a)x=rng()%1000000;return a;};                 // 0..999999
    auto R_999   =[&](int n){vector<int>a(n);for(auto&x:a)x=rng()%1000;return a;};                    // 0..999
    auto R_99    =[&](int n){vector<int>a(n);for(auto&x:a)x=rng()%100;return a;};                     // 0..99
    auto R_9     =[&](int n){vector<int>a(n);for(auto&x:a)x=rng()%10;return a;};                      // 0..9 many dup
    auto R_neg   =[&](int n){vector<int>a(n);for(auto&x:a)x=(int)(rng()%200000-100000);return a;};    // -100k..100k
    auto R_mix   =[&](int n){vector<int>a(n);for(auto&x:a)x=(int)(rng()%2000000000u-1000000000);return a;}; // mixed signs
    auto ASC     =[](int n){vector<int>a(n);for(int i=0;i<n;i++)a[i]=i;return a;};                    // ascending
    auto ASCrep  =[&](int n){vector<int>a(n);for(int i=0;i<n;i++)a[i]=i/4;return a;};                 // ascending+repeats
    auto DESC    =[](int n){vector<int>a(n);for(int i=0;i<n;i++)a[i]=n-i;return a;};                  // descending
    auto DESCrep =[&](int n){vector<int>a(n);for(int i=0;i<n;i++)a[i]=(n-i)/4;return a;};             // descending+repeats
    auto SAME    =[](int n){return vector<int>(n,42);};                                               // all same
    auto near10  =[&](int n){vector<int>a(n);for(int i=0;i<n;i++)a[i]=i;for(int i=0;i<10;i++){int p=rng()%n,q=rng()%n;swap(a[p],a[q]);}return a;};
    auto near200 =[&](int n){vector<int>a(n);for(int i=0;i<n;i++)a[i]=i;for(int i=0;i<200;i++){int p=rng()%n,q=rng()%n;swap(a[p],a[q]);}return a;};
    auto nearWide=[&](int n){vector<int>a(n);for(auto&x:a)x=(int)(rng()%2000000000u-1000000000);sort(a.begin(),a.end());for(int i=0;i<16;i++){int p=rng()%n,q=rng()%n;swap(a[p],a[q]);}return a;};
    auto SNAKE   =[](int n){vector<int>a(n);for(int i=0;i<n;i++)a[i]=(i%2==0)?(i/2+1):(n-i/2);return a;};
    auto ZIGZAG  =[&](int n){vector<int>a(n);for(int i=0;i<n;i++)a[i]=(rng()%2)?i:(n-i);return a;};
    auto rotSort =[&](int n){vector<int>a(n);for(int i=0;i<n;i++)a[i]=i;int k=rng()%n;rotate(a.begin(),a.begin()+k,a.end());return a;}; // rotated sorted
    auto fewdist =[&](int n){vector<int>a(n);for(auto&x:a)x=rng()%3;return a;};                       // only 3 distinct
    auto perm    =[&](int n){vector<int>a(n);for(int i=0;i<n;i++)a[i]=i;shuffle(a.begin(),a.end(),rng);return a;}; // permutation 1..n
    auto tail10  =[&](int n){vector<int>a(n);for(int i=0;i<n*0.9;i++)a[i]=i;for(int i=(int)(n*0.9);i<n;i++)a[i]=(int)(rng()%1000000);return a;}; // 90% sorted + random tail
    auto halfRand=[&](int n){vector<int>a(n);for(int i=0;i<n/2;i++)a[i]=i;for(int i=n/2;i<n;i++)a[i]=(int)(rng()%1000000);return a;}; // half sorted + random

    runCase("Random 0..1e9",          R_wide(N));
    runCase("Random 0..999999",       R_999999(N));
    runCase("Random 0..999",          R_999(N));
    runCase("Random 0..99",           R_99(N));
    runCase("Duplicates 0..9",        R_9(N));
    runCase("Negative -100k..100k",   R_neg(N));
    runCase("Mixed signs wide",       R_mix(N));
    runCase("Ascending",              ASC(N));
    runCase("Ascending + repeats",    ASCrep(N));
    runCase("Descending",             DESC(N));
    runCase("Descending + repeats",   DESCrep(N));
    runCase("All same",               SAME(N));
    runCase("Nearly sorted (10 swaps)", near10(N));
    runCase("Nearly sorted (200 swaps)", near200(N));
    runCase("Nearly sorted wide-range", nearWide(N));
    runCase("Snake pattern",          SNAKE(N));
    runCase("Zigzag peak-valley",     ZIGZAG(N));
    runCase("Rotated sorted",         rotSort(N));
    runCase("Few distinct (0..2)",    fewdist(N));
    runCase("Random permutation 1..n",perm(N));
    runCase("90% sorted + random tail",tail10(N));
    runCase("Half sorted + random",   halfRand(N));
    runCaseCmp("Descending (comparator)", ASC(N), greater<int>());

    // ================= double =================
    printf("---------- double ----------\n\n");
    auto D_wide  =[&](int n){vector<double>a(n);for(auto&x:a)x=(double)((long long)(rng()%2000000000u)-1000000000)/3.7;return a;};
    auto D_small =[&](int n){vector<double>a(n);for(auto&x:a)x=(double)(rng()%1000);return a;};
    auto D_asc   =[](int n){vector<double>a(n);for(int i=0;i<n;i++)a[i]=i;return a;};
    auto D_desc  =[](int n){vector<double>a(n);for(int i=0;i<n;i++)a[i]=n-i;return a;};
    auto D_near  =[&](int n){vector<double>a(n);for(int i=0;i<n;i++)a[i]=i;for(int i=0;i<16;i++){int p=rng()%n,q=rng()%n;swap(a[p],a[q]);}return a;};
    auto D_same  =[](int n){return vector<double>(n,3.14);};
    auto D_mix   =[&](int n){vector<double>a(n);for(auto&x:a)x=(double)((int)(rng()%2000000)-1000000)/2.5;return a;};
    runCase("double Random wide",  D_wide(N));
    runCase("double Random 0..999",D_small(N));
    runCase("double Ascending",    D_asc(N));
    runCase("double Descending",   D_desc(N));
    runCase("double Nearly sorted",D_near(N));
    runCase("double All same",     D_same(N));
    runCase("double Mixed signs",  D_mix(N));

    // ================= string (stable mode) =================
    printf("---------- string (stable mode) ----------\n\n");
    auto S_random=[&](int n){vector<string>a(n);for(auto&x:a){int L=rng()%8+1;x.reserve(L);for(int j=0;j<L;j++)x+=(char)('a'+rng()%26);}return a;};
    auto S_asc   =[&](int n){vector<string>a(n);for(int i=0;i<n;i++){char b[16];snprintf(b,16,"%012d",i);a[i]=b;}return a;};
    auto S_desc  =[&](int n){vector<string>a(n);for(int i=0;i<n;i++){char b[16];snprintf(b,16,"%012d",n-i);a[i]=b;}return a;};
    auto S_near  =[&](int n){vector<string>a(n);for(int i=0;i<n;i++){char b[16];snprintf(b,16,"%012d",i);a[i]=b;}for(int i=0;i<16;i++){int p=rng()%n,q=rng()%n;swap(a[p],a[q]);}return a;};
    auto S_few   =[&](int n){vector<string>a(n);for(auto&x:a)x="s"+to_string(rng()%8);return a;};
    runCase("string Random",   S_random(N));
    runCase("string Ascending",S_asc(N));
    runCase("string Descending",S_desc(N));
    runCase("string Nearly sorted",S_near(N));
    runCase("string Few distinct",S_few(N));

    // ================= different sizes =================
    printf("---------- 不同规模 (Random 0..999999) ----------\n\n");
    for (int n : {100, 1000, 10000, 100000, 1000000}) {
        runCase(("N="+to_string(n)).c_str(), R_999999(n));
    }

    // ================= parallel vs serial vs std =================
    printf("---------- 并行排序 (lxySortParallel, 比较型/CPU密集) ----------\n\n");
    for (int n : {1000000, 3000000}) {
        struct S { int a,b; };
        vector<S> v(n); for(auto&x:v){x.a=(int)(rng()%100000); x.b=(int)(rng()%1000);}
        auto cmp = [](const S&x,const S&y){return x.a<y.a;};
        double seq=0, par=0, st=0;
        { double b=1e300;for(int r=0;r<5;r++){auto c=v;double s=Clock::ms();lxySort(c,cmp);double e=Clock::ms();if(e-s<b)b=e-s;}seq=b; }
        { double b=1e300;for(int r=0;r<5;r++){auto c=v;double s=Clock::ms();lxySortParallel(c,cmp);double e=Clock::ms();if(e-s<b)b=e-s;}par=b; }
        { double b=1e300;for(int r=0;r<5;r++){auto c=v;double s=Clock::ms();sort(c.begin(),c.end(),cmp);double e=Clock::ms();if(e-s<b)b=e-s;}st=b; }
        printf("[Struct sort(by a) N=%d]\n", n);
        printf("  lxySort(串行)   : %8.3fms\n", seq);
        printf("  lxySortParallel : %8.3fms   -> %.2fx vs 串行\n", par, seq/par);
        printf("  std::sort       : %8.3fms   -> lxyParallel %.2fx faster\n\n", st, st/par);
    }
    printf("  注: 算术 radix / 计数 / string(MSD radix) 为内存带宽瓶颈, 并行无增益;\n");
    printf("      并行主要利好 CPU 密集的比较型/自定义比较器路径 (如上方 struct)。\n\n");

    printf("=== Summary: lxySort won %d/%d cases ===\n", gWins, gTotal);
    return 0;
}
