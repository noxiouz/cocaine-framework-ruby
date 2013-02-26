#ifndef _HELPERS_HPP
#define _HELPERS_HPP

#define RUBY_TRY \
    extern VALUE ruby_errinfo; \
    ruby_errinfo = Qnil; \
    try {

#define RUBY_CATCH \
    } catch(const std::exception &e) { \
        std::ostringstream o; \
        o << "c++error: " << e.what(); \
        ruby_errinfo = rb_exc_new2( \
        rb_eRuntimeError, o.str().c_str()); \
    } catch(...) { \
        ruby_errinfo = rb_exc_new2( \
         rb_eRuntimeError, "c++error: Unknown error"); \
    } \
      if(!NIL_P(ruby_errinfo)) { \
      rb_exc_raise(ruby_errinfo); \
    }

template<typename T> 
static void dispose(void *ptr);

template<typename T>
T* get_ctype_pointer(VALUE from);

inline bool
Rb_bool_to_bool(VALUE condition);

inline double
Rb_num_to_double(VALUE _num);

inline std::string 
Rb_string_to_string(VALUE _string);

/****************************************************/

template<typename T>
static void
dispose(void *ptr) {
    if (ptr) {
        T* _p = reinterpret_cast<T*>(ptr);
        delete _p;
    }
}

template<typename T>
T* get_ctype_pointer(VALUE from){
    /* 
     *  ADD throw() if out == NULL
     * */
    T* out;
    Data_Get_Struct(from, T, out);
    if (false == out){
        throw(std::exception());
    }
    return out;
}

inline bool
Rb_bool_to_bool(VALUE condition){
    if (TYPE(condition) == T_TRUE){
        return true;
    } else if (TYPE(condition) == T_FALSE){
        return false;
    } else {
        rb_raise(rb_eArgError, "a boolean is required");
    }
}

inline std::string 
Rb_string_to_string(VALUE _string){
    if (TYPE(_string) == T_STRING){
        return STR2CSTR(_string);
    } else {
        rb_raise(rb_eArgError, "a string is required");
    }
}

inline double
Rb_num_to_double(VALUE _num){
    if (TYPE(_num) == T_FLOAT) {
        return NUM2DBL(_num);
    } else if (TYPE(_num) == T_FIXNUM){
        return NUM2INT(_num);
    } else {
        rb_raise(rb_eArgError, "a string is required");
    }
}
#endif
