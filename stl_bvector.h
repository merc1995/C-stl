/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996-1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_BVECTOR_H
#define __SGI_STL_INTERNAL_BVECTOR_H

__STL_BEGIN_NAMESPACE 

static const int __WORD_BIT = int(CHAR_BIT*sizeof(unsigned int));

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif

struct _Bit_reference {
  unsigned int* _M_p;
  unsigned int _M_mask;
  _Bit_reference(unsigned int* __x, unsigned int __y) 	// 构造函数。
    : _M_p(__x), _M_mask(__y) {}

public:
  _Bit_reference() : _M_p(0), _M_mask(0) {}		// 又一个构造函数。
  operator bool() const { return !(!(*_M_p & _M_mask)); }	// 取&，，为啥要取两遍非呢，，，
  //确保返回值为0或者1. 否则返回值可能为大于1或负数(取决于它的类型).
  _Bit_reference& operator=(bool __x)	//
  {
    if (__x)  *_M_p |= _M_mask;	//如果x为true，使m_p或M_mask 
    else      *_M_p &= ~_M_mask;	// 否则，跟取反后的M_mask取&
    return *this;
  }
  _Bit_reference& operator=(const _Bit_reference& __x) // 重载赋值号
    { return *this = bool(__x); }
  bool operator==(const _Bit_reference& __x) const	// 重载等号
    { return bool(*this) == bool(__x); }
  bool operator<(const _Bit_reference& __x) const {	//重载小于号。
    return !bool(*this) && bool(__x);	// 要满足小于的话，只能是this是0，x是1。所以，对this取反，跟x取and即可。
  }
  void flip() { *_M_p ^= _M_mask; }	// 取异或。
};
// swap
inline void swap(_Bit_reference __x, _Bit_reference __y)
{
  bool __tmp = __x;
  __x = __y;
  __y = __tmp;
}
// 结构体_Bit_iterator_base
struct _Bit_iterator_base : public random_access_iterator<bool, ptrdiff_t> 
{
  unsigned int* _M_p;
  unsigned int _M_offset;

  _Bit_iterator_base(unsigned int* __x, unsigned int __y)	//构造函数。
    : _M_p(__x), _M_offset(__y) {}

  void _M_bump_up() {		// 偏移加一，注意进位！
    if (_M_offset++ == __WORD_BIT - 1) {	//如果偏移量正好是 __WORD_BIT - 1 ，就让偏移置零，M_p++，指针移动一个。
      _M_offset = 0;						//否则就让偏移量加一
      ++_M_p;
    }
  }
  void _M_bump_down() {	//偏移减一 
    if (_M_offset-- == 0) {		//本来就等于零的话，指针要左移
      _M_offset = __WORD_BIT - 1;
      --_M_p;
    }
  }

  void _M_incr(ptrdiff_t __i) {	//对当前增加偏移i
    difference_type __n = __i + _M_offset;		// 先把原来的偏移量加进来。
    _M_p += __n / __WORD_BIT;		// 算出一共有几个指针的移动。
    __n = __n % __WORD_BIT;			//计算剩余的偏移量。
    if (__n < 0) {					//如果剩余偏移量小于零，
      _M_offset = (unsigned int) __n + __WORD_BIT;	// 设置偏移量为 n+word_bit
      --_M_p;										//并且指针减一
    } else
      _M_offset = (unsigned int) __n;	//否则直接设置偏移是n
  }

  bool operator==(const _Bit_iterator_base& __i) const {//重载等于号，指针和偏移必须完全一样。
    return _M_p == __i._M_p && _M_offset == __i._M_offset;
  }
  bool operator<(const _Bit_iterator_base& __i) const {	//重载小于号，
    return _M_p < __i._M_p || (_M_p == __i._M_p && _M_offset < __i._M_offset);// 指针小或者指针相同，偏移小
  }
  bool operator!=(const _Bit_iterator_base& __i) const {	//重载不等于号，直接利用前面重载的等号。
    return !(*this == __i);
  }
  bool operator>(const _Bit_iterator_base& __i) const {	// 重载大于号。
    return __i < *this;
  }
  bool operator<=(const _Bit_iterator_base& __i) const {	//小于等于号
    return !(__i < *this); 
  }
  bool operator>=(const _Bit_iterator_base& __i) const {	//大于等于
    return !(*this < __i);
  }
};

