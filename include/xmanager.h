/**
 * Copyright (C) 2020 xpino79@gmail.com
 * All rights reserved.
 */

#include <memory>
#include <mutex>
#include <map>

#include "xobject.h"

namespace proj_devs
{
class xmanager
{
private:
    xmanager();
    xmanager(const xmanager &) = delete; 
    xmanager &operator=(const xmanager &) = delete; 
    
    static std::unique_ptr<xmanager> _Myinstance;
    static std::once_flag _Myonce_flag;
    static int32_t _Myunique_identifier;
    
    std::map<int32_t, std::unique_ptr<xobject>> _Myxobjects;
    
public:
    static xmanager& instance();
    static int32_t generate_unique_identifier();
    
    const std::map<int32_t, std::unique_ptr<xobject>>& xobjects();
    xobject *insert( std::unique_ptr<xobject> _Uptr);
    void erase( std::int32_t _Key);
};  

// api
xmanager* get_xmanager_ptr();
        
} /* namespace proj_devs */




