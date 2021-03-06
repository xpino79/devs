/**
 * Copyright 2020. xpino79@gmail.com All rights reserved.
 */

#ifndef XGRID_CONTAINER_H_
#define XGRID_CONTAINER_H_

#include <memory>
#include <mutex>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <omp.h> // 병렬 

#include "xcoordinate.h"

namespace my {

class xtopography
{
public:
    xtopography() {}
    ~xtopography() {}
};
    
class xgrid
{
private:
    std::int32_t _Myx; 
    std::int32_t _Myy; 
    std::vector<std::vector<std::unique_ptr<xtopography>>> _Mytopography;
    std::map<std::int32_t, std::set<std::int32_t>> _Mydata;
 
public:
    xgrid()
    {
        _Myx = -1; 
        _Myy = -1;
    }
    ~xgrid()
    {
    }
 
    std::int32_t x() { return _Myx; }
    std::int32_t y() { return _Myy; }
    std::vector<std::vector<std::unique_ptr<xtopography>>> &topography() { return _Mytopography; }
    void set_x(std::int32_t _X) { _Myx = _X; }
    void set_y(std::int32_t _Y) { _Myy = _Y; }
    void set_topography(std::int32_t _Width, std::int32_t _Height)
    {
        _Mytopography.resize(_Width);
        for (auto& _Elem : _Mytopography)
        {
            _Elem.resize(_Height);
            for (auto& _Topography : _Elem)
            {
                _Topography = std::make_unique<xtopography>();
            }
        }
    }

    void push_data(std::int32_t _Type, std::int32_t _Handle)
    {
        std::set<std::int32_t> _Handles{_Handle};

        std::map<std::int32_t, std::set<std::int32_t>>::iterator _It;
        bool _Success;

        std::tie(_It, _Success) = _Mydata.insert(std::make_pair(_Type, _Handles));
        if(!(_Success))
        {
            std::tie(std::ignore, _Success) = _It->second.insert(_Handle);
            if(!(_Success))
            {
                /* insert 실패 */
            }
        }
    }
    
    void erase_data(std::int32_t _Type, std::int32_t _Handle)
    {
        auto _It = _Mydata.find(_Type);
        if(_It != _Mydata.end())
        {
            auto _Erase_size = _It->second.erase(_Handle);
            if(0 == _Erase_size)
            {
                /* 삭제 실패 */
            } 
        }
    }
};

class xgrid_container
{
private:
    std::unique_ptr<xcoordinate> _Myleft_bottom; 
    std::unique_ptr<xcoordinate> _Myright_top; 

    std::vector<std::vector<std::unique_ptr<xgrid>>> _Mygrid;
    std::int32_t _Myinterval_xy; 
    std::int32_t _Mymaximum_x; 
    std::int32_t _Mymaximum_y; 
    
public:
    xgrid_container()
    {
        _Myleft_bottom = std::make_unique<xcoordinate>();
        _Myright_top = std::make_unique<xcoordinate>();
        
        _Myinterval_xy = 0;
        _Mymaximum_x = 0;
        _Mymaximum_y = 0;
    }
    
    ~xgrid_container()
    {
    }
    
    void initialize(
        std::int32_t _Interval_xy, 
        std::int32_t _Left_bottom_x, std::int32_t _Left_bottom_y, 
        std::int32_t _Right_top_x, std::int32_t _Right_top_y
    )
    {
        _Myinterval_xy = _Interval_xy;
        _Myleft_bottom->set_x(_Left_bottom_x);
        _Myleft_bottom->set_y(_Left_bottom_y);
        _Myright_top->set_x(_Right_top_x);
        _Myright_top->set_y(_Right_top_y);
        
        _Mymaximum_x = ((_Right_top_x - _Left_bottom_x) / _Interval_xy) + 1;
        _Mymaximum_y = ((_Right_top_y - _Left_bottom_y) / _Interval_xy) + 1;
        
        _Mygrid.clear();
        _Mygrid.resize(_Mymaximum_x);
 
        // >>>>> 병력 처리 속도 테스트 , https://pages.tacc.utexas.edu/~eijkhout/pcse/html/omp-loop.html
        #pragma omp parallel for schedule(static)
        for (std::int32_t _Idx_x = 0; _Idx_x < _Mymaximum_x; ++_Idx_x)
        {
            // std::cout << "#병렬처리 " << _Idx_x << std::endl;
            _Mygrid[_Idx_x].resize(_Mymaximum_y);
            for (std::int32_t _Idx_y = 0; _Idx_y < _Mymaximum_y; ++_Idx_y)
            {
                _Mygrid[_Idx_x][_Idx_y] = std::make_unique<xgrid>();
                _Mygrid[_Idx_x][_Idx_y]->set_x(_Idx_x);
                _Mygrid[_Idx_x][_Idx_y]->set_y(_Idx_y);

                if ((0 <= _Idx_x) && (_Idx_x < _Mymaximum_x) &&
                    (0 <= _Idx_y) && (_Idx_y < _Mymaximum_y))
                {
                    _Mygrid[_Idx_x][_Idx_y]->set_topography(20, 20);
                }
            } // for
        } // for
        
    }