inline ptrdiff_t
operator-(const _Bit_iterator_base& __x, const _Bit_iterator_base& __y) {//减号，，返回二者偏移量的差值
  return __WORD_BIT * (__x._M_p - __y._M_p) + __x._M_offset - __y._M_offset;
}


struct _Bit_iterator : public _Bit_iterator_base		//公有继承上面的 基本迭代器类
{
  typedef _Bit_reference  reference;
  typedef _Bit_reference* pointer;
  typedef _Bit_iterator   iterator;

  _Bit_iterator() : _Bit_iterator_base(0, 0) {}	//继承构造函数；
  _Bit_iterator(unsigned int* __x, unsigned int __y) 
    : _Bit_iterator_base(__x, __y) {}		//自己的构造函数

  reference operator*() const { return reference(_M_p, 1U << _M_offset); }
  iterator& operator++() {	//重载++
    _M_bump_up();
    return *this;
  }
  iterator operator++(int) {
    iterator __tmp = *this;
    _M_bump_up();
    return __tmp;
  }
  iterator& operator--() {
    _M_bump_down();
    return *this;
  }
  iterator operator--(int) {
    iterator __tmp = *this;
    _M_bump_down();
    return __tmp;
  }
  iterator& operator+=(difference_type __i) {
    _M_incr(__i);
    return *this;
  }
  iterator& operator-=(difference_type __i) {
    *this += -__i;
    return *this;
  }
  iterator operator+(difference_type __i) const {
    iterator __tmp = *this;
    return __tmp += __i;
  }
  iterator operator-(difference_type __i) const {
    iterator __tmp = *this;
    return __tmp -= __i;
  }

  reference operator[](difference_type __i) { return *(*this + __i); }		//	重载[]，返回下标对应的元素
};

inline _Bit_iterator 
operator+(ptrdiff_t __n, const _Bit_iterator& __x) { return __x + __n; }


struct _Bit_const_iterator : public _Bit_iterator_base
{
  typedef bool                 reference;
  typedef bool                 const_reference;
  typedef const bool*          pointer;
  typedef _Bit_const_iterator  const_iterator;
//3个构造函数
  _Bit_const_iterator() : _Bit_iterator_base(0, 0) {}
  _Bit_const_iterator(unsigned int* __x, unsigned int __y) 
    : _Bit_iterator_base(__x, __y) {}
  _Bit_const_iterator(const _Bit_iterator& __x) 
    : _Bit_iterator_base(__x._M_p, __x._M_offset) {}

  const_reference operator*() const {
    return _Bit_reference(_M_p, 1U << _M_offset);
  }
  const_iterator& operator++() {
    _M_bump_up();
    return *this;
  }
  const_iterator operator++(int) {
    const_iterator __tmp = *this;
    _M_bump_up();
    return __tmp;
  }
  const_iterator& operator--() {
    _M_bump_down();
    return *this;
  }
  const_iterator operator--(int) {
    const_iterator __tmp = *this;
    _M_bump_down();
    return __tmp;
  }
  const_iterator& operator+=(difference_type __i) {
    _M_incr(__i);
    return *this;
  }
  const_iterator& operator-=(difference_type __i) {
    *this += -__i;
    return *this;
  }
  const_iterator operator+(difference_type __i) const {
    const_iterator __tmp = *this;
    return __tmp += __i;
  }
  const_iterator operator-(difference_type __i) const {
    const_iterator __tmp = *this;
    return __tmp -= __i;
  }
  const_reference operator[](difference_type __i) { 
    return *(*this + __i); 
  }
};

inline _Bit_const_iterator 
operator+(ptrdiff_t __n, const _Bit_const_iterator& __x) { return __x + __n; }


