#ifndef XTBB_H_
#define XTBB_H_

#include <vector>
#include <map>
#include <chrono>
#include <tbb/tbb.h> // 병렬


// 병렬처리 속도 관련 
// https://docs.microsoft.com/ko-kr/dotnet/standard/parallel-programming/potential-pitfalls-in-data-and-task-parallelism
// 병렬처리 가이드 
// https://www.kdata.or.kr/info/info_04_view.html?field=&keyword=&type=techreport&page=27&dbnum=180834&mode=detail&type=techreport

// TBB 설치방법
// tbb-2018.2-9.el8.x86_64.rpm  tbb-devel-2018.2-9.el8.x86_64.rpm 설치
// Makefile 에 -ltbb 라이브러리 추가 

void _My_tbb_for()
{

    int _Max = 25000;
    int _Sum = 0;
    
    std::chrono::system_clock::time_point _Tbegin;
    std::chrono::system_clock::time_point _Tend;
    std::chrono::duration<double> _Tseconds;
    
    // 병렬처리 25000개 이상의 루프. 그 이하는 속도저하
    tbb::concurrent_unordered_map<int,int> _CUmap;
    for (int i=0;i < _Max;i++)
    {
        _CUmap.insert(tbb::concurrent_unordered_map<int,int>::value_type(i,1));
    }

    _Tbegin = std::chrono::system_clock::now();
    tbb::parallel_for(_CUmap.range(),[](const tbb::concurrent_unordered_map<int,int>::range_type& _Range)
    {
        for (auto _Iter = _Range.begin(); _Iter != _Range.end(); _Iter++)
        {
            // printf("key : %d\t value : %d\n",_Iter->first,_Iter->second);
        }
    }, tbb::auto_partitioner() );

    _Tend = std::chrono::system_clock::now();
    _Tseconds = _Tend-_Tbegin;
    printf("#parallel eslaped time : %f sec\n", _Tseconds.count());
 

    _Tbegin = std::chrono::high_resolution_clock::now();
    for (auto _Iter = _CUmap.begin(); _Iter != _CUmap.end(); _Iter++)
    {
        // printf("key : %d\t value : %d\n",_Iter->first,_Iter->second);
    }

    _Tend = std::chrono::system_clock::now();
    _Tseconds = _Tend-_Tbegin;
    printf("#seq eslaped time : %f sec\n", _Tseconds.count()); 

    // >>>>> std::vector 
    auto _Vec = std::vector<double>(_Max);
    tbb::parallel_for( tbb::blocked_range<int>(0,_Vec.size()),
                       [&](tbb::blocked_range<int> _Range)
    {
        for (int i=_Range.begin(); i<_Range.end(); ++i)
        {
            _Vec[i] = 1;
        }
    });
    
    _Tbegin = std::chrono::system_clock::now();
    tbb::parallel_for( tbb::blocked_range<int>(0,_Vec.size()),
                       [&](tbb::blocked_range<int> _Range)
    {
        for (int i=_Range.begin(); i<_Range.end(); ++i)
        {
             _Sum +=_Vec[i];
        }
    });
    _Tend = std::chrono::system_clock::now();
    _Tseconds = _Tend-_Tbegin;
    printf("#parallel eslaped time : %f sec\n", _Tseconds.count());

    _Sum = 0;
    _Tbegin = std::chrono::system_clock::now(); 
    for (double _Val : _Vec)
    {
        _Sum +=_Val;
    }
    _Tend = std::chrono::system_clock::now();
    _Tseconds = _Tend-_Tbegin;
    printf("#seq eslaped time : %f sec\n", _Tseconds.count());
    
}
 
#endif /* XTBB_H_ */
