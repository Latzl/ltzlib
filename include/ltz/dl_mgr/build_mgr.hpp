/**
  @file build_mgr.hpp
  @version 0.1
  @brief An easy way to load dynamic library.
  @details 
    Usage: 
        1. Define LTZ_DL_MGR_NAME as mgr type name;
        2. Define LTZ_DL_MGR_SEQ, which is a sequence with tuple element. For each tuple, the first element is function type, the second element is function name.
        2.5. Optional, define LTZ_DL_MGR_NAMESPACE_SEQ, which is a sequence of hierarchical namespace.
        3. Include LTZ_DL_MGR_BUILD_MGR()
        4. Instantiate the mgr obj, as the type same as LTZ_DL_MGR_NAME. Call the function that in LTZ_DL_MGR_SEQ
  @throw std::runtime_error if the dynamic library can not be loaded.
  @example 
    #define LTZ_DL_MGR_NAME example_mgr
    #define LTZ_DL_MGR_SEQ ((void (*)(), foo))((int (*)(int,int), bar))
    #define LTZ_DL_MGR_NAMESPACE_SEQ (ns_a)(ns_b)(ns_c)
    #include LTZ_DL_MGR_BUILD_MGR()
    int main(){
        try{
            ns_a::ns_b::ns_c::example_mgr mgr("libexample.so");
            mgr.foo();
            mgr.bar(1, 2);
        }catch(const std::runtime_error& e){
            std::cerr << e.what() << std::endl;
        }
    }
 */
#pragma once

#define LTZ_DL_MGR_BUILD_MGR() <ltz/dl_mgr/detail/build_mgr.hpp>