// Bit-vector base class, which encapsulates the difference between
// old SGI-style allocators and standard-conforming allocators.

#ifdef __STL_USE_STD_ALLOCATORS

// Base class for ordinary allocators.
template <class _Allocator, bool __is_static>
class _Bvector_alloc_base {
public:
  typedef typename _Alloc_traits<bool, _Allocator>::allocator_type
          allocator_type;
  allocator_type get_allocator() const { return _M_data_allocator; }

  _Bvector_alloc_base(const allocator_type& __a)
    : _M_data_allocator(__a), _M_start(), _M_finish(), _M_end_of_storage(0) {}

protected:
  unsigned int* _M_bit_alloc(size_t __n) 
    { return _M_data_allocator.allocate((__n + __WORD_BIT - 1)/__WORD_BIT); }
  void _M_deallocate() {
    if (_M_start._M_p)
      _M_data_allocator.deallocate(_M_start._M_p, 
                                   _M_end_of_storage - _M_start._M_p);
  }  

  typename _Alloc_traits<unsigned int, _Allocator>::allocator_type 
          _M_data_allocator;
  _Bit_iterator _M_start;
  _Bit_iterator _M_finish;
  unsigned int* _M_end_of_storage;
};

// Specialization for instanceless allocators.
template <class _Allocator>
class _Bvector_alloc_base<_Allocator, true> {
public:
  typedef typename _Alloc_traits<bool, _Allocator>::allocator_type
          allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }

  _Bvector_alloc_base(const allocator_type&)
    : _M_start(), _M_finish(), _M_end_of_storage(0) {}

protected:
  typedef typename _Alloc_traits<unsigned int, _Allocator>::_Alloc_type
          _Alloc_type;
          
  unsigned int* _M_bit_alloc(size_t __n) 
    { return _Alloc_type::allocate((__n + __WORD_BIT - 1)/__WORD_BIT); }
  void _M_deallocate() {
    if (_M_start._M_p)
      _Alloc_type::deallocate(_M_start._M_p,
                              _M_end_of_storage - _M_start._M_p);
  }  

  _Bit_iterator _M_start;
  _Bit_iterator _M_finish;
  unsigned int* _M_end_of_storage;
};  

template <class _Alloc>
class _Bvector_base
  : public _Bvector_alloc_base<_Alloc,
                               _Alloc_traits<bool, _Alloc>::_S_instanceless>
{
  typedef _Bvector_alloc_base<_Alloc,
                              _Alloc_traits<bool, _Alloc>::_S_instanceless>
          _Base;
public:
  typedef typename _Base::allocator_type allocator_type;

  _Bvector_base(const allocator_type& __a) : _Base(__a) {}
  ~_Bvector_base() { _Base::_M_deallocate(); }
};

#else /* __STL_USE_STD_ALLOCATORS */

template <class _Alloc>
class _Bvector_base
{
public:
  typedef _Alloc allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }

  _Bvector_base(const allocator_type&)
    : _M_start(), _M_finish(), _M_end_of_storage(0) {}
  ~_Bvector_base() { _M_deallocate(); }

protected:
  typedef simple_alloc<unsigned int, _Alloc> _Alloc_type;
  
  unsigned int* _M_bit_alloc(size_t __n) 
    { return _Alloc_type::allocate((__n + __WORD_BIT - 1)/__WORD_BIT); }
  void _M_deallocate() {
    if (_M_start._M_p)
      _Alloc_type::deallocate(_M_start._M_p,
                              _M_end_of_storage - _M_start._M_p);
  }

  _Bit_iterator _M_start;
  _Bit_iterator _M_finish;
  unsigned int* _M_end_of_storage;  
};

#endif /* __STL_USE_STD_ALLOCATORS */

// The next few lines are confusing.  What we're doing is declaring a
//  partial specialization of vector<T, Alloc> if we have the necessary
//  compiler support.  Otherwise, we define a class bit_vector which uses
//  the default allocator. 

