/**
 * Copyright 2020. xpino79@gmail.com All rights reserved.
 */

#ifndef XGRID_CONTAINER_H_
#define XGRID_CONTAINER_H_

#include <memory>
#include <mutex>
#include <vector>
#include "xcoordinate.h"

namespace my {

class xtopography
{
public:
    xtopography() {}
    ~xtopography() {}
};

typedef std::vector<std::unique_ptr<xtopography>> xtopography1d;
typedef std::vector<xtopography1d> xtopography2d;
class xgrid
{
private:
    std::int32_t _Myx; 
    std::int32_t _Myy; 
    xtopography2d _Mytopography;
 
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
    xtopography2d &topography() { return _Mytopography; }
    void set_x(std::int32_t _X) { _Myx = _X; }
    void set_y(std::int32_t _Y) { _Myy = _Y; }
};

typedef std::vector<std::unique_ptr<xgrid>> xgrid1d;
typedef std::vector<xgrid1d> xgrid2d;
class xgrid_container
{
private:
    std::unique_ptr<xcoordinate> _Myleft_bottom; 
    std::unique_ptr<xcoordinate> _Myright_top; 

    xgrid2d _Mygrid; 
    std::int32_t _Myinterval_xy; 
    std::int32_t _Mymaximum_cols; 
    std::int32_t _Mymaximum_rows; 
    
public:
    xgrid_container()
    {
        _Myleft_bottom = std::make_unique<xcoordinate>();
        _Myright_top = std::make_unique<xcoordinate>();
        
        _Myinterval_xy = 0;
        _Mymaximum_cols = 0;
        _Mymaximum_rows = 0;

        initialize( 10000, 726029, 1464252, 1383896, 2559450 );
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
        _Mymaximum_cols = ((_Right_top_x - _Left_bottom_x) / _Interval_xy) + 1;
        _Mymaximum_rows = ((_Right_top_y - _Left_bottom_y) / _Interval_xy) + 1;
        
        _Mygrid = xgrid2d(_Mymaximum_cols);
        for (std::int32_t _Idx_x = 0; _Idx_x<_Mymaximum_cols; _Idx_x++)
        {
            _Mygrid[_Idx_x] = xgrid1d(_Mymaximum_rows);
            for (std::int32_t _Idx_y = 0; _Idx_y<_Mymaximum_rows; _Idx_y++)
            {
                _Mygrid[_Idx_x][_Idx_y] = std::make_unique<xgrid>();
                _Mygrid[_Idx_x][_Idx_y]->set_x(_Idx_x);
                _Mygrid[_Idx_x][_Idx_y]->set_y(_Idx_y);

                if ((_Idx_x >= 0) && (_Idx_x < _Mymaximum_rows) &&
                    (_Idx_y >= 0) && (_Idx_y < _Mymaximum_rows))
                {
                    _Mygrid[_Idx_x][_Idx_y]->topography() = xtopography2d(20);
                    for (auto &_Row : _Mygrid[_Idx_x][_Idx_y]->topography())
                    {
                        _Row = xtopography1d(20);
                        for (auto &_Topography : _Row)
                        {
                            _Topography = std::make_unique<xtopography>();
                        }
                    }
                }
            } // for
        }  // for       
    } 
    
};

} /* namespace my */

#endif /* XGRID_CONTAINER_H_ */