    std::tuple<std::int32_t, std::int32_t> to_grid_index(std::int32_t _Pos_x, std::int32_t _Pos_y)
    {
        xcoordinate _Pos(_Pos_x, _Pos_y);
        if((*(_Myleft_bottom.get()) <= _Pos) && (_Pos <= *(_Myright_top.get())))
        {
            _Pos -= *(_Myleft_bottom.get());
            _Pos /= _Myinterval_xy;
        }
        else
        {
            _Pos.set_x(-1);
            _Pos.set_y(-1);
        }
        return std::tuple<std::int32_t, std::int32_t>(_Pos.x(), _Pos.y());
    }

    xgrid *find_grid(std::int32_t _Pos_x, std::int32_t _Pos_y)
    {
        std::int32_t _Idx_x;
        std::int32_t _Idx_y;

        std::tie(_Idx_x, _Idx_y) = to_grid_index(_Pos_x, _Pos_y);
    
        xgrid *_Ptr = nullptr;
        if((0 <= _Idx_x) && (0 <= _Idx_y))
        {
            _Ptr = _Mygrid[_Idx_x][_Idx_y].get();
        }

        return _Ptr;
    }
    
    std::list<xgrid *> within_grid(std::int32_t _Pos_x, std::int32_t _Pos_y, std::int32_t _Range = 0)
    {
        std::list<xgrid *> _Val;
        
        xgrid *_Off = find_grid( _Pos_x,  _Pos_y);
        if ( nullptr != _Off )
        {
            std::int32_t _Interval = _Range / _Myinterval_xy;
            if(0 == _Interval)
            {
                _Val.push_back(_Off);
            }
            else
            {
                std::int32_t _Begin_x = std::max(_Off->x()-_Interval, 0);
                std::int32_t _Begin_y = std::max(_Off->y()-_Interval, 0);
                std::int32_t _End_x = std::min(_Off->x()+_Interval, _Mymaximum_x-1);
                std::int32_t _End_y = std::min(_Off->y()+_Interval, _Mymaximum_y-1);
 
                // >>>>> 병렬 테스트
                // #pragma omp parallel for schedule(static) collapse(2)
                for (auto _Idx_y = _Begin_y; _Idx_y <= _End_y; ++_Idx_y)
                {
                    for (auto _Idx_x = _Begin_x; _Idx_x <= _End_x; ++_Idx_x)
                    {
                        xgrid *_Ptr = _Mygrid[_Idx_x][_Idx_y].get();
                        if(nullptr != _Ptr)
                        {
                            _Val.push_back( _Ptr );
                        }
                    }
                }
            }
            
        }
        return _Val;
    }

    void push_griddata(std::int32_t _Type, std::int32_t _Handle, std::int32_t _Pos_x, std::int32_t _Pos_y)
    {
        xgrid *_Ptr = find_grid(_Pos_x, _Pos_y);
        if(nullptr != _Ptr)
        {
            _Ptr->push_data(_Type, _Handle);
        }
    }
    
    void erase_griddata(std::int32_t _Type, std::int32_t _Handle, std::int32_t _Pos_x, std::int32_t _Pos_y)
    {
        xgrid *_Ptr = find_grid(_Pos_x, _Pos_y);
        if(nullptr != _Ptr)
        {
            _Ptr->erase_data(_Type, _Handle);
        }
    }
};

} /* namespace my */

#endif /* XGRID_CONTAINER_H_ */