#if defined(__STL_CLASS_PARTIAL_SPECIALIZATION) && !defined(__STL_NO_BOOL)
#  define __SGI_STL_VECBOOL_TEMPLATE
#  define __BVECTOR           vector<bool, _Alloc>
#  define __VECTOR            vector
#  define __BVECTOR_BASE      _Bvector_base<_Alloc>
#  define __BVECTOR_TMPL_LIST template <class _Alloc>
   __STL_END_NAMESPACE
#  include <stl_vector.h>
   __STL_BEGIN_NAMESPACE
#else  /* __STL_CLASS_PARTIAL_SPECIALIZATION && !__STL_NO_BOOL */
#  undef  __SGI_STL_VECBOOL_TEMPLATE
#  define __BVECTOR           bit_vector
#  define __VECTOR            bit_vector
#  define __BVECTOR_BASE      _Bvector_base<__STL_DEFAULT_ALLOCATOR(bool) >
#  define __BVECTOR_TMPL_LIST
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION && !__STL_NO_BOOL */


__BVECTOR_TMPL_LIST 
class __BVECTOR : public __BVECTOR_BASE 
{
public:
  typedef bool value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type; 
  typedef _Bit_reference reference;
  typedef bool const_reference;
  typedef _Bit_reference* pointer;
  typedef const bool* const_pointer;

  typedef _Bit_iterator                iterator;
  typedef _Bit_const_iterator          const_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
  typedef reverse_iterator<const_iterator, value_type, const_reference, 
                           difference_type> const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type, reference, difference_type>
          reverse_iterator;
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

  typedef typename __BVECTOR_BASE::allocator_type allocator_type;
  allocator_type get_allocator() const {
    return __BVECTOR_BASE::get_allocator();
  }

protected:
#ifdef __STL_USE_NAMESPACES  
  using __BVECTOR_BASE::_M_bit_alloc;
  using __BVECTOR_BASE::_M_deallocate;
  using __BVECTOR_BASE::_M_start;
  using __BVECTOR_BASE::_M_finish;
  using __BVECTOR_BASE::_M_end_of_storage;
#endif /* __STL_USE_NAMESPACES */

protected:
  void _M_initialize(size_type __n) {	//初始化，大小为n
    unsigned int* __q = _M_bit_alloc(__n);	
    _M_end_of_storage = __q + (__n + __WORD_BIT - 1)/__WORD_BIT;	//需要向上取整。
    _M_start = iterator(__q, 0);	//起始迭代器
    _M_finish = _M_start + difference_type(__n);	//终止迭代器
  }
  void _M_insert_aux(iterator __position, bool __x) {	// 在位置position插入 x
    if (_M_finish._M_p != _M_end_of_storage) {		//如果存储空间足够
      copy_backward(__position, _M_finish, _M_finish + 1);	//利用copy_backward把后面的部分右移
      *__position = __x;		// 把x放到位置position
      ++_M_finish;				//结束迭代器右移一
    }
    else {	//如果空间不足，需要开辟
      size_type __len = size() ? 2 * size() : __WORD_BIT;
      unsigned int* __q = _M_bit_alloc(__len);
      iterator __i = copy(begin(), __position, iterator(__q, 0));
      *__i++ = __x;
      _M_finish = copy(__position, end(), __i);
      _M_deallocate();
      _M_end_of_storage = __q + (__len + __WORD_BIT - 1)/__WORD_BIT;
      _M_start = iterator(__q, 0);
    }
  }

#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIterator>
  void _M_initialize_range(_InputIterator __first, _InputIterator __last,
                           input_iterator_tag) {	//区间初始化，初始化为区间first到last的值
    _M_start = iterator();
    _M_finish = iterator();
    _M_end_of_storage = 0;
    for ( ; __first != __last; ++__first) 
      push_back(*__first);
  }
// 前向迭代器，
  template <class _ForwardIterator>
  void _M_initialize_range(_ForwardIterator __first, _ForwardIterator __last,
                           forward_iterator_tag) {
    size_type __n = 0;
    distance(__first, __last, __n);
    _M_initialize(__n);		// 初始化一个大小为n的区间，
    copy(__first, __last, _M_start);	// 然后把first到last拷贝过去。
  }
//插入区间
//对于区间中的每个元素进行单独插入。
  template <class _InputIterator>
  void _M_insert_range(iterator __pos,
                       _InputIterator __first, _InputIterator __last,
                       input_iterator_tag) {
    for ( ; __first != __last; ++__first) {
      __pos = insert(__pos, *__first);
      ++__pos;
    }
  }
//对于前向迭代器。
  template <class _ForwardIterator>
  void _M_insert_range(iterator __position,
                       _ForwardIterator __first, _ForwardIterator __last,
                       forward_iterator_tag) {
    if (__first != __last) {
      size_type __n = 0;
      distance(__first, __last, __n);		//n记录长度。
      if (capacity() - size() >= __n) {	//如果剩余空间足够
        copy_backward(__position, end(), _M_finish + difference_type(__n));	//就先把pos后面的部分拷到后面对应位置
        copy(__first, __last, __position);	//再把要插入的区间拷进来。
        _M_finish += difference_type(__n);	//更新结束的指针
      }
      else {
        size_type __len = size() + max(size(), __n);
        unsigned int* __q = _M_bit_alloc(__len);	// 开辟新空间，
        iterator __i = copy(begin(), __position, iterator(__q, 0));	//从begin到pos拷到新空间里
        __i = copy(__first, __last, __i);	//后面是要插入的部分。
        _M_finish = copy(__position, end(), __i);	//最后把剩余部分加上。
        _M_deallocate();	//释放空间
        _M_end_of_storage = __q + (__len + __WORD_BIT - 1)/__WORD_BIT;
        _M_start = iterator(__q, 0);
      }
    }
  }      

#endif /* __STL_MEMBER_TEMPLATES */

public:
  iterator begin() { return _M_start; }	//返回开始迭代器。
  const_iterator begin() const { return _M_start; }	
  iterator end() { return _M_finish; }	//返回结束迭代器。
  const_iterator end() const { return _M_finish; }

