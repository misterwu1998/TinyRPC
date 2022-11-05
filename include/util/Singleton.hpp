#if !defined(_Singleton_hpp)
#define _Singleton_hpp

/// 其它成员、静态get方法由用户自定义
#define QuickSingleton_prologue(Typename)\
class Typename {\
private:\
  Typename(){};\
  Typename(Typename const& ){};\
  Typename(Typename && ){};\
  Typename & operator=(Typename const&){};\
public:

/// 其它成员、其它构造函数、静态get方法由用户自定义
#define QuickSingleton_epilogue\
};

#endif // _Singleton_hpp

#if 0
QuickSingleton_prologue(Pool)
private:
  int a;
  Pool(int i):a(i){}
public:
  static Pool& get(int i){
    static Pool p(i);
    return p;
  }
QuickSingleton_epilogue
#endif
