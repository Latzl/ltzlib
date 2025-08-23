#include <dlfcn.h>
#include <string>
#include <boost/preprocessor.hpp>

#if defined(LTZ_DL_MGR_NAME) && defined(LTZ_DL_MGR_SEQ)

// utils macro
#define _LTZ_DL_MGR_EXTRACT_SEQ_IN_TUPLESEQ_MACRO(r, data, elem) (BOOST_PP_TUPLE_ELEM(data, elem))
#define LTZ_DL_MGR_EXTRACT_SEQ_IN_TUPLESEQ(idx, seq) \
    BOOST_PP_SEQ_FOR_EACH(_LTZ_DL_MGR_EXTRACT_SEQ_IN_TUPLESEQ_MACRO, idx, seq)

#define LTZ_DL_MGR_FN_NUM BOOST_PP_SEQ_SIZE(LTZ_DL_MGR_SEQ)
#define LTZ_DL_MGR_FNT_SEQ LTZ_DL_MGR_EXTRACT_SEQ_IN_TUPLESEQ(0, LTZ_DL_MGR_SEQ)  // function type
#define LTZ_DL_MGR_FN_SEQ LTZ_DL_MGR_EXTRACT_SEQ_IN_TUPLESEQ(1, LTZ_DL_MGR_SEQ)

#define LTZ_DL_MGR_GEN_CLASS_NAME_PREFIX BOOST_PP_CAT(_ltz_dl_mgr_, BOOST_PP_CAT(LTZ_DL_MGR_NAME, _))
#define LTZ_DL_MGR_GEN_CLASS_NAME(n) \
    BOOST_PP_CAT(LTZ_DL_MGR_GEN_CLASS_NAME_PREFIX, BOOST_PP_SEQ_ELEM(n, LTZ_DL_MGR_FN_SEQ))

// namespace
#define _LTZ_DL_MGR_NAMESPACE_START_MACRO(r, data, elem) namespace elem {
#define _LTZ_DL_MGR_NAMESPACE_END_MACRO(z, n, data) }
#if defined(LTZ_DL_MGR_NAMESPACE_SEQ)
#define LTZ_DL_MGR_NAMESPACE_START BOOST_PP_SEQ_FOR_EACH(_LTZ_DL_MGR_NAMESPACE_START_MACRO, ~, LTZ_DL_MGR_NAMESPACE_SEQ)
#define LTZ_DL_MGR_NAMESPACE_END \
    BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(LTZ_DL_MGR_NAMESPACE_SEQ), _LTZ_DL_MGR_NAMESPACE_END_MACRO, ~)
#else
#define LTZ_DL_MGR_NAMESPACE_START
#define LTZ_DL_MGR_NAMESPACE_END
#endif

#ifndef LTZ_DL_MGR_BASE_CLASS
#define LTZ_DL_MGR_BASE_CLASS
struct _ltz_dl_mgr_base {
    explicit _ltz_dl_mgr_base(const std::string& libpath) {
        dl_handle = dlopen(libpath.c_str(), RTLD_NOW);
        if (!dl_handle) {
            throw std::runtime_error(dlerror());
        }
    }
    ~_ltz_dl_mgr_base() {
        if (dl_handle) {
            dlclose(dl_handle);
        }
    }

   protected:
    void* dl_handle{nullptr};
    static void* get_sym(void* handle, const std::string& name) {
        void* sym = dlsym(handle, name.c_str());
        if (!sym) {
            throw std::runtime_error(dlerror());
        }
        return sym;
    }
};
#endif

#define LTZ_DL_MGR_FNT(n) BOOST_PP_SEQ_ELEM(n, LTZ_DL_MGR_FNT_SEQ)
#define LTZ_DL_MGR_FN(n) BOOST_PP_SEQ_ELEM(n, LTZ_DL_MGR_FN_SEQ)
#define LTZ_DL_MGR_FNT_PTR(n) BOOST_PP_CAT(p_, LTZ_DL_MGR_FN(n))

#define BOOST_PP_LOCAL_MACRO(n)                                                           \
    LTZ_DL_MGR_NAMESPACE_START                                                            \
    struct LTZ_DL_MGR_GEN_CLASS_NAME(n) {                                                 \
        LTZ_DL_MGR_GEN_CLASS_NAME(n)(void* handle) {                                      \
            LTZ_DL_MGR_FN(n) = reinterpret_cast<LTZ_DL_MGR_FNT_PTR(n)>(                   \
                _ltz_dl_mgr_base::get_sym(handle, BOOST_PP_STRINGIZE(LTZ_DL_MGR_FN(n)))); \
        }                                                                                 \
        using LTZ_DL_MGR_FNT_PTR(n) = LTZ_DL_MGR_FNT(n);                                  \
        LTZ_DL_MGR_FNT_PTR(n) LTZ_DL_MGR_FN(n){nullptr};                                  \
    };                                                                                    \
    LTZ_DL_MGR_NAMESPACE_END

#define BOOST_PP_LOCAL_LIMITS (0, (LTZ_DL_MGR_FN_NUM - 1))
#include BOOST_PP_LOCAL_ITERATE()

// ---- generate final manager ----
#define _LTZ_DL_MGR_INHERIT_MACRO(r, data, i, elem) , public LTZ_DL_MGR_GEN_CLASS_NAME(i)
#define _LTZ_DL_MGR_CTOR_INIT_MACRO(r, data, i, elem) , LTZ_DL_MGR_GEN_CLASS_NAME(i)(dl_handle)

LTZ_DL_MGR_NAMESPACE_START
struct LTZ_DL_MGR_NAME : private _ltz_dl_mgr_base BOOST_PP_SEQ_FOR_EACH_I(
                             _LTZ_DL_MGR_INHERIT_MACRO, ~, LTZ_DL_MGR_FN_SEQ) {
    LTZ_DL_MGR_NAME(const std::string& path)
        : _ltz_dl_mgr_base(path) BOOST_PP_SEQ_FOR_EACH_I(_LTZ_DL_MGR_CTOR_INIT_MACRO, ~, LTZ_DL_MGR_FN_SEQ) {}
};
LTZ_DL_MGR_NAMESPACE_END

// inner undef
#undef LTZ_DL_MGR_NAMESPACE_START
#undef LTZ_DL_MGR_NAMESPACE_END
#undef LTZ_DL_MGR_FN_SEQ
#undef LTZ_DL_MGR_FNT_SEQ
#undef LTZ_DL_MGR_FN_NUM

// outer undef
#undef LTZ_DL_MGR_NAMESPACE_SEQ
#undef LTZ_DL_MGR_NAME
#undef LTZ_DL_MGR_SEQ

#endif