  reverse_iterator rbegin() { return reverse_iterator(end()); }	//反向迭代器，尾变头，头变尾
  const_reverse_iterator rbegin() const { 
    return const_reverse_iterator(end()); 
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { 
    return const_reverse_iterator(begin()); 
  }

  size_type size() const { return size_type(end() - begin()); }	//size等于end减begin
  size_type max_size() const { return size_type(-1); }	//最大size，-1？？？
  //size_type是无符号数, 将－１转换成无符号数就是最大整数。
  size_type capacity() const {	//占用空间大小
    return size_type(const_iterator(_M_end_of_storage, 0) - begin());
  }
  bool empty() const { return begin() == end(); }	//begin==end表示容器为空。

  reference operator[](size_type __n)	//重载[]，可以用下标直接访问。
    { return *(begin() + difference_type(__n)); }
  const_reference operator[](size_type __n) const
    { return *(begin() + difference_type(__n)); }

#ifdef __STL_THROW_RANGE_ERRORS
  void _M_range_check(size_type __n) const {	//区间检查，如果n大于区间长度，抛出错误
    if (__n >= this->size())
      __stl_throw_range_error("vector<bool>");
  }

  reference at(size_type __n)		//用中括号访问时先检查是否越界
    { _M_range_check(__n); return (*this)[__n]; }
  const_reference at(size_type __n) const
    { _M_range_check(__n); return (*this)[__n]; }
#endif /* __STL_THROW_RANGE_ERRORS */
//各种构造函数
  explicit __VECTOR(const allocator_type& __a = allocator_type())
    : __BVECTOR_BASE(__a) {}

  __VECTOR(size_type __n, bool __value,
            const allocator_type& __a = allocator_type())
    : __BVECTOR_BASE(__a)
  {
    _M_initialize(__n);
    fill(_M_start._M_p, _M_end_of_storage, __value ? ~0 : 0);	//value为true，填充1，否则填充零
  }

  explicit __VECTOR(size_type __n)
    : __BVECTOR_BASE(allocator_type())
  {
    _M_initialize(__n);
    fill(_M_start._M_p, _M_end_of_storage, 0);
  }

  __VECTOR(const __VECTOR& __x) : __BVECTOR_BASE(__x.get_allocator()) {
    _M_initialize(__x.size());
    copy(__x.begin(), __x.end(), _M_start);
  }

#ifdef __STL_MEMBER_TEMPLATES

  // Check whether it's an integral type.  If so, it's not an iterator.

  template <class _Integer>
  void _M_initialize_dispatch(_Integer __n, _Integer __x, __true_type) {
    _M_initialize(__n);
    fill(_M_start._M_p, _M_end_of_storage, __x ? ~0 : 0);
  }

  template <class _InputIterator>
  void _M_initialize_dispatch(_InputIterator __first, _InputIterator __last,
                              __false_type) {
    _M_initialize_range(__first, __last, __ITERATOR_CATEGORY(__first));
  }

  template <class _InputIterator>
  __VECTOR(_InputIterator __first, _InputIterator __last,
           const allocator_type& __a = allocator_type())
    : __BVECTOR_BASE(__a)
  {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_initialize_dispatch(__first, __last, _Integral());
  }
    
#else /* __STL_MEMBER_TEMPLATES */

  __VECTOR(const_iterator __first, const_iterator __last,
           const allocator_type& __a = allocator_type())
    : __BVECTOR_BASE(__a)
  {
    size_type __n = 0;
    distance(__first, __last, __n);
    _M_initialize(__n);
    copy(__first, __last, _M_start);
  }
  __VECTOR(const bool* __first, const bool* __last,
           const allocator_type& __a = allocator_type())
    : __BVECTOR_BASE(__a)
  {
    size_type __n = 0;
    distance(__first, __last, __n);
    _M_initialize(__n);
    copy(__first, __last, _M_start);
  }

#endif /* __STL_MEMBER_TEMPLATES */

  ~__VECTOR() { }

  __VECTOR& operator=(const __VECTOR& __x) {	//重载赋值号
    if (&__x == this) return *this;// 如果本来就一样，直接返回
    if (__x.size() > capacity()) {	//如果空间不足
      _M_deallocate();	//先把原来的注销
      _M_initialize(__x.size());//	然后重新分配空间
    }
    copy(__x.begin(), __x.end(), begin());
    _M_finish = begin() + difference_type(__x.size());
    return *this;
  }

  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.
//填充前n个
  void _M_fill_assign(size_t __n, bool __x) {
    if (__n > size()) {	//如果填充数量大于原来的。
      fill(_M_start._M_p, _M_end_of_storage, __x ? ~0 : 0);	//先把原来的全部覆盖。
      insert(end(), __n - size(), __x);	//然后在后面插入剩余的
    }
    else {
      erase(begin() + __n, end());	//否则直接把后边的删掉。
      fill(_M_start._M_p, _M_end_of_storage, __x ? ~0 : 0);
    }
  }

  void assign(size_t __n, bool __x) { _M_fill_assign(__n, __x); }

#ifdef __STL_MEMBER_TEMPLATES

  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_assign_dispatch(__first, __last, _Integral());
  }

  template <class _Integer>
  void _M_assign_dispatch(_Integer __n, _Integer __val, __true_type)
    { _M_fill_assign((size_t) __n, (bool) __val); }

  template <class _InputIter>
  void _M_assign_dispatch(_InputIter __first, _InputIter __last, __false_type)
    { _M_assign_aux(__first, __last, __ITERATOR_CATEGORY(__first)); }

	//初始化一段区间。
  template <class _InputIterator>
  void _M_assign_aux(_InputIterator __first, _InputIterator __last,
                     input_iterator_tag) {
    iterator __cur = begin();
    for ( ; __first != __last && __cur != end(); ++__cur, ++__first)
      *__cur = *__first;
    if (__first == __last)	// 满了就把后面的删掉
      erase(__cur, end());
    else
      insert(end(), __first, __last);	//否则就插入到后面。
  }

  template <class _ForwardIterator>
  void _M_assign_aux(_ForwardIterator __first, _ForwardIterator __last,
                     forward_iterator_tag) {
    size_type __len = 0;
    distance(__first, __last, __len);
    if (__len < size())
      erase(copy(__first, __last, begin()), end());
    else {
      _ForwardIterator __mid = __first;
      advance(__mid, size());
      copy(__first, __mid, begin());
      insert(end(), __mid, __last);
    }
  }    

#endif /* __STL_MEMBER_TEMPLATES */

  void reserve(size_type __n) {//预留空间
    if (capacity() < __n) {
      unsigned int* __q = _M_bit_alloc(__n);
      _M_finish = copy(begin(), end(), iterator(__q, 0));
      _M_deallocate();
      _M_start = iterator(__q, 0);
      _M_end_of_storage = __q + (__n + __WORD_BIT - 1)/__WORD_BIT;
    }
  }

  reference front() { return *begin(); }	//返回容器头
  const_reference front() const { return *begin(); }
  reference back() { return *(end() - 1); }	//返回容器尾
  const_reference back() const { return *(end() - 1); }
  void push_back(bool __x) {	//push_back
    if (_M_finish._M_p != _M_end_of_storage)	//空间未满，直接添加
      *_M_finish++ = __x;
    else
      _M_insert_aux(end(), __x);	//空间已满，向后插入。
  }
  void swap(__BVECTOR& __x) {
    __STD::swap(_M_start, __x._M_start);
    __STD::swap(_M_finish, __x._M_finish);
    __STD::swap(_M_end_of_storage, __x._M_end_of_storage);
  }
  iterator insert(iterator __position, bool __x = bool()) {
    difference_type __n = __position - begin();
    if (_M_finish._M_p != _M_end_of_storage && __position == end())
      *_M_finish++ = __x;
    else
      _M_insert_aux(__position, __x);
    return begin() + __n;
  }

#ifdef __STL_MEMBER_TEMPLATES
  // Check whether it's an integral type.  If so, it's not an iterator.

  template <class _Integer>
  void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __x,
                          __true_type) {
    _M_fill_insert(__pos, __n, __x);
  }

  template <class _InputIterator>
  void _M_insert_dispatch(iterator __pos,
                          _InputIterator __first, _InputIterator __last,
                          __false_type) {
    _M_insert_range(__pos, __first, __last, __ITERATOR_CATEGORY(__first));
  }

  template <class _InputIterator>
  void insert(iterator __position,
              _InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_insert_dispatch(__position, __first, __last, _Integral());
  }

#else /* __STL_MEMBER_TEMPLATES */
  void insert(iterator __position,
              const_iterator __first, const_iterator __last) {
//在position的位置在插入一个 该区间本身
    if (__first == __last) return;	//如果空，返回
    size_type __n = 0;
    distance(__first, __last, __n);
    if (capacity() - size() >= __n) {	//如果剩余空间足够的话
      copy_backward(__position, end(), _M_finish + __n);	//先把pos后的部分拷到对应位置。
      copy(__first, __last, __position);	//再把整个区间拷到pos后面
      _M_finish += __n;	//更新结束的指针
    }
    else {
      size_type __len = size() + max(size(), __n);
      unsigned int* __q = _M_bit_alloc(__len);
      iterator __i = copy(begin(), __position, iterator(__q, 0));
      __i = copy(__first, __last, __i);
      _M_finish = copy(__position, end(), __i);
      _M_deallocate();
      _M_end_of_storage = __q + (__len + __WORD_BIT - 1)/__WORD_BIT;
      _M_start = iterator(__q, 0);
    }
  }

  void insert(iterator __position, const bool* __first, const bool* __last) {
    if (__first == __last) return;
    size_type __n = 0;
    distance(__first, __last, __n);
    if (capacity() - size() >= __n) {
      copy_backward(__position, end(), _M_finish + __n);
      copy(__first, __last, __position);
      _M_finish += __n;
    }
    else {
      size_type __len = size() + max(size(), __n);
      unsigned int* __q = _M_bit_alloc(__len);
      iterator __i = copy(begin(), __position, iterator(__q, 0));
      __i = copy(__first, __last, __i);
      _M_finish = copy(__position, end(), __i);
      _M_deallocate();
      _M_end_of_storage = __q + (__len + __WORD_BIT - 1)/__WORD_BIT;
      _M_start = iterator(__q, 0);
    }
  }
#endif /* __STL_MEMBER_TEMPLATES */
//在pos位置插入n个x
  void _M_fill_insert(iterator __position, size_type __n, bool __x) {
    if (__n == 0) return;
    if (capacity() - size() >= __n) {//空间够的话
      copy_backward(__position, end(), _M_finish + difference_type(__n));
      fill(__position, __position + difference_type(__n), __x);//填充n个x
      _M_finish += difference_type(__n);
    }
    else {
      size_type __len = size() + max(size(), __n);
      unsigned int* __q = _M_bit_alloc(__len);
      iterator __i = copy(begin(), __position, iterator(__q, 0));
      fill_n(__i, __n, __x);
      _M_finish = copy(__position, end(), __i + difference_type(__n));
      _M_deallocate();
      _M_end_of_storage = __q + (__len + __WORD_BIT - 1)/__WORD_BIT;
      _M_start = iterator(__q, 0);
    }
  }

  void insert(iterator __position, size_type __n, bool __x) {
    _M_fill_insert(__position, __n, __x);
  }

  void pop_back() { --_M_finish; }
  iterator erase(iterator __position) {	//删除第pos位置的元素。
    if (__position + 1 != end())	//如果它不是最后一个元素的话，
      copy(__position + 1, end(), __position);	//把它下一个一直到最后拷到它的位置，相当于把它删掉了。
      --_M_finish;
    return __position;
  }
  iterator erase(iterator __first, iterator __last) {	//删除一段区间
    _M_finish = copy(__last, end(), __first);
    return __first;
  }
  void resize(size_type __new_size, bool __x = bool()) {	//重新定义大小
    if (__new_size < size()) 
      erase(begin() + difference_type(__new_size), end());
    else
      insert(end(), __new_size - size(), __x);
  }
  void flip() {	//取异或。
    for (unsigned int* __p = _M_start._M_p; __p != _M_end_of_storage; ++__p)
      *__p = ~*__p;
  }

  void clear() { erase(begin(), end()); }
};

#ifdef __SGI_STL_VECBOOL_TEMPLATE

// This typedef is non-standard.  It is provided for backward compatibility.
typedef vector<bool, alloc> bit_vector;

#else /* __SGI_STL_VECBOOL_TEMPLATE */

inline void swap(bit_vector& __x, bit_vector& __y) {
  __x.swap(__y);
}
// 重载==
inline bool 
operator==(const bit_vector& __x, const bit_vector& __y)
{
  return (__x.size() == __y.size() && 
          equal(__x.begin(), __x.end(), __y.begin()));
}

inline bool 
operator!=(const bit_vector& __x, const bit_vector& __y)
{
  return !(__x == __y);
}
//重载，把小于号重载之后下面都光用到小于号，比较方便
inline bool 
operator<(const bit_vector& __x, const bit_vector& __y)
{
  return lexicographical_compare(__x.begin(), __x.end(), 
                                 __y.begin(), __y.end());
}

inline bool operator>(const bit_vector& __x, const bit_vector& __y)
{
  return __y < __x;
}

inline bool operator<=(const bit_vector& __x, const bit_vector& __y)
{
  return !(__y < __x);
}

inline bool operator>=(const bit_vector& __x, const bit_vector& __y)
{
  return !(__x < __y);
}

#endif /* __SGI_STL_VECBOOL_TEMPLATE */

#undef __SGI_STL_VECBOOL_TEMPLATE
#undef __BVECTOR
#undef __VECTOR
#undef __BVECTOR_BASE
#undef __BVECTOR_TMPL_LIST 

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__STL_END_NAMESPACE 

#endif /* __SGI_STL_INTERNAL_BVECTOR_H */

// Local Variables:
// mode:C++
// End:
