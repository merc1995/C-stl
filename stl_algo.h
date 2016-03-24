/*
	merc
	2016-3-17
	stl_algo.h
*/

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
 * Copyright (c) 1996
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

#ifndef __SGI_STL_INTERNAL_ALGO_H
#define __SGI_STL_INTERNAL_ALGO_H

#include <stl_heap.h>

// See concept_checks.h for the concept-checking macros 
// __STL_REQUIRES, __STL_CONVERTIBLE, etc.


__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1209
#endif

// __median (an extension, not present in the C++ standard).
// 比較a,b,c三個數中 中間的數字是幾
template <class _Tp>
inline const _Tp& __median(const _Tp& __a, const _Tp& __b, const _Tp& __c) {
  __STL_REQUIRES(_Tp, _LessThanComparable);
  if (__a < __b)			// if a<b  ，看c。如果c比a小，返回a，c比b大，返回b。比a大，比b小，返回c		
    if (__b < __c)
      return __b;
    else if (__a < __c)
      return __c;
    else
      return __a;
  else if (__a < __c)	// a>=b && a<c	返回a 
    return __a;
  else if (__b < __c)		// a>=b ,a>=c    b<c 返回c
    return __c;
  else
    return __b;				// a>=b a>=c b>=c ,返回b
}

template <class _Tp, class _Compare>
inline const _Tp&
__median(const _Tp& __a, const _Tp& __b, const _Tp& __c, _Compare __comp) {
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);		// 對於不同的 比較運算符，類似上面的操作
  if (__comp(__a, __b))
    if (__comp(__b, __c))
      return __b;
    else if (__comp(__a, __c))
      return __c;
    else
      return __a;
  else if (__comp(__a, __c))
    return __a;
  else if (__comp(__b, __c))
    return __c;
  else
    return __b;
}

// for_each.  Apply a function to every element of a range.		for_each操作，遍歷一個函數的每一個元素。
// 對每一個元素執行 __f函數操作。
template <class _InputIter, class _Function>
_Function for_each(_InputIter __first, _InputIter __last, _Function __f) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  for ( ; __first != __last; ++__first)
    __f(*__first);
  return __f;
}

// find and find_if.
// find和find_if操作，查找是__val這個值第一次出現的地方。如果沒有返回last
//對於結構體的vector     需要自定义类 ，并重载()操作符号,例如：
/*
class findx {

public:

         findx(const string str){test=str;}

         string GetTest() {return test;}

         bool operator()(A& dValue) {

         if(dValue.GetStr().compare(test)==0)

              return true;

         else

              return false;

          }

private: 

          string test;

};
*/
template <class _InputIter, class _Tp>
inline _InputIter find(_InputIter __first, _InputIter __last,
                       const _Tp& __val,
                       input_iterator_tag)
{
  while (__first != __last && !(*__first == __val))
    ++__first;
  return __first;
}
//find_if 是判斷是否存在滿足判斷條件 pred 的元素，沒有的話返回last
template <class _InputIter, class _Predicate>
inline _InputIter find_if(_InputIter __first, _InputIter __last,
                          _Predicate __pred,
                          input_iterator_tag)
{
  while (__first != __last && !__pred(*__first))
    ++__first;
  return __first;
}
// 下面這個find 並不能看懂，，mark一下！！！

//解： 對於隨機獲取的 迭代器 首先要計算出first 和 last之間的差值。然後進行遍歷。至於為什麼使用這種方式遍歷。。。。。？
#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION

template <class _RandomAccessIter, class _Tp>
_RandomAccessIter find(_RandomAccessIter __first, _RandomAccessIter __last,
                       const _Tp& __val,
                       random_access_iterator_tag)
{
  typename iterator_traits<_RandomAccessIter>::difference_type __trip_count
    = (__last - __first) >> 2;

  for ( ; __trip_count > 0 ; --__trip_count) {
    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;
  }

  switch(__last - __first) {
  case 3:
    if (*__first == __val) return __first;
    ++__first;
  case 2:
    if (*__first == __val) return __first;
    ++__first;
  case 1:
    if (*__first == __val) return __first;
    ++__first;
  case 0:
  default:
    return __last;
  }
}

template <class _RandomAccessIter, class _Predicate>
_RandomAccessIter find_if(_RandomAccessIter __first, _RandomAccessIter __last,
                          _Predicate __pred,
                          random_access_iterator_tag)
{
  typename iterator_traits<_RandomAccessIter>::difference_type __trip_count
    = (__last - __first) >> 2;

  for ( ; __trip_count > 0 ; --__trip_count) {
    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;
  }

  switch(__last - __first) {
  case 3:
    if (__pred(*__first)) return __first;
    ++__first;
  case 2:
    if (__pred(*__first)) return __first;
    ++__first;
  case 1:
    if (__pred(*__first)) return __first;
    ++__first;
  case 0:
  default:
    return __last;
  }
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
//函数 find 根据迭代器的类型选择之前定义的不同 find 函数来实现查找从 __first 开始到 __last 结束的区域中的第一个值为 __val 的元素。函数中有两个类型检测语句，其中第一个要求迭代器为 input_iterator 。第二个语句要求迭代器中的键值类型能够和 _Tp 类型应用 operator== 进行比较。
template <class _InputIter, class _Tp>
inline _InputIter find(_InputIter __first, _InputIter __last,
                       const _Tp& __val)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool, 
            typename iterator_traits<_InputIter>::value_type, _Tp);
  return find(__first, __last, __val, __ITERATOR_CATEGORY(__first));
}

template <class _InputIter, class _Predicate>
inline _InputIter find_if(_InputIter __first, _InputIter __last,
                          _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
          typename iterator_traits<_InputIter>::value_type);
  return find_if(__first, __last, __pred, __ITERATOR_CATEGORY(__first));
}

// adjacent_find.
//查找第一次出現相鄰兩個元素相等的位置，沒有的話返回last
template <class _ForwardIter>
_ForwardIter adjacent_find(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  if (__first == __last)
    return __last;
  _ForwardIter __next = __first;
  while(++__next != __last) {
    if (*__first == *__next)	// first始終指向next的前一個元素，用於比較兩個相鄰元素是否相等。
      return __first;		//相等返回first，也就是相等元素的前一個的位置
    __first = __next;
  }
  return __last;
}
// 類似上面的find_if函數，用於判斷是否存在相鄰兩個元素滿足 __binary_pred 函數。沒有的話返回last
template <class _ForwardIter, class _BinaryPredicate>
_ForwardIter adjacent_find(_ForwardIter __first, _ForwardIter __last,
                           _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
          typename iterator_traits<_ForwardIter>::value_type,
          typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last)
    return __last;
  _ForwardIter __next = __first;
  while(++__next != __last) {
    if (__binary_pred(*__first, *__next))
      return __first;
    __first = __next;
  }
  return __last;
}

// count and count_if.  There are two version of each, one whose return type
// type is void and one (present only if we have partial specialization)
// whose return type is iterator_traits<_InputIter>::difference_type.  The
// C++ standard only has the latter version, but the former, which was present
// in the HP STL, is retained for backward compatibility.
// count 和 count_if 各有兩個版本 第一個是void類型的，無返回值。第二個有返回值，c++標準只有第二種
// count用於計算 迭代器 first到last之間一共有多少個 __value 元素，返回其個數
template <class _InputIter, class _Tp, class _Size>
void count(_InputIter __first, _InputIter __last, const _Tp& __value,
           _Size& __n) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _EqualityComparable);
  __STL_REQUIRES(_Tp, _EqualityComparable);
  for ( ; __first != __last; ++__first)
    if (*__first == __value)
      ++__n;
}
// count_if 計算 符合函數 __pred的元素的個數，返回其值
template <class _InputIter, class _Predicate, class _Size>
void count_if(_InputIter __first, _InputIter __last, _Predicate __pred,
              _Size& __n) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool, 
                  typename iterator_traits<_InputIter>::value_type);
  for ( ; __first != __last; ++__first)
    if (__pred(*__first))
      ++__n;
}

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION

template <class _InputIter, class _Tp>
typename iterator_traits<_InputIter>::difference_type
count(_InputIter __first, _InputIter __last, const _Tp& __value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _EqualityComparable);
  __STL_REQUIRES(_Tp, _EqualityComparable);
  typename iterator_traits<_InputIter>::difference_type __n = 0;
  for ( ; __first != __last; ++__first)
    if (*__first == __value)
      ++__n;
  return __n;
}

template <class _InputIter, class _Predicate>
typename iterator_traits<_InputIter>::difference_type
count_if(_InputIter __first, _InputIter __last, _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool, 
                  typename iterator_traits<_InputIter>::value_type);
  typename iterator_traits<_InputIter>::difference_type __n = 0;
  for ( ; __first != __last; ++__first)
    if (__pred(*__first))
      ++__n;
  return __n;
}


#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// search.
// search 是在 指定區間 first1  和 last1 之間查找  first2  和  last2
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter1 search(_ForwardIter1 __first1, _ForwardIter1 __last1,
                     _ForwardIter2 __first2, _ForwardIter2 __last2) 
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);

  // Test for empty ranges  如果其中有空區間 ，直接返回first1
  if (__first1 == __last1 || __first2 == __last2)
    return __first1;

  // Test for a pattern of length 1.  如果第二個區間中只有一個元素的話，直接調用 find 函數 在 第一個區間內進行查找。
  _ForwardIter2 __tmp(__first2);
  ++__tmp;
  if (__tmp == __last2)
    return find(__first1, __last1, *__first2);

  // General case. 對於一般情況

  _ForwardIter2 __p1, __p;

  __p1 = __first2; ++__p1;		// p1指向第二個區間第二個元素

  _ForwardIter1 __current = __first1; // current指向第一個區間第一個元素

  while (__first1 != __last1) {
    __first1 = find(__first1, __last1, *__first2);		// 首先查找第二個區間中第一個元素在第一個區間出現的位置。
    if (__first1 == __last1)							//	如果在第一個區間中未能找到該元素，直接返回 last1
      return __last1;

    __p = __p1;				// p指向第二個區間第二個元素。
    __current = __first1; 	// current現在指向查找到的元素位置。
    if (++__current == __last1)	// current指向下一個元素，如果到了最後，也就是last。而根據上面的判斷，第二個區間不止一個元素，所以肯定沒有包含第二個區間，直接返回last1
      return __last1;

    while (*__current == *__p) {		// 從找到的下標開始一一判斷是否存在連續區間，恰好是第二個區間。
      if (++__p == __last2)		//	第二個區間已經全部被查找完，說明存在。返回此時的first1，也就是上面find到的位置。
        return __first1;
      if (++__current == __last1)	// 還沒遍歷完第二個區間，第一個區間就沒了，返回last1
        return __last1;
    }

    ++__first1;		// 講當前first1 指向下一個元素，進行循環。
  }
  return __first1;		// 查找結束后 返回first1.
}
// 下面的search 加入了一個 判定函數，用於判斷區間一中 是否存在 子區間繼而區間二中的元素滿足該判定函數。
template <class _ForwardIter1, class _ForwardIter2, class _BinaryPred>
_ForwardIter1 search(_ForwardIter1 __first1, _ForwardIter1 __last1,
                     _ForwardIter2 __first2, _ForwardIter2 __last2,
                     _BinaryPred  __predicate) 
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPred, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);

  // Test for empty ranges
  if (__first1 == __last1 || __first2 == __last2)
    return __first1;

  // Test for a pattern of length 1.
  _ForwardIter2 __tmp(__first2);
  ++__tmp;
  if (__tmp == __last2) {
    while (__first1 != __last1 && !__predicate(*__first1, *__first2))		// 還用不同的地方就是 前面的額search在區間二只有一個元素時，直接使用了find函數，而這個裡面卻是直接遍歷了一遍
	// 因為 find_if 中的函數的參數只有一個，這裡無法調用find_if，只能自己來遍歷。
      ++__first1;
    return __first1;    
  }

  // General case.

  _ForwardIter2 __p1, __p;

  __p1 = __first2; ++__p1;

  _ForwardIter1 __current = __first1;

  while (__first1 != __last1) {
    while (__first1 != __last1) {			//這裡同樣，必須自己遍歷來找到第一個滿足條件的地方。
      if (__predicate(*__first1, *__first2))
        break;
      ++__first1;
    }
    while (__first1 != __last1 && !__predicate(*__first1, *__first2))
      ++__first1;
    if (__first1 == __last1)
      return __last1;

    __p = __p1;
    __current = __first1; 
    if (++__current == __last1) return __last1;

    while (__predicate(*__current, *__p)) {
      if (++__p == __last2)
        return __first1;
      if (++__current == __last1)
        return __last1;
    }

    ++__first1;
  }
  return __first1;
}

// search_n.  Search for __count consecutive copies of __val.
// search_n 	查找出現count次連續val值的首個下標
template <class _ForwardIter, class _Integer, class _Tp>
_ForwardIter search_n(_ForwardIter __first, _ForwardIter __last,
                      _Integer __count, const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  __STL_REQUIRES(_Tp, _EqualityComparable);

  if (__count <= 0)	// 如果count<=0的話直接返回first
    return __first;//  小想法：可以利用這個性質，修改一下，使某個地方一直返回第一個元素。
  else {	
    __first = find(__first, __last, __val);	// 首先find到第一個出現val的位置
    while (__first != __last) {			// 循環
      _Integer __n = __count - 1;		// n用於計數。
      _ForwardIter __i = __first;	
      ++__i;	// i指向查找到的下一個元素。
      while (__i != __last && __n != 0 && *__i == __val) {	// 開始循環判斷是否存在連續的元素值等於val
        ++__i;
        --__n;
      }
      if (__n == 0)		// 如果n等於零表示存在一個連續的長度至少為 count的 val 元素。
        return __first;
      else
        __first = find(__i, __last, __val);		//否則繼續查找下一個等於val的值的位置。
    }
    return __last;
  }
}
// 感覺這個應該叫  search_if 更恰當些，，他比上面的search多加了 一個判斷函數 __binary_pred 用於判斷是否存在一個長度至少為n的連續區間，滿足這個函數。
template <class _ForwardIter, class _Integer, class _Tp, class _BinaryPred>
_ForwardIter search_n(_ForwardIter __first, _ForwardIter __last,
                      _Integer __count, const _Tp& __val,
                      _BinaryPred __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPred, bool, 
             typename iterator_traits<_ForwardIter>::value_type, _Tp);
  if (__count <= 0)
    return __first;
  else {
    while (__first != __last) {
      if (__binary_pred(*__first, __val))
        break;
      ++__first;
    }
    while (__first != __last) {
      _Integer __n = __count - 1;
      _ForwardIter __i = __first;
      ++__i;
      while (__i != __last && __n != 0 && __binary_pred(*__i, __val)) {
        ++__i;
        --__n;
      }
      if (__n == 0)
        return __first;
      else {
        while (__i != __last) {
          if (__binary_pred(*__i, __val))
            break;
          ++__i;
        }
        __first = __i;
      }
    }
    return __last;
  }
} 

// swap_ranges
// 交換區間。 對first1和last1中的元素與 first2開始的同樣長度的元素交換。返回交換后第二個區間的 末尾。
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter2 swap_ranges(_ForwardIter1 __first1, _ForwardIter1 __last1,
                          _ForwardIter2 __first2) {
  __STL_REQUIRES(_ForwardIter1, _Mutable_ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _Mutable_ForwardIterator);
  __STL_CONVERTIBLE(typename iterator_traits<_ForwardIter1>::value_type,
                    typename iterator_traits<_ForwardIter2>::value_type);
  __STL_CONVERTIBLE(typename iterator_traits<_ForwardIter2>::value_type,
                    typename iterator_traits<_ForwardIter1>::value_type);
  for ( ; __first1 != __last1; ++__first1, ++__first2)
    iter_swap(__first1, __first2);
  return __first2;
}

// transform
// 變換 對於從first到last之間的元素，每個都執行 opr 操作，存到result裡面，返回result的末尾。
template <class _InputIter, class _OutputIter, class _UnaryOperation>
_OutputIter transform(_InputIter __first, _InputIter __last,
                      _OutputIter __result, _UnaryOperation __opr) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);

  for ( ; __first != __last; ++__first, ++__result)
    *__result = __opr(*__first);
  return __result;
}
// 這個是二元操作。對於兩個區間，進行 binary_op 操作，結果存到result。
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _BinaryOperation>
_OutputIter transform(_InputIter1 __first1, _InputIter1 __last1,
                      _InputIter2 __first2, _OutputIter __result,
                      _BinaryOperation __binary_op) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  for ( ; __first1 != __last1; ++__first1, ++__first2, ++__result)
    *__result = __binary_op(*__first1, *__first2);
  return __result;
}

// replace, replace_if, replace_copy, replace_copy_if
// replace ，用 new_value 替代 old_value 。遍歷，全是遍歷。
template <class _ForwardIter, class _Tp>
void replace(_ForwardIter __first, _ForwardIter __last,
             const _Tp& __old_value, const _Tp& __new_value) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
         typename iterator_traits<_ForwardIter>::value_type, _Tp);
  __STL_CONVERTIBLE(_Tp, typename iterator_traits<_ForwardIter>::value_type);
  for ( ; __first != __last; ++__first)
    if (*__first == __old_value)
      *__first = __new_value;
}
// replace_if 把判斷條件變為一個函數 pred ，同樣遍歷。
template <class _ForwardIter, class _Predicate, class _Tp>
void replace_if(_ForwardIter __first, _ForwardIter __last,
                _Predicate __pred, const _Tp& __new_value) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_CONVERTIBLE(_Tp, typename iterator_traits<_ForwardIter>::value_type);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
             typename iterator_traits<_ForwardIter>::value_type);
  for ( ; __first != __last; ++__first)
    if (__pred(*__first))
      *__first = __new_value;
}
// replace_copy，把 replace之後的結果存到了 result 裡面
template <class _InputIter, class _OutputIter, class _Tp>
_OutputIter replace_copy(_InputIter __first, _InputIter __last,
                         _OutputIter __result,
                         const _Tp& __old_value, const _Tp& __new_value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
         typename iterator_traits<_InputIter>::value_type, _Tp);
  for ( ; __first != __last; ++__first, ++__result)
    *__result = *__first == __old_value ? __new_value : *__first;
  return __result;
}
// replace_if 后 吧結果存到 result 裡面
template <class _InputIter, class _OutputIter, class _Predicate, class _Tp>
_OutputIter replace_copy_if(_InputIter __first, _InputIter __last,
                            _OutputIter __result,
                            _Predicate __pred, const _Tp& __new_value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
                typename iterator_traits<_InputIter>::value_type);
  for ( ; __first != __last; ++__first, ++__result)
    *__result = __pred(*__first) ? __new_value : *__first;
  return __result;
}

// generate and generate_n
// generate 把所有值 變為 gen()
template <class _ForwardIter, class _Generator>
void generate(_ForwardIter __first, _ForwardIter __last, _Generator __gen) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_GENERATOR_CHECK(_Generator, 
          typename iterator_traits<_ForwardIter>::value_type);
  for ( ; __first != __last; ++__first)
    *__first = __gen();
}
// 把前n個值變為gen()
//用指定函数对象产生的值去给容器内元素赋值
template <class _OutputIter, class _Size, class _Generator>
_OutputIter generate_n(_OutputIter __first, _Size __n, _Generator __gen) {
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  for ( ; __n > 0; --__n, ++__first)
    *__first = __gen();
  return __first;
}

// remove, remove_if, remove_copy, remove_copy_if
// 將first到last區間的元素中 值為value 的去掉，并存到result中。
template <class _InputIter, class _OutputIter, class _Tp>
_OutputIter remove_copy(_InputIter __first, _InputIter __last,
                        _OutputIter __result, const _Tp& __value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
       typename iterator_traits<_InputIter>::value_type, _Tp);
  for ( ; __first != __last; ++__first)
    if (!(*__first == __value)) {
      *__result = *__first;
      ++__result;
    }
  return __result;
}
// 把判斷條件變為一個函數，remove_copy_if
template <class _InputIter, class _OutputIter, class _Predicate>
_OutputIter remove_copy_if(_InputIter __first, _InputIter __last,
                           _OutputIter __result, _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
             typename iterator_traits<_InputIter>::value_type);
  for ( ; __first != __last; ++__first)
    if (!__pred(*__first)) {
      *__result = *__first;
      ++__result;
    }
  return __result;
}
// 去掉區間中值為 value的 ，并依舊存在原來的地方。
template <class _ForwardIter, class _Tp>
_ForwardIter remove(_ForwardIter __first, _ForwardIter __last,
                    const _Tp& __value) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
       typename iterator_traits<_ForwardIter>::value_type, _Tp);
  __STL_CONVERTIBLE(_Tp, typename iterator_traits<_ForwardIter>::value_type);
  __first = find(__first, __last, __value);		// 調用find，找到value第一次出現的地方。
  _ForwardIter __i = __first;
  return __first == __last ? __first 		// 如果first等於last表示未找到value，返回first，也就是last
                           : remove_copy(++__i, __last, __first, __value);	// 否則將迭代器加一跳過該元素，使用remove_copy 將剩下的元素拷貝到去掉value后的區間的後面。
						   // 1 2 3 4 5 去掉3之後，i指向4，first指向3，把從4開始的元素使用remove_copy拷貝到first後面。變為1 2 4 5
}
// 這裡使用 remove_copy_if 其他與 remove相同。
template <class _ForwardIter, class _Predicate>
_ForwardIter remove_if(_ForwardIter __first, _ForwardIter __last,
                       _Predicate __pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
               typename iterator_traits<_ForwardIter>::value_type);
  __first = find_if(__first, __last, __pred);
  _ForwardIter __i = __first;
  return __first == __last ? __first 
                           : remove_copy_if(++__i, __last, __first, __pred);
}

// unique and unique_copy
// unique用於去除相鄰的重複元素。
template <class _InputIter, class _OutputIter, class _Tp>
_OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                          _OutputIter __result, _Tp*) {
  _Tp __value = *__first;	// 先讓value等於第一個元素，
  *__result = __value;
  while (++__first != __last)	// 遍歷，如果後一個跟value不相等，則更新value，并存到result裡面
    if (!(__value == *__first)) {
      __value = *__first;
      *++__result = __value;
    }
  return ++__result;	// 注意返回的迭代器指向最後一個元素的下一個。
}

template <class _InputIter, class _OutputIter>
inline _OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                                 _OutputIter __result, 
                                 output_iterator_tag) {
  return __unique_copy(__first, __last, __result, __VALUE_TYPE(__first));
}

template <class _InputIter, class _ForwardIter>
_ForwardIter __unique_copy(_InputIter __first, _InputIter __last,
                           _ForwardIter __result, forward_iterator_tag) {
  *__result = *__first;
  while (++__first != __last)
    if (!(*__result == *__first))
      *++__result = *__first;
  return ++__result;
}

template <class _InputIter, class _OutputIter>
inline _OutputIter unique_copy(_InputIter __first, _InputIter __last,
                               _OutputIter __result) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _EqualityComparable);
  if (__first == __last) return __result;
  return __unique_copy(__first, __last, __result,
                       __ITERATOR_CATEGORY(__result));
}
// 二元判斷函數 binary_pred 用於判斷是否滿足條件，相當於把原來的value==*first變為函數判斷。
template <class _InputIter, class _OutputIter, class _BinaryPredicate,
          class _Tp>
_OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                          _OutputIter __result,
                          _BinaryPredicate __binary_pred, _Tp*) {
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool, _Tp, _Tp);
  _Tp __value = *__first;
  *__result = __value;
  while (++__first != __last)
    if (!__binary_pred(__value, *__first)) {
      __value = *__first;
      *++__result = __value;
    }
  return ++__result;
}

template <class _InputIter, class _OutputIter, class _BinaryPredicate>
inline _OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                                 _OutputIter __result,
                                 _BinaryPredicate __binary_pred,
                                 output_iterator_tag) {
  return __unique_copy(__first, __last, __result, __binary_pred,
                       __VALUE_TYPE(__first));
}

template <class _InputIter, class _ForwardIter, class _BinaryPredicate>
_ForwardIter __unique_copy(_InputIter __first, _InputIter __last,
                           _ForwardIter __result, 
                           _BinaryPredicate __binary_pred,
                           forward_iterator_tag) {
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
     typename iterator_traits<_ForwardIter>::value_type,
     typename iterator_traits<_InputIter>::value_type);
  *__result = *__first;
  while (++__first != __last)
    if (!__binary_pred(*__result, *__first)) *++__result = *__first;
  return ++__result;
}

template <class _InputIter, class _OutputIter, class _BinaryPredicate>
inline _OutputIter unique_copy(_InputIter __first, _InputIter __last,
                               _OutputIter __result,
                               _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  if (__first == __last) return __result;
  return __unique_copy(__first, __last, __result, __binary_pred,
                       __ITERATOR_CATEGORY(__result));
}

template <class _ForwardIter>
_ForwardIter unique(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  __first = adjacent_find(__first, __last); //使用ａｄｊａｃｅｎｔ＿ｆｉｎｄ查找第一個相鄰重複元素的位置， 1 2 2 4
  return unique_copy(__first, __last, __first);	//使用r=unique_copy把後面的元素進行去重。
}
// 把判斷條件變為函數。
template <class _ForwardIter, class _BinaryPredicate>
_ForwardIter unique(_ForwardIter __first, _ForwardIter __last,
                    _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool, 
      typename iterator_traits<_ForwardIter>::value_type,
      typename iterator_traits<_ForwardIter>::value_type);
  __first = adjacent_find(__first, __last, __binary_pred);
  return unique_copy(__first, __last, __first, __binary_pred);
}

// reverse and reverse_copy, and their auxiliary functions
// reverse 用來反轉 元素順序。
template <class _BidirectionalIter>
void __reverse(_BidirectionalIter __first, _BidirectionalIter __last, 	// 這裡用的是 雙向迭代器
               bidirectional_iterator_tag) {
  while (true)
    if (__first == __last || __first == --__last)		// 如果 區間中只有一個元素 或者沒有 元素 ，(其中last要減一指向了最後一個元素)。，直接return
      return;
    else
      iter_swap(__first++, __last);			// 否則 交換 頭尾兩個元素，并將起始迭代器向下移動。！而尾部迭代器在上面的 if 判斷裡面已經向前移動了一個了。
}
// 對於隨機存取 迭代器，可以直接判斷。
template <class _RandomAccessIter>
void __reverse(_RandomAccessIter __first, _RandomAccessIter __last,
               random_access_iterator_tag) {
  while (__first < __last)
    iter_swap(__first++, --__last);
}

template <class _BidirectionalIter>
inline void reverse(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  __reverse(__first, __last, __ITERATOR_CATEGORY(__first));
}
//copy的話需要一個額外的result來存儲，直接反向遍歷容器即可。
template <class _BidirectionalIter, class _OutputIter>
_OutputIter reverse_copy(_BidirectionalIter __first,
                         _BidirectionalIter __last,
                         _OutputIter __result) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  while (__first != __last) {
    --__last;
    *__result = *__last;
    ++__result;
  }
  return __result;
}

// rotate and rotate_copy, and their auxiliary functions
// gcd用來求最大公約數，，並未使用我們常用的遞歸，而是使用while循環，同樣是輾轉相除法。。。
template <class _EuclideanRingElement>
_EuclideanRingElement __gcd(_EuclideanRingElement __m,
                            _EuclideanRingElement __n)
{
  while (__n != 0) {
    _EuclideanRingElement __t = __m % __n;
    __m = __n;
    __n = __t;
  }
  return __m;
}
// rotate ，以middle為中心對容器進行移位。相當於交換區間first,middle-1 和區間 middle,last-1
// 進行循環移位，將整個區間左移middle-1 位。

//第一個版本，ForwardIter 迭代器。
// 進行的是 分段交換，按照前middle-1 長度進行分段，，然後分別把後面的段交換到前面，最後對最一段進行反轉，使其恢復原來的順序（這種情況只是最後一段不足middle-1時）。
template <class _ForwardIter, class _Distance>
_ForwardIter __rotate(_ForwardIter __first,
                      _ForwardIter __middle,
                      _ForwardIter __last,
                      _Distance*,
                      forward_iterator_tag) {
  if (__first == __middle)	// 如果rotate點是first，相當於不變，直接返回last。
    return __last;
  if (__last  == __middle)	// 如果中心是last。。返回first，相當於把first移動到了last的位置，first是新的last。
    return __first;

  _ForwardIter __first2 = __middle;			// first2指向middle，也就是first要移動到的位置。
  do {
    swap(*__first++, *__first2++);		// 交換first1和first2
    if (__first == __middle)			//如果first1遇到了middle，也就是前面的的已經全移動到了後面去了。
      __middle = __first2;		// 相當於進行了 分段 交換。
  } while (__first2 != __last);

  _ForwardIter __new_middle = __first;		//new_middle指向倒數 第middle-1個元素  比如說 最後兩段是這樣的：1 2 3 4   7 8然後middle是5 交換之後得到 7 8 3 4   1 2 此時的first就指向3，new_middle也就指向3.

  __first2 = __middle;		// 上面那種情況下。first2指向 後來的 1，也就是原來的 7

  while (__first2 != __last) {	//  最後要使得倒數middle-1位恢復原來的順序。
    swap (*__first++, *__first2++);	//	3 4  1  2  ==>   1 4 3 2 ==> 1 2 3 4
    if (__first == __middle)		// 這個位置和上面一樣，如果first到middle了，就到下一段。
      __middle = __first2;	
    else if (__first2 == __last)		// 如果first2指向最後了，但是first沒有指向middle，說明前面還有一段，所以first2要變回middle。
      __first2 = __middle;
  }
		// 關於後面這段我們有三種情況，一是上面說的，3 4    1 2  正好長度一樣，所以first是middle的時候first2也指向last了，但是if執行了，else if就不會執行了。所以會正好交換回來
		// 第二種情況是 1 2 3 4 5    7 8 ==>  7 8 3 4 5   1 2 ==>  3 4 5   1 2  ==> 1 2 5     3    4(這裡就是first未指向middle，而first2已經到last了，所以if不會執行，而else if 會執行，使first2回到middle，也就是指向3，把後面的3 4 重新移回到前面) ==> 1 2 3  5 4(這個時候first==middle了，所以middle指向了4)==> 1 2 3 4 5(first正好到middle，而且first2也已經到last了，結束)
		// 如果出現了 1 2 3 4 5   6 7 8 9  ==> 6 7 8 9 5   1 2 3 4==> 5   1 2 3 4==> 1  5 2 3 4 (first指向1，到了middle；first2指向2，middle更新為2)==>1 2   5 3 4(first 又到middle了，同上，繼續更新，直到) ====>  1 2 3 4 5 
  return __new_middle;
}

// 第二個版本  雙向迭代器
// 總體思路就是對兩部分分別進行逆序，然後前後交換，，剩餘部分再逆序，就變回原來的樣子了，同時完成了兩個區間的交換。
template <class _BidirectionalIter, class _Distance>
_BidirectionalIter __rotate(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance*,
                            bidirectional_iterator_tag) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  if (__first == __middle)
    return __last;
  if (__last  == __middle)
    return __first;
// 1 2 3 4 5 6 7 8 middle=4
  __reverse(__first,  __middle, bidirectional_iterator_tag());	//[first,middle) reverse 成逆序 3 2 1
  __reverse(__middle, __last,   bidirectional_iterator_tag());	// [middle,last) reverse 成逆序 8 7 6 5 4 

  while (__first != __middle && __middle != __last)  // 把前後兩段從外向內進行交換，4 5 6   8(first,middle) 7(last)   1 2 3
    swap (*__first++, *--__last);

  if (__first == __middle) {	//  如果first==middle說明後面那一段比較長， 對後面 剩餘部分進行reverse逆序。
    __reverse(__middle, __last,   bidirectional_iterator_tag());	// 4 5 6 7 8 1 2 3
    return __last;
  }
  else {			//  否則說明前面比較長，逆序前面那一部分。。
    __reverse(__first,  __middle, bidirectional_iterator_tag());
    return __first;
  }
}
//  第三個版本，隨機存取迭代器。
// 找出前一段长度和总长度的最大公约数，在假设后一段长度长的情况下，可以得出，只要循环公约数次，每次把后面的 l/d 个元素移动到前面去，就可以在d次循环结束后把后面l个全部移动到前面去。对于前面长的情况同样。！！
template <class _RandomAccessIter, class _Distance, class _Tp>
_RandomAccessIter __rotate(_RandomAccessIter __first,
                           _RandomAccessIter __middle,
                           _RandomAccessIter __last,
                           _Distance *, _Tp *) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  _Distance __n = __last   - __first;	// 計算區間長度。
  _Distance __k = __middle - __first;	// 計算第一個部分長度
  _Distance __l = __n - __k;			// 計算第二個部分長度
  _RandomAccessIter __result = __first + (__last - __middle); //  result 指向了  rotate完成之后  原先first的现在的位置。

  if (__k == 0)		// 第一部分為零，表示middle是first，返回last
    return __last;

  else if (__k == __l) {		//  if前後兩部分長度一樣，直接 swap_ranges 交換區間即可。把[first,middle)跟[middle,last)交換。
    swap_ranges(__first, __middle, __middle);
    return __result;
  }
	// 1 2 3 4     5(middle) 6 7 8 9 10 11 12 13 14
  _Distance __d = __gcd(__n, __k);	//  d是 區間長度和 第一部分长度 的最大公约数  // d=2
	// 求出最大公约数的目的就是方便 按照其来进行分组交换。
  for (_Distance __i = 0; __i < __d; __i++) {	
    _Tp __tmp = *__first;
    _RandomAccessIter __p = __first;

    if (__k < __l) {		// 如果第二部分比较长，
      for (_Distance __j = 0; __j < __l/__d; __j++) {	//  __l/__d计算出后一部分有几个长度为公约数的部分，这里可以保证一定是整除的。
	  // 证明 l+k=n   根据gcd。。l=x1*d  n=x2*d (x2>x1) 	那么k= n-l=(x2-x1)*d 必然可以整除d
	  //  j<5
	  //  如果p大于first+l，说明后面p+k就越界了，所以要往前找。p-l。！！！！
        if (__p > __first + __l) {	// 5 2 3 4 9 6 7 8 13 10 11 12 3(p-l) 14	
          *__p = *(__p - __l);			// p==>第三个位置
          __p -= __l;
        }

        *__p = *(__p + __k);	// 5 2 3 4 5 6 7.....//5 2 3 4 9 6 7 8 9....//5 2 3 4 9 6 7 8 13 10 11 12 13 14//
								//	 这里不管前面约不越界都要执行的，，，5 2 7 4 9 6 7 8 13 10 11 12 3 14
        __p += __k;				// 5 2 7 4 9 6 11 8 13 10 11 12 3 14
      }
    }

    else {
      for (_Distance __j = 0; __j < __k/__d - 1; __j ++) {
        if (__p < __last - __k) {
          *__p = *(__p + __k);
          __p += __k;
        }

        *__p = * (__p - __l);
        __p -= __l;
      }
    }

    *__p = __tmp;	// 5 2 7 4 9 6 11 8 13 10 1 12 3 14
    ++__first;		// first ==》 第2个位置。
  }
/*
tmp=2
5 6()  7 4 9 6 11 8 13 10 1 12 3 14
5 6 7 4 9 10()  11 8 13 10 1 12 3 14
5 6 7 4 9 10 11 8 13 14() 1 12 3 14

这两个合起来是第四步：5 6 7 4 9 10 11 8 13 14 1 12 3 4()
					  5 6 7 8()  9 10 11 8 13 14 1 12 3 4

第5步5 6 7 8 9 10 11 12 13 14 1 12 3 4

5 6 7 8 9 10 11 12 13 14 1 2 3 4

*/
  return __result;
}

template <class _ForwardIter>
inline _ForwardIter rotate(_ForwardIter __first, _ForwardIter __middle,
                           _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  return __rotate(__first, __middle, __last,
                  __DISTANCE_TYPE(__first),
                  __ITERATOR_CATEGORY(__first));
}

template <class _ForwardIter, class _OutputIter>
_OutputIter rotate_copy(_ForwardIter __first, _ForwardIter __middle,
                        _ForwardIter __last, _OutputIter __result) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  return copy(__first, __middle, copy(__middle, __last, __result));
}

// Return a random number in the range [0, __n).  This function encapsulates
// whether we're using rand (part of the standard C library) or lrand48
// (not standard, but a much better choice whenever it's available).

template <class _Distance>
inline _Distance __random_number(_Distance __n) {//产生0到n的随机数
#ifdef __STL_NO_DRAND48
  return rand() % __n;
#else
  return lrand48() % __n;
#endif
}

// random_shuffle
//random_shuffle()用来对一个元素序列进行重新排序（随机的)
template <class _RandomAccessIter>
inline void random_shuffle(_RandomAccessIter __first,
                           _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  if (__first == __last) return;
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
    iter_swap(__i, __first + __random_number((__i - __first) + 1));
}

template <class _RandomAccessIter, class _RandomNumberGenerator>
void random_shuffle(_RandomAccessIter __first, _RandomAccessIter __last,
                    _RandomNumberGenerator& __rand) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  if (__first == __last) return;
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
    iter_swap(__i, __first + __rand((__i - __first) + 1));
}

// random_sample and random_sample_n (extensions, not part of the standard).
//它能够随机将[first,last)中的某些元素复制到[out,out+n)中，它会复制m个元素，m为min{last-first,n}，以均匀分布的方式取样
//random_sample_n会保持元素的相对顺序。
template <class _ForwardIter, class _OutputIter, class _Distance>
_OutputIter random_sample_n(_ForwardIter __first, _ForwardIter __last,
                            _OutputIter __out, const _Distance __n)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  _Distance __remaining = 0;
  distance(__first, __last, __remaining);
  _Distance __m = min(__n, __remaining);

  while (__m > 0) {
    if (__random_number(__remaining) < __m) {//只有产生小于m的随机数，才会往out里加入一个元素。而且是按照原来的相对顺序来的。
      *__out = *__first;					// 因为first 是从头开始遍历++的，所以相对顺序不变。（有可能跳过某些元素）
      ++__out;
      --__m;
    }

    --__remaining;
    ++__first;
  }
  return __out;
}

template <class _ForwardIter, class _OutputIter, class _Distance,
          class _RandomNumberGenerator>
_OutputIter random_sample_n(_ForwardIter __first, _ForwardIter __last,
                            _OutputIter __out, const _Distance __n,
                            _RandomNumberGenerator& __rand)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_UNARY_FUNCTION_CHECK(_RandomNumberGenerator, _Distance, _Distance);
  _Distance __remaining = 0;
  distance(__first, __last, __remaining);
  _Distance __m = min(__n, __remaining);

  while (__m > 0) {
    if (__rand(__remaining) < __m) {
      *__out = *__first;
      ++__out;
      --__m;
    }

    --__remaining;
    ++__first;
  }
  return __out;
}
//random_sample无法保持元素的相对顺序
template <class _InputIter, class _RandomAccessIter, class _Distance>
_RandomAccessIter __random_sample(_InputIter __first, _InputIter __last,
                                  _RandomAccessIter __out,
                                  const _Distance __n)
{
  _Distance __m = 0;
  _Distance __t = __n;
  for ( ; __first != __last && __m < __n; ++__m, ++__first) //先复制前n个元素到out中。
    __out[__m] = *__first;

  while (__first != __last) {
    ++__t;
    _Distance __M = __random_number(__t);	//  产生随机数，更新该下标的元素。
    if (__M < __n)		// 如果在范围内的话。
      __out[__M] = *__first;
    ++__first;
  }

  return __out + __m;
}

template <class _InputIter, class _RandomAccessIter,
          class _RandomNumberGenerator, class _Distance>
_RandomAccessIter __random_sample(_InputIter __first, _InputIter __last,
                                  _RandomAccessIter __out,
                                  _RandomNumberGenerator& __rand,
                                  const _Distance __n)
{
  __STL_UNARY_FUNCTION_CHECK(_RandomNumberGenerator, _Distance, _Distance);
  _Distance __m = 0;
  _Distance __t = __n;
  for ( ; __first != __last && __m < __n; ++__m, ++__first)
    __out[__m] = *__first;

  while (__first != __last) {
    ++__t;
    _Distance __M = __rand(__t);
    if (__M < __n)
      __out[__M] = *__first;
    ++__first;
  }

  return __out + __m;
}

template <class _InputIter, class _RandomAccessIter>
inline _RandomAccessIter
random_sample(_InputIter __first, _InputIter __last,
              _RandomAccessIter __out_first, _RandomAccessIter __out_last) 
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  return __random_sample(__first, __last,
                         __out_first, __out_last - __out_first);
}


template <class _InputIter, class _RandomAccessIter, 
          class _RandomNumberGenerator>
inline _RandomAccessIter
random_sample(_InputIter __first, _InputIter __last,
              _RandomAccessIter __out_first, _RandomAccessIter __out_last,
              _RandomNumberGenerator& __rand) 
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  return __random_sample(__first, __last,
                         __out_first, __rand,
                         __out_last - __out_first);
}

// partition, stable_partition, and their auxiliary functions
// 分割
template <class _ForwardIter, class _Predicate>
_ForwardIter __partition(_ForwardIter __first,
		         _ForwardIter __last,
			 _Predicate   __pred,
			 forward_iterator_tag) {
  if (__first == __last) return __first;

  while (__pred(*__first))	//分割出满足条件的元素，
    if (++__first == __last) return __first;

  _ForwardIter __next = __first;

  while (++__next != __last)		// 对于后面的元素，和前面的first交换，最终将满足条件的元素全部移动到前面。
    if (__pred(*__next)) {
      swap(*__first, *__next);
      ++__first;
    }

  return __first;
}

template <class _BidirectionalIter, class _Predicate>
_BidirectionalIter __partition(_BidirectionalIter __first,
                               _BidirectionalIter __last,
			       _Predicate __pred,
			       bidirectional_iterator_tag) {
  while (true) {
    while (true)
      if (__first == __last)
        return __first;
      else if (__pred(*__first))
        ++__first;
      else
        break;	//  找到前面一段满足pred的元素
    --__last;
    while (true)	//从后往前找，对于不满足的元素跳过。
      if (__first == __last)
        return __first;
      else if (!__pred(*__last))
        --__last;
      else		// 找到最后一个满足的元素
        break;
    iter_swap(__first, __last);		// 和前面的first交换，再次循环，直到遍历完所有。
    ++__first;
  }
}

template <class _ForwardIter, class _Predicate>
inline _ForwardIter partition(_ForwardIter __first,
   			      _ForwardIter __last,
			      _Predicate   __pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool, 
        typename iterator_traits<_ForwardIter>::value_type);
  return __partition(__first, __last, __pred, __ITERATOR_CATEGORY(__first));
}


template <class _ForwardIter, class _Predicate, class _Distance>
_ForwardIter __inplace_stable_partition(_ForwardIter __first,
                                        _ForwardIter __last,
                                        _Predicate __pred, _Distance __len) {
  if (__len == 1)
    return __pred(*__first) ? __last : __first;	//	如果长度为1，若第一个满足，则返回last，否则返回first。
  _ForwardIter __middle = __first;
  advance(__middle, __len / 2);	// middle指向中间
  return rotate(__inplace_stable_partition(__first, __middle, __pred, 		// 进行二分对前一部分和后一部分分别分割，然后rotate移位。把前一部分不满足的和后一部分满足的进行交换。满足的就都在前面了。
                                           __len / 2),
                __middle,
                __inplace_stable_partition(__middle, __last, __pred,
                                           __len - __len / 2));
}

template <class _ForwardIter, class _Pointer, class _Predicate, 
          class _Distance>
_ForwardIter __stable_partition_adaptive(_ForwardIter __first,
                                         _ForwardIter __last,
                                         _Predicate __pred, _Distance __len,
                                         _Pointer __buffer,
                                         _Distance __buffer_size) 
{
  if (__len <= __buffer_size) {	//如果需要分割的区间长度小于缓冲区长度。
    _ForwardIter __result1 = __first;
    _Pointer __result2 = __buffer;
    for ( ; __first != __last ; ++__first)	// 循环遍历
      if (__pred(*__first)) {	// 如果满足pred，则放到result1里面
        *__result1 = *__first;
        ++__result1;
      }
      else {
        *__result2 = *__first;		// 否则放到result2里面。
        ++__result2;
      }
    copy(__buffer, __result2, __result1);	// 最后把result2中的元素拷贝到result1后面去。
    return __result1;
  }
  else {	// 缓冲区存不下的话，跟上个函数一样，二分+rotate移位。
    _ForwardIter __middle = __first;	
    advance(__middle, __len / 2);
    return rotate(__stable_partition_adaptive(
                          __first, __middle, __pred,
                          __len / 2, __buffer, __buffer_size),
                    __middle,
                    __stable_partition_adaptive(
                          __middle, __last, __pred,
                          __len - __len / 2, __buffer, __buffer_size));
  }
}

template <class _ForwardIter, class _Predicate, class _Tp, class _Distance>
inline _ForwardIter
__stable_partition_aux(_ForwardIter __first, _ForwardIter __last, 
                       _Predicate __pred, _Tp*, _Distance*)
{
  _Temporary_buffer<_ForwardIter, _Tp> __buf(__first, __last);
  if (__buf.size() > 0)
    return __stable_partition_adaptive(__first, __last, __pred,
                                       _Distance(__buf.requested_size()),
                                       __buf.begin(), __buf.size());
  else
    return __inplace_stable_partition(__first, __last, __pred, 
                                      _Distance(__buf.requested_size()));
}

template <class _ForwardIter, class _Predicate>
inline _ForwardIter stable_partition(_ForwardIter __first,
                                     _ForwardIter __last, 
                                     _Predicate __pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
      typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last)
    return __first;
  else
    return __stable_partition_aux(__first, __last, __pred,
                                  __VALUE_TYPE(__first),
                                  __DISTANCE_TYPE(__first));
}
// 把比pivot小的元素移到前面，大的移到后面。也就是按照pivot来分割。
template <class _RandomAccessIter, class _Tp>
_RandomAccessIter __unguarded_partition(_RandomAccessIter __first, 
                                        _RandomAccessIter __last, 
                                        _Tp __pivot) 
{
  while (true) {
    while (*__first < __pivot)	// first找到第一个比pivot的地方
      ++__first;
    --__last;	
    while (__pivot < *__last)	// 从后往前找到第一个比pivot小的地方，
      --__last;
    if (!(__first < __last))	// 如果已遍历完，返回first
      return __first;
    iter_swap(__first, __last);		// 交换这两个元素
    ++__first;			// first指向下一个元素
  }
}    
// 这里把大于小于变成了函数判断，，一样的道理
template <class _RandomAccessIter, class _Tp, class _Compare>
_RandomAccessIter __unguarded_partition(_RandomAccessIter __first, 
                                        _RandomAccessIter __last, 
                                        _Tp __pivot, _Compare __comp) 
{
  while (true) {
    while (__comp(*__first, __pivot))
      ++__first;
    --__last;
    while (__comp(__pivot, *__last))
      --__last;
    if (!(__first < __last))
      return __first;
    iter_swap(__first, __last);
    ++__first;
  }
}

const int __stl_threshold = 16;

// sort() and its auxiliary functions. 

// 对随机存取迭代器的插入操作 // 这里是从小到大排列有序的容器
template <class _RandomAccessIter, class _Tp>
void __unguarded_linear_insert(_RandomAccessIter __last, _Tp __val) {
  _RandomAccessIter __next = __last;
  --__next;	// next指向最后一个元素
  while (__val < *__next) {	// 如果val小于当前值，则把这一段后移
    *__last = *__next;		// 后移
    __last = __next;
    --__next;		//next向前移动
  }
  *__last = __val;	//last指向了最后要插入的位置
}
// 比较换成了函数判断。
template <class _RandomAccessIter, class _Tp, class _Compare>
void __unguarded_linear_insert(_RandomAccessIter __last, _Tp __val, 
                               _Compare __comp) {
  _RandomAccessIter __next = __last;
  --__next;  
  while (__comp(__val, *__next)) {
    *__last = *__next;
    __last = __next;
    --__next;
  }
  *__last = __val;
}
// 对最后一个元素进行插入。
template <class _RandomAccessIter, class _Tp>
inline void __linear_insert(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Tp*) {
  _Tp __val = *__last;
  if (__val < *__first) {	// 如果最后一个元素小于第一个，把整体向后拷贝一个位置，把最后一个移到前面来。
    copy_backward(__first, __last, __last + 1);
    *__first = __val;
  }
  else	// 否则 执行__unguarded_linear_insert将这个值插入到正确位置。。
    __unguarded_linear_insert(__last, __val);
}
// 换成函数比较
template <class _RandomAccessIter, class _Tp, class _Compare>
inline void __linear_insert(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Tp*, _Compare __comp) {
  _Tp __val = *__last;
  if (__comp(__val, *__first)) {
    copy_backward(__first, __last, __last + 1);
    *__first = __val;
  }
  else
    __unguarded_linear_insert(__last, __val, __comp);
}

template <class _RandomAccessIter>
void __insertion_sort(_RandomAccessIter __first, _RandomAccessIter __last) {
  if (__first == __last) return; 
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
    __linear_insert(__first, __i, __VALUE_TYPE(__first));
}
// 插入排序。
template <class _RandomAccessIter, class _Compare>
void __insertion_sort(_RandomAccessIter __first,
                      _RandomAccessIter __last, _Compare __comp) {
  if (__first == __last) return;
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
    __linear_insert(__first, __i, __VALUE_TYPE(__first), __comp);	// 对后面每一个元素，执行插入操作，相当于实现了插入排序。
																	// 这里是每次把最后一个元素进行插入。
}
// 随机存取迭代器的插入排序。
template <class _RandomAccessIter, class _Tp>
void __unguarded_insertion_sort_aux(_RandomAccessIter __first, 
                                    _RandomAccessIter __last, _Tp*) {
  for (_RandomAccessIter __i = __first; __i != __last; ++__i)
    __unguarded_linear_insert(__i, _Tp(*__i));
}

template <class _RandomAccessIter>
inline void __unguarded_insertion_sort(_RandomAccessIter __first, 
                                _RandomAccessIter __last) {
  __unguarded_insertion_sort_aux(__first, __last, __VALUE_TYPE(__first));
}

template <class _RandomAccessIter, class _Tp, class _Compare>
void __unguarded_insertion_sort_aux(_RandomAccessIter __first, 
                                    _RandomAccessIter __last,
                                    _Tp*, _Compare __comp) {
  for (_RandomAccessIter __i = __first; __i != __last; ++__i)
    __unguarded_linear_insert(__i, _Tp(*__i), __comp);
}

template <class _RandomAccessIter, class _Compare>
inline void __unguarded_insertion_sort(_RandomAccessIter __first, 
                                       _RandomAccessIter __last,
                                       _Compare __comp) {
  __unguarded_insertion_sort_aux(__first, __last, __VALUE_TYPE(__first),
                                 __comp);
}
// 最终的插入排序。
template <class _RandomAccessIter>
void __final_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last) {
  if (__last - __first > __stl_threshold) {
    __insertion_sort(__first, __first + __stl_threshold);
    __unguarded_insertion_sort(__first + __stl_threshold, __last);
  }
  else
    __insertion_sort(__first, __last);
}

template <class _RandomAccessIter, class _Compare>
void __final_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Compare __comp) {
  if (__last - __first > __stl_threshold) {
    __insertion_sort(__first, __first + __stl_threshold, __comp);
    __unguarded_insertion_sort(__first + __stl_threshold, __last, __comp);
  }
  else
    __insertion_sort(__first, __last, __comp);
}

template <class _Size>
inline _Size __lg(_Size __n) {	// 返回n的   二进制的位数-1 
  _Size __k;
  for (__k = 0; __n != 1; __n >>= 1) ++__k;
  return __k;
}

template <class _RandomAccessIter, class _Tp, class _Size>
void __introsort_loop(_RandomAccessIter __first,
                      _RandomAccessIter __last, _Tp*,
                      _Size __depth_limit)
{
  while (__last - __first > __stl_threshold) {	//当区间长度大于16时
    if (__depth_limit == 0) {		// 如果深度限制是0
      partial_sort(__first, __last, __last);		// 直接调用partial_sort进行排序
      return;
    }
    --__depth_limit;	// 深度减一
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2),	// median返回三个元素中间的那个。
                                         *(__last - 1))));
										 //也就是取第一个，中间，最后一个这三个元素中 中间的那个当做哨兵进行分割。
    __introsort_loop(__cut, __last, (_Tp*) 0, __depth_limit);	// 递归调用，对后面的排好序。
    __last = __cut;		// 让last指向刚刚中间的位置，以实现对前面的排序。
  }
}
// 比较换成了函数判断。
template <class _RandomAccessIter, class _Tp, class _Size, class _Compare>
void __introsort_loop(_RandomAccessIter __first,
                      _RandomAccessIter __last, _Tp*,
                      _Size __depth_limit, _Compare __comp)
{
  while (__last - __first > __stl_threshold) {		// >16？
    if (__depth_limit == 0) {
      partial_sort(__first, __last, __last, __comp);
      return;
    }
    --__depth_limit;
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2),
                                         *(__last - 1), __comp)),
       __comp);
    __introsort_loop(__cut, __last, (_Tp*) 0, __depth_limit, __comp);
    __last = __cut;
  }
}
//  快排的实现！！！
template <class _RandomAccessIter>
inline void sort(_RandomAccessIter __first, _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  if (__first != __last) {
    __introsort_loop(__first, __last,
                     __VALUE_TYPE(__first),
                     __lg(__last - __first) * 2);	// 深度限制为区间长度的二进制位数-1的2倍。
    __final_insertion_sort(__first, __last);		// 对总体有序的区间进行插入排序。总体有序是因为他前面设定了一个门槛是16，长度小于16 不执行loop了。
  }
}

template <class _RandomAccessIter, class _Compare>
inline void sort(_RandomAccessIter __first, _RandomAccessIter __last,
                 _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_RandomAccessIter>::value_type,
       typename iterator_traits<_RandomAccessIter>::value_type);
  if (__first != __last) {
    __introsort_loop(__first, __last,
                     __VALUE_TYPE(__first),
                     __lg(__last - __first) * 2,
                     __comp);
    __final_insertion_sort(__first, __last, __comp);
  }
}

// stable_sort() and its auxiliary functions.
// 稳定排序。
template <class _RandomAccessIter>
void __inplace_stable_sort(_RandomAccessIter __first,
                           _RandomAccessIter __last) {
  if (__last - __first < 15) {		//  长度小于15使用插入排序
    __insertion_sort(__first, __last);
    return;
  }
  _RandomAccessIter __middle = __first + (__last - __first) / 2;
  __inplace_stable_sort(__first, __middle);	// 归并排序
  __inplace_stable_sort(__middle, __last);
  __merge_without_buffer(__first, __middle, __last,		// 合并的过程。
                         __middle - __first,
                         __last - __middle);
}
// 把比较换成了comp
template <class _RandomAccessIter, class _Compare>
void __inplace_stable_sort(_RandomAccessIter __first,
                           _RandomAccessIter __last, _Compare __comp) {
  if (__last - __first < 15) {
    __insertion_sort(__first, __last, __comp);
    return;
  }
  _RandomAccessIter __middle = __first + (__last - __first) / 2;
  __inplace_stable_sort(__first, __middle, __comp);
  __inplace_stable_sort(__middle, __last, __comp);
  __merge_without_buffer(__first, __middle, __last,
                         __middle - __first,
                         __last - __middle,
                         __comp);
}
// 
template <class _RandomAccessIter1, class _RandomAccessIter2,
          class _Distance>
void __merge_sort_loop(_RandomAccessIter1 __first,
                       _RandomAccessIter1 __last, 
                       _RandomAccessIter2 __result, _Distance __step_size) {
  _Distance __two_step = 2 * __step_size;		//two_step是两倍的step_size

  while (__last - __first >= __two_step) {		// 长度大于两倍的步长时
    __result = merge(__first, __first + __step_size,		// merge，合并两个步长的元素。
                     __first + __step_size, __first + __two_step,
                     __result);
    __first += __two_step;		// 对下面两个步长进行操作。
  }

  __step_size = min(_Distance(__last - __first), __step_size);	// 对于剩下不足两个步长的部分 ，如果也不足一个步长，就更新步长为剩余长度。
  merge(__first, __first + __step_size, __first + __step_size, __last,		// 合并。
        __result);
}
// comp函数比较
template <class _RandomAccessIter1, class _RandomAccessIter2,
          class _Distance, class _Compare>
void __merge_sort_loop(_RandomAccessIter1 __first,
                       _RandomAccessIter1 __last, 
                       _RandomAccessIter2 __result, _Distance __step_size,
                       _Compare __comp) {
  _Distance __two_step = 2 * __step_size;

  while (__last - __first >= __two_step) {
    __result = merge(__first, __first + __step_size,
                     __first + __step_size, __first + __two_step,
                     __result,
                     __comp);
    __first += __two_step;
  }
  __step_size = min(_Distance(__last - __first), __step_size);

  merge(__first, __first + __step_size,
        __first + __step_size, __last,
        __result,
        __comp);
}

const int __stl_chunk_size = 7;
// chunk 插入排序   就是分了个段 然后插入排序？？？？？？？？？？？？     
template <class _RandomAccessIter, class _Distance>
void __chunk_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Distance __chunk_size)
{
  while (__last - __first >= __chunk_size) {		// 分段插入排序。
    __insertion_sort(__first, __first + __chunk_size);
    __first += __chunk_size;
  }
  __insertion_sort(__first, __last);
}

template <class _RandomAccessIter, class _Distance, class _Compare>
void __chunk_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last,
                            _Distance __chunk_size, _Compare __comp)
{
  while (__last - __first >= __chunk_size) {
    __insertion_sort(__first, __first + __chunk_size, __comp);
    __first += __chunk_size;
  }
  __insertion_sort(__first, __last, __comp);
}
//　有缓冲区的归并排序
template <class _RandomAccessIter, class _Pointer, class _Distance>
void __merge_sort_with_buffer(_RandomAccessIter __first, 
                              _RandomAccessIter __last,
                              _Pointer __buffer, _Distance*) {
  _Distance __len = __last - __first;		//len表示区间长度。
  _Pointer __buffer_last = __buffer + __len;	// buffer的最后。

  _Distance __step_size = __stl_chunk_size;		// 步长为7
  __chunk_insertion_sort(__first, __last, __step_size);		// 对其进行分段插入排序、
	// 相当于有两个缓冲区，分别进行合并，最后就是一个有序的区间了。
  while (__step_size < __len) {			
    __merge_sort_loop(__first, __last, __buffer, __step_size);	// 对排好序的分段，进行两两合并。
    __step_size *= 2;		// 步长加倍。
    __merge_sort_loop(__buffer, __buffer_last, __first, __step_size);	//对加倍后的步长区间进行两两合并
    __step_size *= 2;		//再次加倍。
  }
}
// compare！
template <class _RandomAccessIter, class _Pointer, class _Distance,
          class _Compare>
void __merge_sort_with_buffer(_RandomAccessIter __first, 
                              _RandomAccessIter __last, _Pointer __buffer,
                              _Distance*, _Compare __comp) {
  _Distance __len = __last - __first;
  _Pointer __buffer_last = __buffer + __len;

  _Distance __step_size = __stl_chunk_size;
  __chunk_insertion_sort(__first, __last, __step_size, __comp);

  while (__step_size < __len) {
    __merge_sort_loop(__first, __last, __buffer, __step_size, __comp);
    __step_size *= 2;
    __merge_sort_loop(__buffer, __buffer_last, __first, __step_size, __comp);
    __step_size *= 2;
  }
}
// 有适应性的稳定排序。。。。。
template <class _RandomAccessIter, class _Pointer, class _Distance>
void __stable_sort_adaptive(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Pointer __buffer,
                            _Distance __buffer_size) {
  _Distance __len = (__last - __first + 1) / 2;
  _RandomAccessIter __middle = __first + __len;
  if (__len > __buffer_size) {	// 如果缓冲区不足以存下一半区间，则分别对前后两个区间排序。
    __stable_sort_adaptive(__first, __middle, __buffer, __buffer_size);
    __stable_sort_adaptive(__middle, __last, __buffer, __buffer_size);
  }
  else {		// 否则的话，调用前面的有缓冲区的归并排序，对前后两个区间分别排序、
    __merge_sort_with_buffer(__first, __middle, __buffer, (_Distance*)0);
    __merge_sort_with_buffer(__middle, __last, __buffer, (_Distance*)0);
  }
  __merge_adaptive(__first, __middle, __last, _Distance(__middle - __first), 	// 	最后对两个区间进行合并。。。
                   _Distance(__last - __middle), __buffer, __buffer_size);
}
// compare
template <class _RandomAccessIter, class _Pointer, class _Distance, 
          class _Compare>
void __stable_sort_adaptive(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Pointer __buffer,
                            _Distance __buffer_size, _Compare __comp) {
  _Distance __len = (__last - __first + 1) / 2;
  _RandomAccessIter __middle = __first + __len;
  if (__len > __buffer_size) {
    __stable_sort_adaptive(__first, __middle, __buffer, __buffer_size, 
                           __comp);
    __stable_sort_adaptive(__middle, __last, __buffer, __buffer_size, 
                           __comp);
  }
  else {
    __merge_sort_with_buffer(__first, __middle, __buffer, (_Distance*)0,
                               __comp);
    __merge_sort_with_buffer(__middle, __last, __buffer, (_Distance*)0,
                               __comp);
  }
  __merge_adaptive(__first, __middle, __last, _Distance(__middle - __first), 
                   _Distance(__last - __middle), __buffer, __buffer_size,
                   __comp);
}
// 所以说这个排序是啥意思？？？？？

template <class _RandomAccessIter, class _Tp, class _Distance>
inline void __stable_sort_aux(_RandomAccessIter __first,
                              _RandomAccessIter __last, _Tp*, _Distance*) {
  _Temporary_buffer<_RandomAccessIter, _Tp> buf(__first, __last);
  if (buf.begin() == 0)
    __inplace_stable_sort(__first, __last);
  else 
    __stable_sort_adaptive(__first, __last, buf.begin(),
                           _Distance(buf.size()));
}

template <class _RandomAccessIter, class _Tp, class _Distance, class _Compare>
inline void __stable_sort_aux(_RandomAccessIter __first,
                              _RandomAccessIter __last, _Tp*, _Distance*,
                              _Compare __comp) {
  _Temporary_buffer<_RandomAccessIter, _Tp> buf(__first, __last);
  if (buf.begin() == 0)
    __inplace_stable_sort(__first, __last, __comp);
  else 
    __stable_sort_adaptive(__first, __last, buf.begin(),
                           _Distance(buf.size()),
                           __comp);
}
// 内联函数，sort
template <class _RandomAccessIter>
inline void stable_sort(_RandomAccessIter __first,
                        _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __stable_sort_aux(__first, __last,
                    __VALUE_TYPE(__first),
                    __DISTANCE_TYPE(__first));
}

template <class _RandomAccessIter, class _Compare>
inline void stable_sort(_RandomAccessIter __first,
                        _RandomAccessIter __last, _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_RandomAccessIter>::value_type,
       typename iterator_traits<_RandomAccessIter>::value_type);
  __stable_sort_aux(__first, __last,
                    __VALUE_TYPE(__first),
                    __DISTANCE_TYPE(__first), 
                    __comp);
}

// partial_sort, partial_sort_copy, and auxiliary functions.
// 局部排序，对first到middle-1进行排序，
template <class _RandomAccessIter, class _Tp>
void __partial_sort(_RandomAccessIter __first, _RandomAccessIter __middle,
                    _RandomAccessIter __last, _Tp*) {
  make_heap(__first, __middle);		// 对前middle-1个建堆，这个堆是个最大堆，也就是前面的元素是最大的。
  for (_RandomAccessIter __i = __middle; __i < __last; ++__i)		// 对于后面部分的元素，如果比前面最大元素要小，说明它应该是我们需要排序进来的部分，就把第一个元素pop出来，这样就可以得到下个最大值。然后把第i个加进去。
    if (*__i < *__first) 
      __pop_heap(__first, __middle, __i, _Tp(*__i),
                 __DISTANCE_TYPE(__first));
  sort_heap(__first, __middle);		// 对堆进行排序。
}

template <class _RandomAccessIter>
inline void partial_sort(_RandomAccessIter __first,
                         _RandomAccessIter __middle,
                         _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __partial_sort(__first, __middle, __last, __VALUE_TYPE(__first));
}
// compare
template <class _RandomAccessIter, class _Tp, class _Compare>
void __partial_sort(_RandomAccessIter __first, _RandomAccessIter __middle,
                    _RandomAccessIter __last, _Tp*, _Compare __comp) {
  make_heap(__first, __middle, __comp);
  for (_RandomAccessIter __i = __middle; __i < __last; ++__i)
    if (__comp(*__i, *__first))
      __pop_heap(__first, __middle, __i, _Tp(*__i), __comp,
                 __DISTANCE_TYPE(__first));
  sort_heap(__first, __middle, __comp);
}

template <class _RandomAccessIter, class _Compare>
inline void partial_sort(_RandomAccessIter __first,
                         _RandomAccessIter __middle,
                         _RandomAccessIter __last, _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, 
      typename iterator_traits<_RandomAccessIter>::value_type,
      typename iterator_traits<_RandomAccessIter>::value_type);
  __partial_sort(__first, __middle, __last, __VALUE_TYPE(__first), __comp);
}
//排序并拷贝
template <class _InputIter, class _RandomAccessIter, class _Distance,
          class _Tp>
_RandomAccessIter __partial_sort_copy(_InputIter __first,
                                      _InputIter __last,
                                      _RandomAccessIter __result_first,
                                      _RandomAccessIter __result_last, 
                                      _Distance*, _Tp*) {
  if (__result_first == __result_last) return __result_last;	// 如果要拷贝到的区间为零，返回last
  _RandomAccessIter __result_real_last = __result_first;		
  while(__first != __last && __result_real_last != __result_last) {		//先讲first到last的内容拷贝到result中
    *__result_real_last = *__first;
    ++__result_real_last;
    ++__first;
  }
  make_heap(__result_first, __result_real_last);		// 对拷贝后的result建堆
  while (__first != __last) {			// 如果 result的长度比原来的要小，对原来的区间来说，first！=last。
    if (*__first < *__result_first) 	// 如果 比前面的要小的话，
      __adjust_heap(__result_first, _Distance(0),	// 调整最大堆，把此时的first加进去。
                    _Distance(__result_real_last - __result_first),
                    _Tp(*__first));
    ++__first;
  }
  sort_heap(__result_first, __result_real_last);	// 排序之后返回结果。
  return __result_real_last;
}

template <class _InputIter, class _RandomAccessIter>
inline _RandomAccessIter
partial_sort_copy(_InputIter __first, _InputIter __last,
                  _RandomAccessIter __result_first,
                  _RandomAccessIter __result_last) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_CONVERTIBLE(typename iterator_traits<_InputIter>::value_type,
                    typename iterator_traits<_RandomAccessIter>::value_type);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _LessThanComparable);
  return __partial_sort_copy(__first, __last, __result_first, __result_last, 
                             __DISTANCE_TYPE(__result_first),
                             __VALUE_TYPE(__first));
}
// compare
template <class _InputIter, class _RandomAccessIter, class _Compare,
          class _Distance, class _Tp>
_RandomAccessIter __partial_sort_copy(_InputIter __first,
                                         _InputIter __last,
                                         _RandomAccessIter __result_first,
                                         _RandomAccessIter __result_last,
                                         _Compare __comp, _Distance*, _Tp*) {
  if (__result_first == __result_last) return __result_last;
  _RandomAccessIter __result_real_last = __result_first;
  while(__first != __last && __result_real_last != __result_last) {
    *__result_real_last = *__first;
    ++__result_real_last;
    ++__first;
  }
  make_heap(__result_first, __result_real_last, __comp);
  while (__first != __last) {
    if (__comp(*__first, *__result_first))
      __adjust_heap(__result_first, _Distance(0),
                    _Distance(__result_real_last - __result_first),
                    _Tp(*__first),
                    __comp);
    ++__first;
  }
  sort_heap(__result_first, __result_real_last, __comp);
  return __result_real_last;
}

template <class _InputIter, class _RandomAccessIter, class _Compare>
inline _RandomAccessIter
partial_sort_copy(_InputIter __first, _InputIter __last,
                  _RandomAccessIter __result_first,
                  _RandomAccessIter __result_last, _Compare __comp) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_CONVERTIBLE(typename iterator_traits<_InputIter>::value_type,
                    typename iterator_traits<_RandomAccessIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
     typename iterator_traits<_RandomAccessIter>::value_type,
     typename iterator_traits<_RandomAccessIter>::value_type);  
  return __partial_sort_copy(__first, __last, __result_first, __result_last,
                             __comp,
                             __DISTANCE_TYPE(__result_first),
                             __VALUE_TYPE(__first));
}

// nth_element() and its auxiliary functions.  
// 这个函数和前面 partial_sort不同之处在于，这个函数只是把前n个元素选出来，不保证他们是有序的，但一定是前n小的，。
//也就是把排序后应该在第n个位置的元素放到第n个，然后前面都是比他小的，后面都是比他大的。
template <class _RandomAccessIter, class _Tp>
void __nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                   _RandomAccessIter __last, _Tp*) {
  while (__last - __first > 3) { //当区间长度大于3时
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2),
                                         *(__last - 1))));
										 //	首先以first，middle，last-1三个元素中间那个为基准进行分段，partition。
										 // 得到分段后中间的位置为cut，
    if (__cut <= __nth)					// 如果cut<=nth的话，说明前面的全用而也不够，让first=cut，从后面再找一段凑足nth
      __first = __cut;
    else 
      __last = __cut;	// 否则从前面减一点去，以达到nth。
  //	上面实际上就是一个二分的过程。
  }
  __insertion_sort(__first, __last);	// 最后长度小于等于3的时候，对fitst到last排序。
}

template <class _RandomAccessIter>
inline void nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                        _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __nth_element(__first, __nth, __last, __VALUE_TYPE(__first));
}
// compare
template <class _RandomAccessIter, class _Tp, class _Compare>
void __nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                   _RandomAccessIter __last, _Tp*, _Compare __comp) {
  while (__last - __first > 3) {
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2), 	// median返回三个元素中间的那个。
                                         *(__last - 1),
                                         __comp)),
                            __comp);
    if (__cut <= __nth)
      __first = __cut;
    else 
      __last = __cut;
  }
  __insertion_sort(__first, __last, __comp);
}

template <class _RandomAccessIter, class _Compare>
inline void nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                        _RandomAccessIter __last, _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
     typename iterator_traits<_RandomAccessIter>::value_type,
     typename iterator_traits<_RandomAccessIter>::value_type);
  __nth_element(__first, __nth, __last, __VALUE_TYPE(__first), __comp);
}


// Binary search (lower_bound, upper_bound, equal_range, binary_search).
// 二分查找，经常用！！！！

// lower_bound 查找第一个>=val 的元素。
template <class _ForwardIter, class _Tp, class _Distance>
_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Distance*) 
{
  _Distance __len = 0;		// 因为是前向迭代器，所以这里要这样写，。
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle;

  while (__len > 0) {	
    __half = __len >> 1;		// 二分查找。
    __middle = __first;
    advance(__middle, __half);
    if (*__middle < __val) {		// 如果val大于中间的元素。first指向middle。继续二分。
      __first = __middle;
      ++__first;			// 因为要找大于等于的，所以first++
      __len = __len - __half - 1;	// 长度减去一半，再减一（middle）。
    }
    else
      __len = __half;		// 否则直接长度变为一半。
  }
  return __first;
}

template <class _ForwardIter, class _Tp>
inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
				const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __lower_bound(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}
// compare
template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                              const _Tp& __val, _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__comp(*__middle, __val)) {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
    else
      __len = __half;
  }
  return __first;
}

template <class _ForwardIter, class _Tp, class _Compare>
inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val, _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __lower_bound(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
}
// upper_bound返回 大于val  的第一个元素位置。
template <class _ForwardIter, class _Tp, class _Distance>
_ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__val < *__middle)
      __len = __half;
    else {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
  }
  return __first;
}

template <class _ForwardIter, class _Tp>
inline _ForwardIter upper_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __upper_bound(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}
// compare
template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
_ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__comp(__val, *__middle))
      __len = __half;
    else {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
  }
  return __first;
}

template <class _ForwardIter, class _Tp, class _Compare>
inline _ForwardIter upper_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val, _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __upper_bound(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
}
//查找等于val的连续的区间。返回区间前后的迭代器pair
template <class _ForwardIter, class _Tp, class _Distance>
pair<_ForwardIter, _ForwardIter>
__equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
              _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle, __left, __right;

  while (__len > 0) {		// 二分查找、
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (*__middle < __val) {		// 如果val大于middle元素，first指向middle+1，len-=half+1
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
    else if (__val < *__middle)	// 如果小于，len-=half
      __len = __half;
    else {		// 如果恰好等于。
      __left = lower_bound(__first, __middle, __val);		// 使用lower_bound查找左边大于等于val的第一个元素
      advance(__first, __len);		// first指向当前区间长度的最后一个。
      __right = upper_bound(++__middle, __first, __val);	// 向后使用upper_bound查找大于val的第一个元素。
      return pair<_ForwardIter, _ForwardIter>(__left, __right);	// 返回该映射。
    }
  }
  return pair<_ForwardIter, _ForwardIter>(__first, __first);	// 如果len<=0，说明该元素只有一个。 返回此时的first，first。
}

template <class _ForwardIter, class _Tp>
inline pair<_ForwardIter, _ForwardIter>
equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp, 
       typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __equal_range(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}
// compare
template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
pair<_ForwardIter, _ForwardIter>
__equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
              _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle, __left, __right;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__comp(*__middle, __val)) {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
    else if (__comp(__val, *__middle))
      __len = __half;
    else {
      __left = lower_bound(__first, __middle, __val, __comp);
      advance(__first, __len);
      __right = upper_bound(++__middle, __first, __val, __comp);
      return pair<_ForwardIter, _ForwardIter>(__left, __right);
    }
  }
  return pair<_ForwardIter, _ForwardIter>(__first, __first);
}           

template <class _ForwardIter, class _Tp, class _Compare>
inline pair<_ForwardIter, _ForwardIter>
equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
            _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp, 
       typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __equal_range(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
} 
// 二分查找。判断是否存在一个大于等于val的元素。
template <class _ForwardIter, class _Tp>
bool binary_search(_ForwardIter __first, _ForwardIter __last,
                   const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
        typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  _ForwardIter __i = lower_bound(__first, __last, __val);
  return __i != __last && !(__val < *__i);
}
// 二分判断是否存在满足comp 的元素。
template <class _ForwardIter, class _Tp, class _Compare>
bool binary_search(_ForwardIter __first, _ForwardIter __last,
                   const _Tp& __val,
                   _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
        typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  _ForwardIter __i = lower_bound(__first, __last, __val, __comp);
  return __i != __last && !__comp(__val, *__i);
}

// merge, with and without an explicitly supplied comparison function.
// merge 合并函数。，（有或者没有特定的比较函数）
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter merge(_InputIter1 __first1, _InputIter1 __last1,
                  _InputIter2 __first2, _InputIter2 __last2,
                  _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2) {		// 经典的归并排序的合并过程。
    if (*__first2 < *__first1) {
      *__result = *__first2;
      ++__first2;
    }
    else {
      *__result = *__first1;
      ++__first1;
    }
    ++__result;
  }	// 合并到result之后
  return copy(__first2, __last2, copy(__first1, __last1, __result));	//	把first1和first2都拷贝到result后面。
}

template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter merge(_InputIter1 __first1, _InputIter1 __last1,
                  _InputIter2 __first2, _InputIter2 __last2,
                  _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES_SAME_TYPE(
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter1>::value_type);
  while (__first1 != __last1 && __first2 != __last2) {
    if (__comp(*__first2, *__first1)) {
      *__result = *__first2;
      ++__first2;
    }
    else {
      *__result = *__first1;
      ++__first1;
    }
    ++__result;
  }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}

// inplace_merge and its auxiliary functions. 


// 无缓冲区合并。
template <class _BidirectionalIter, class _Distance>
void __merge_without_buffer(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance __len1, _Distance __len2) {
  if (__len1 == 0 || __len2 == 0)
    return;
  if (__len1 + __len2 == 2) {	// 两个长度都是1的话。
    if (*__middle < *__first)		// 如果后面的比前面的小，交换一下。
      iter_swap(__first, __middle);
    return;
  }
  _BidirectionalIter __first_cut = __first;
  _BidirectionalIter __second_cut = __middle;
  _Distance __len11 = 0;
  _Distance __len22 = 0;
  if (__len1 > __len2) {	// 如果第一段长的话
    __len11 = __len1 / 2;	//  first_cut指向前一段的中间，
    advance(__first_cut, __len11);
    __second_cut = lower_bound(__middle, __last, *__first_cut);	// second_cut指向后一段第一个大于等于first_cut的元素。
    distance(__middle, __second_cut, __len22);
  }
  else {	//否则，相反。
    __len22 = __len2 / 2;
    advance(__second_cut, __len22);
    __first_cut = upper_bound(__first, __middle, *__second_cut);
    distance(__first, __first_cut, __len11);
  }
  _BidirectionalIter __new_middle	// 然后交换[first_cut,middle),[middle,second_cut)，这样这两部分就是有序的了。
    = rotate(__first_cut, __middle, __second_cut);
  __merge_without_buffer(__first, __first_cut, __new_middle,	// 然后对第一部分的前半部分执行合并
                         __len11, __len22);
  __merge_without_buffer(__new_middle, __second_cut, __last, __len1 - __len11,	//在对第二部分的后半部分进行合并。
                         __len2 - __len22);
}
// compare
template <class _BidirectionalIter, class _Distance, class _Compare>
void __merge_without_buffer(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance __len1, _Distance __len2,
                            _Compare __comp) {
  if (__len1 == 0 || __len2 == 0)
    return;
  if (__len1 + __len2 == 2) {
    if (__comp(*__middle, *__first))
      iter_swap(__first, __middle);
    return;
  }
  _BidirectionalIter __first_cut = __first;
  _BidirectionalIter __second_cut = __middle;
  _Distance __len11 = 0;
  _Distance __len22 = 0;
  if (__len1 > __len2) {
    __len11 = __len1 / 2;
    advance(__first_cut, __len11);
    __second_cut = lower_bound(__middle, __last, *__first_cut, __comp);
    distance(__middle, __second_cut, __len22);
  }
  else {
    __len22 = __len2 / 2;
    advance(__second_cut, __len22);
    __first_cut = upper_bound(__first, __middle, *__second_cut, __comp);
    distance(__first, __first_cut, __len11);
  }
  _BidirectionalIter __new_middle
    = rotate(__first_cut, __middle, __second_cut);
  __merge_without_buffer(__first, __first_cut, __new_middle, __len11, __len22,
                         __comp);
  __merge_without_buffer(__new_middle, __second_cut, __last, __len1 - __len11,
                         __len2 - __len22, __comp);
}
// 有缓冲区的额rotate 
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _Distance>
_BidirectionalIter1 __rotate_adaptive(_BidirectionalIter1 __first,
                                      _BidirectionalIter1 __middle,
                                      _BidirectionalIter1 __last,
                                      _Distance __len1, _Distance __len2,
                                      _BidirectionalIter2 __buffer,
                                      _Distance __buffer_size) {
  _BidirectionalIter2 __buffer_end;
  if (__len1 > __len2 && __len2 <= __buffer_size) {	// 如果第一个区间长度大于第二个，且第二个长度小于缓冲区长度。
    __buffer_end = copy(__middle, __last, __buffer);	// 先把第二个区间拷到缓冲区，
    copy_backward(__first, __middle, __last);	// 然后把第一个部分逆向拷贝到第二个缓冲区，也就是从last开始，先把第一个区间的最后一个元素拷过去，然后。。。
    return copy(__buffer, __buffer_end, __first);	// 最后把缓冲区中的第二个区间考回到前面first开始的位置。完成交换。也即是移位。
  }	
  else if (__len1 <= __buffer_size) {	// 如果 第一个区间就比buffer小，
    __buffer_end = copy(__first, __middle, __buffer);	//直接把第一个区间拷到缓冲区，
    copy(__middle, __last, __first);		// 把第二个区间拷到前面来，
    return copy_backward(__buffer, __buffer_end, __last);	// 逆向拷贝缓冲区的元素，也就是第一个区间的元素到后面。
  }
  else
    return rotate(__first, __middle, __last);		//否则缓冲区不够用，直接还是调用rotate函数。
}
// 反向合并。
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BidirectionalIter3>
_BidirectionalIter3 __merge_backward(_BidirectionalIter1 __first1,
                                     _BidirectionalIter1 __last1,
                                     _BidirectionalIter2 __first2,
                                     _BidirectionalIter2 __last2,
                                     _BidirectionalIter3 __result) {
  if (__first1 == __last1)// 如果第一个区间长度为零。
    return copy_backward(__first2, __last2, __result);	// 把第二个区间直接反向拷贝到result中。
  if (__first2 == __last2)	// 如果第二个区间是空的。就把第一个反向拷到result中。
    return copy_backward(__first1, __last1, __result);
  --__last1;
  --__last2;	// 指向最后一个元素。
  while (true) {
    if (*__last2 < *__last1) {	// 如果last2<last1
      *--__result = *__last1;	// last1赋值给result 
      if (__first1 == __last1)	// 如果第一个空了的话，。直接把第二个反向copy过去。
        return copy_backward(__first2, ++__last2, __result);	// last2++是要它指向最后一个元素的后面。
      --__last1;	// last1指向前一个元素。
    }
    else {		// 否则，，与上面相反。
      *--__result = *__last2;
      if (__first2 == __last2)
        return copy_backward(__first1, ++__last1, __result);
      --__last2;
    }
  }
}
// compare
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BidirectionalIter3, class _Compare>
_BidirectionalIter3 __merge_backward(_BidirectionalIter1 __first1,
                                     _BidirectionalIter1 __last1,
                                     _BidirectionalIter2 __first2,
                                     _BidirectionalIter2 __last2,
                                     _BidirectionalIter3 __result,
                                     _Compare __comp) {
  if (__first1 == __last1)
    return copy_backward(__first2, __last2, __result);
  if (__first2 == __last2)
    return copy_backward(__first1, __last1, __result);
  --__last1;
  --__last2;
  while (true) {
    if (__comp(*__last2, *__last1)) {
      *--__result = *__last1;
      if (__first1 == __last1)
        return copy_backward(__first2, ++__last2, __result);
      --__last1;
    }
    else {
      *--__result = *__last2;
      if (__first2 == __last2)
        return copy_backward(__first1, ++__last1, __result);
      --__last2;
    }
  }
}

template <class _BidirectionalIter, class _Distance, class _Pointer>
void __merge_adaptive(_BidirectionalIter __first,
                      _BidirectionalIter __middle, 
                      _BidirectionalIter __last,
                      _Distance __len1, _Distance __len2,
                      _Pointer __buffer, _Distance __buffer_size) {
  if (__len1 <= __len2 && __len1 <= __buffer_size) {// 如果第一个区间小于第二个，且第一个小于缓冲区 （这里有两种情况）
    _Pointer __buffer_end = copy(__first, __middle, __buffer);	// 直接把第一个区间拷到缓冲区。
    merge(__buffer, __buffer_end, __middle, __last, __first);	// 然后把缓冲区和第二个区间合并到原来第一个区间开始的地方。
  }
  else if (__len2 <= __buffer_size) {	// 如果第二个区间长度小于缓冲区长度， 同上。（除去上面的if，这里有两种情况）。
    _Pointer __buffer_end = copy(__middle, __last, __buffer);
    __merge_backward(__first, __middle, __buffer, __buffer_end, __last);	// 这里要反向合并,防止本来第一个区间的额元素被改写、
  }
  else {		//其他情况，也就是缓冲区是最小的情况，，，无法使用。
    _BidirectionalIter __first_cut = __first;
    _BidirectionalIter __second_cut = __middle;
    _Distance __len11 = 0;
    _Distance __len22 = 0;
    if (__len1 > __len2) {	// 如果第一个区间长度大于第二个
      __len11 = __len1 / 2;
      advance(__first_cut, __len11);	// first_cut指向前一段的中间
      __second_cut = lower_bound(__middle, __last, *__first_cut);	// second_cut指向后一段第一个大于等于first_cut的元素。
      distance(__middle, __second_cut, __len22); // len22表示该元素距中点的距离。
    }
    else {	// 否则就去前面找第一个大于 后面中间的元素。
      __len22 = __len2 / 2;
      advance(__second_cut, __len22);
      __first_cut = upper_bound(__first, __middle, *__second_cut);
      distance(__first, __first_cut, __len11);
    }
    _BidirectionalIter __new_middle =
      __rotate_adaptive(__first_cut, __middle, __second_cut, __len1 - __len11,
                        __len22, __buffer, __buffer_size);	// 很明显找出来这两块需要交换顺序，也即是移位。
															// 相当于进行了合并操作，使得中间这段有序了。
    __merge_adaptive(__first, __first_cut, __new_middle, __len11,	// 然后合并前面剩下的部分
                     __len22, __buffer, __buffer_size);
    __merge_adaptive(__new_middle, __second_cut, __last, __len1 - __len11,	// 合并后面剩下的部分。
                     __len2 - __len22, __buffer, __buffer_size);
  }
}
// compare
template <class _BidirectionalIter, class _Distance, class _Pointer,
          class _Compare>
void __merge_adaptive(_BidirectionalIter __first, 
                      _BidirectionalIter __middle, 
                      _BidirectionalIter __last,
                      _Distance __len1, _Distance __len2,
                      _Pointer __buffer, _Distance __buffer_size,
                      _Compare __comp) {
  if (__len1 <= __len2 && __len1 <= __buffer_size) {
    _Pointer __buffer_end = copy(__first, __middle, __buffer);
    merge(__buffer, __buffer_end, __middle, __last, __first, __comp);
  }
  else if (__len2 <= __buffer_size) {
    _Pointer __buffer_end = copy(__middle, __last, __buffer);
    __merge_backward(__first, __middle, __buffer, __buffer_end, __last,
                     __comp);
  }
  else {
    _BidirectionalIter __first_cut = __first;
    _BidirectionalIter __second_cut = __middle;
    _Distance __len11 = 0;
    _Distance __len22 = 0;
    if (__len1 > __len2) {
      __len11 = __len1 / 2;
      advance(__first_cut, __len11);
      __second_cut = lower_bound(__middle, __last, *__first_cut, __comp);
      distance(__middle, __second_cut, __len22);   
    }
    else {
      __len22 = __len2 / 2;
      advance(__second_cut, __len22);
      __first_cut = upper_bound(__first, __middle, *__second_cut, __comp);
      distance(__first, __first_cut, __len11);
    }
    _BidirectionalIter __new_middle =
      __rotate_adaptive(__first_cut, __middle, __second_cut, __len1 - __len11,
                        __len22, __buffer, __buffer_size);
    __merge_adaptive(__first, __first_cut, __new_middle, __len11,
                     __len22, __buffer, __buffer_size, __comp);
    __merge_adaptive(__new_middle, __second_cut, __last, __len1 - __len11,
                     __len2 - __len22, __buffer, __buffer_size, __comp);
  }
}

template <class _BidirectionalIter, class _Tp, class _Distance>
inline void __inplace_merge_aux(_BidirectionalIter __first,
                                _BidirectionalIter __middle,
                                _BidirectionalIter __last, _Tp*, _Distance*) {
  _Distance __len1 = 0;
  distance(__first, __middle, __len1);
  _Distance __len2 = 0;
  distance(__middle, __last, __len2);

  _Temporary_buffer<_BidirectionalIter, _Tp> __buf(__first, __last);
  if (__buf.begin() == 0)
    __merge_without_buffer(__first, __middle, __last, __len1, __len2);
  else
    __merge_adaptive(__first, __middle, __last, __len1, __len2,
                     __buf.begin(), _Distance(__buf.size()));
}

template <class _BidirectionalIter, class _Tp, 
          class _Distance, class _Compare>
inline void __inplace_merge_aux(_BidirectionalIter __first,
                                _BidirectionalIter __middle,
                                _BidirectionalIter __last, _Tp*, _Distance*,
                                _Compare __comp) {
  _Distance __len1 = 0;
  distance(__first, __middle, __len1);
  _Distance __len2 = 0;
  distance(__middle, __last, __len2);

  _Temporary_buffer<_BidirectionalIter, _Tp> __buf(__first, __last);
  if (__buf.begin() == 0)
    __merge_without_buffer(__first, __middle, __last, __len1, __len2, __comp);
  else
    __merge_adaptive(__first, __middle, __last, __len1, __len2,
                     __buf.begin(), _Distance(__buf.size()),
                     __comp);
}

template <class _BidirectionalIter>
inline void inplace_merge(_BidirectionalIter __first,
                          _BidirectionalIter __middle,
                          _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __middle || __middle == __last)
    return;
  __inplace_merge_aux(__first, __middle, __last,
                      __VALUE_TYPE(__first), __DISTANCE_TYPE(__first));
}

template <class _BidirectionalIter, class _Compare>
inline void inplace_merge(_BidirectionalIter __first,
                          _BidirectionalIter __middle,
                          _BidirectionalIter __last, _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
           typename iterator_traits<_BidirectionalIter>::value_type,
           typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __middle || __middle == __last)
    return;
  __inplace_merge_aux(__first, __middle, __last,
                      __VALUE_TYPE(__first), __DISTANCE_TYPE(__first),
                      __comp);
}

// Set algorithms: includes, set_union, set_intersection, set_difference,
// set_symmetric_difference.  All of these algorithms have the precondition
// that their input ranges are sorted and the postcondition that their output
// ranges are sorted.
// set,,,首先要保证传进来的是有序的。

// include用于判断first1中是否存在子序列first2.
template <class _InputIter1, class _InputIter2>
bool includes(_InputIter1 __first1, _InputIter1 __last1,
              _InputIter2 __first2, _InputIter2 __last2) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2)	//两个串都不为空时
    if (*__first2 < *__first1)		// 因为是从小到大排序的，所以如果first2<first1的话，那first1后面更不会有等于first2的了，所以返回false
      return false;
    else if(*__first1 < *__first2) // 如果first1<first2，那first1++往后找，可能有等于first2的
      ++__first1;
    else
      ++__first1, ++__first2;	// 如果恰好相等，就分别向后移动。

  return __first2 == __last2;	// 最后只有first2移动到了last2，才能保证first2中的每一个元素都能在first1中找到。
}
//  < 变成了comp
template <class _InputIter1, class _InputIter2, class _Compare>
bool includes(_InputIter1 __first1, _InputIter1 __last1,
              _InputIter2 __first2, _InputIter2 __last2, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first2, *__first1))
      return false;
    else if(__comp(*__first1, *__first2)) 
      ++__first1;
    else
      ++__first1, ++__first2;

  return __first2 == __last2;
}
//union完成集合的合并。注意，set中是没有重复元素的。multiset才有重复的。
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter set_union(_InputIter1 __first1, _InputIter1 __last1,
                      _InputIter2 __first2, _InputIter2 __last2,
                      _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2) {	//类似前面的merge函数，因为要保证有序，所以。。。
    if (*__first1 < *__first2) {
      *__result = *__first1;
      ++__first1;
    }
    else if (*__first2 < *__first1) {
      *__result = *__first2;
      ++__first2;
    }
    else {
      *__result = *__first1;		// 这里碰到两个相等的情况，要两个同时后移，跳过一个。
      ++__first1;
      ++__first2;
    }
    ++__result;	
  }
  return copy(__first2, __last2, copy(__first1, __last1, __result));	//最后先把first1拷到result后面，再把first2拷到last1后面。也就是把剩下的可能没有遍历到的元素加到后面去！！！
}
// 小于号换成 compare
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter set_union(_InputIter1 __first1, _InputIter1 __last1,
                      _InputIter2 __first2, _InputIter2 __last2,
                      _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  while (__first1 != __last1 && __first2 != __last2) {
    if (__comp(*__first1, *__first2)) {
      *__result = *__first1;
      ++__first1;
    }
    else if (__comp(*__first2, *__first1)) {
      *__result = *__first2;
      ++__first2;
    }
    else {
      *__result = *__first1;
      ++__first1;
      ++__first2;
    }
    ++__result;
  }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}
// set_intersection 求两个集合的交集。
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter set_intersection(_InputIter1 __first1, _InputIter1 __last1,
                             _InputIter2 __first2, _InputIter2 __last2,
                             _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2) 
    if (*__first1 < *__first2) 
      ++__first1;
    else if (*__first2 < *__first1) 
      ++__first2;
    else {// 把二者想同的元素存到result里面。
      *__result = *__first1;		
      ++__first1;
      ++__first2;
      ++__result;
    }
  return __result;
}
//  小于号变 compare
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter set_intersection(_InputIter1 __first1, _InputIter1 __last1,
                             _InputIter2 __first2, _InputIter2 __last2,
                             _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);

  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first1, *__first2))
      ++__first1;
    else if (__comp(*__first2, *__first1))
      ++__first2;
    else {
      *__result = *__first1;
      ++__first1;
      ++__first2;
      ++__result;
    }
  return __result;
}
// set_difference 求差集，所有属于第一个集合但是不属于第二个集合的元素。
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter set_difference(_InputIter1 __first1, _InputIter1 __last1,
                           _InputIter2 __first2, _InputIter2 __last2,
                           _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2)
    if (*__first1 < *__first2) {	
      *__result = *__first1;	// 属于第一个集合的元素，存到result中。
      ++__first1;
      ++__result;
    }
    else if (*__first2 < *__first1)	// 属于二不属于一，或者相等的，忽略。
      ++__first2;
    else {
      ++__first1;
      ++__first2;
    }
  return copy(__first1, __last1, __result);  //把剩下的第一个集合中的元素拷到result中。
}
// compare
template <class _InputIter1, class _InputIter2, class _OutputIter, 
          class _Compare>
_OutputIter set_difference(_InputIter1 __first1, _InputIter1 __last1,
                           _InputIter2 __first2, _InputIter2 __last2, 
                           _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);

  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first1, *__first2)) {
      *__result = *__first1;
      ++__first1;
      ++__result;
    }
    else if (__comp(*__first2, *__first1))
      ++__first2;
    else {
      ++__first1;
      ++__first2;
    }
  return copy(__first1, __last1, __result);
}
// set_symmetric_difference 求两个集合的额，。(A并B)-(A交B)
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter 
set_symmetric_difference(_InputIter1 __first1, _InputIter1 __last1,
                         _InputIter2 __first2, _InputIter2 __last2,
                         _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2)
    if (*__first1 < *__first2) {
      *__result = *__first1;
      ++__first1;
      ++__result;
    }
    else if (*__first2 < *__first1) {
      *__result = *__first2;
      ++__first2;
      ++__result;
    }
    else {		// 除了相等的部分，全都存到result中
      ++__first1;
      ++__first2;
    }
  return copy(__first2, __last2, copy(__first1, __last1, __result));	//对于没遍历到的，直接拷贝过去。
}
// compare
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter 
set_symmetric_difference(_InputIter1 __first1, _InputIter1 __last1,
                         _InputIter2 __first2, _InputIter2 __last2,
                         _OutputIter __result,
                         _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first1, *__first2)) {
      *__result = *__first1;
      ++__first1;
      ++__result;
    }
    else if (__comp(*__first2, *__first1)) {
      *__result = *__first2;
      ++__first2;
      ++__result;
    }
    else {
      ++__first1;
      ++__first2;
    }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}

// min_element and max_element, with and without an explicitly supplied
// comparison function.
// 最小，最大，（可以自定义比较函数）

// 求最大。元素是随机排列的。
template <class _ForwardIter>
_ForwardIter max_element(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _LessThanComparable);
  if (__first == __last) return __first; //如果就一个元素，直接返回。
  _ForwardIter __result = __first;
  while (++__first != __last) 	// 遍历，如果后面的比result大，更新之。
    if (*__result < *__first)
      __result = __first;
  return __result;
}
// compare
template <class _ForwardIter, class _Compare>
_ForwardIter max_element(_ForwardIter __first, _ForwardIter __last,
			 _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_ForwardIter>::value_type,
    typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last) return __first;
  _ForwardIter __result = __first;
  while (++__first != __last) 
    if (__comp(*__result, *__first)) __result = __first;
  return __result;
}
// 求最小，同样的道理。
template <class _ForwardIter>
_ForwardIter min_element(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _LessThanComparable);
  if (__first == __last) return __first;
  _ForwardIter __result = __first;
  while (++__first != __last) 
    if (*__first < *__result)
      __result = __first;
  return __result;
}

template <class _ForwardIter, class _Compare>
_ForwardIter min_element(_ForwardIter __first, _ForwardIter __last,
			 _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_ForwardIter>::value_type,
    typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last) return __first;
  _ForwardIter __result = __first;
  while (++__first != __last) 
    if (__comp(*__first, *__result))
      __result = __first;
  return __result;
}

// next_permutation and prev_permutation, with and without an explicitly 
// supplied comparison function.

//next_permutation生成元素的所有排列。n个元素有n!种情况 
//这是一个求一个排序的下一个排列的函数，可以遍历全排列
// 如果已经是最大的排列情况，会循环到最开始 排列最小的情况。
template <class _BidirectionalIter>
bool next_permutation(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)	// 空容器，返回false
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)	// 如果就一个元素，也是false
    return false;
  __i = __last;
  --__i;	//i指向最后一个元素。
	// 1 2 3(i) 5(ii) 4 3 2  ==> 1 2 3 5 4 3 2 ==>1 2 3 2 3 4 5 
  for(;;) {
    _BidirectionalIter __ii = __i;	//ii指向当前元素。
    --__i;	// i往前指一个。
    if (*__i < *__ii) {	//  如果 前面的比后面小
      _BidirectionalIter __j = __last;	
      while (!(*__i < *--__j))	// 从后往前找到第一个比i大的或相等的元素j。
        {}
      iter_swap(__i, __j);	// 交换i和j
      reverse(__ii, __last);	// 从ii到last 逆序。
      return true;		// 存在新的排列，返回true
    }
    if (__i == __first) {		// 不存在新的排列，当前是最大的排列了。
      reverse(__first, __last);	// 循环。。。。。   得到最小的排列
      return false;		// 返回false
    }
  }
}

template <class _BidirectionalIter, class _Compare>
bool next_permutation(_BidirectionalIter __first, _BidirectionalIter __last,
                      _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_BidirectionalIter>::value_type,
    typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (__comp(*__i, *__ii)) {
      _BidirectionalIter __j = __last;
      while (!__comp(*__i, *--__j))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}
// prev_permutation跟next_permutation完全相反。是反向排列的。
template <class _BidirectionalIter>
bool prev_permutation(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (*__ii < *__i) {
      _BidirectionalIter __j = __last;
      while (!(*--__j < *__i))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}

template <class _BidirectionalIter, class _Compare>
bool prev_permutation(_BidirectionalIter __first, _BidirectionalIter __last,
                      _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_BidirectionalIter>::value_type,
    typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (__comp(*__ii, *__i)) {
      _BidirectionalIter __j = __last;
      while (!__comp(*--__j, *__i))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}

// find_first_of, with and without an explicitly supplied comparison function.
// find_first_of，找到first1和first2第一个相等的元素的位置。
template <class _InputIter, class _ForwardIter>
_InputIter find_first_of(_InputIter __first1, _InputIter __last1,
                         _ForwardIter __first2, _ForwardIter __last2)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool, 
     typename iterator_traits<_InputIter>::value_type,
     typename iterator_traits<_ForwardIter>::value_type);

  for ( ; __first1 != __last1; ++__first1) 	
    for (_ForwardIter __iter = __first2; __iter != __last2; ++__iter)
      if (*__first1 == *__iter)
        return __first1;
  return __last1;
}
//compare
template <class _InputIter, class _ForwardIter, class _BinaryPredicate>
_InputIter find_first_of(_InputIter __first1, _InputIter __last1,
                         _ForwardIter __first2, _ForwardIter __last2,
                         _BinaryPredicate __comp)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
     typename iterator_traits<_InputIter>::value_type,
     typename iterator_traits<_ForwardIter>::value_type);

  for ( ; __first1 != __last1; ++__first1) 
    for (_ForwardIter __iter = __first2; __iter != __last2; ++__iter)
      if (__comp(*__first1, *__iter))
        return __first1;
  return __last1;
}


// find_end, with and without an explicitly supplied comparison function.
// Search [first2, last2) as a subsequence in [first1, last1), and return
// the *last* possible match.  Note that find_end for bidirectional iterators
// is much faster than for forward iterators.

// find_end for forward iterators. 
// find_end  前向迭代器版本。
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter1 __find_end(_ForwardIter1 __first1, _ForwardIter1 __last1,
                         _ForwardIter2 __first2, _ForwardIter2 __last2,
                         forward_iterator_tag, forward_iterator_tag)
{
  if (__first2 == __last2)	// 如果第二个区间是空的 ，返回last1
    return __last1;
  else {	// 否则。
    _ForwardIter1 __result = __last1;
    while (1) {		// 因为前向迭代器只能从头到尾，所以要从first开始。
      _ForwardIter1 __new_result		// 首先查找first1是否存在子区间是first2。
        = search(__first1, __last1, __first2, __last2);
      if (__new_result == __last1)	// 如果没有相等的话，返回result
        return __result;
      else {
        __result = __new_result;		// 有，更新result。
        __first1 = __new_result;	// 更新first1，从而找下一个相等的区间
        ++__first1;
      }
    }
  }
}

template <class _ForwardIter1, class _ForwardIter2,
          class _BinaryPredicate>
_ForwardIter1 __find_end(_ForwardIter1 __first1, _ForwardIter1 __last1,
                         _ForwardIter2 __first2, _ForwardIter2 __last2,
                         forward_iterator_tag, forward_iterator_tag,
                         _BinaryPredicate __comp)
{
  if (__first2 == __last2)
    return __last1;
  else {
    _ForwardIter1 __result = __last1;
    while (1) {
      _ForwardIter1 __new_result
        = search(__first1, __last1, __first2, __last2, __comp);
      if (__new_result == __last1)
        return __result;
      else {
        __result = __new_result;
        __first1 = __new_result;
        ++__first1;
      }
    }
  }
}

// find_end for bidirectional iterators.  Requires partial specialization.
#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
// find_end ，双向迭代器。
template <class _BidirectionalIter1, class _BidirectionalIter2>
_BidirectionalIter1
__find_end(_BidirectionalIter1 __first1, _BidirectionalIter1 __last1,
           _BidirectionalIter2 __first2, _BidirectionalIter2 __last2,
           bidirectional_iterator_tag, bidirectional_iterator_tag)
{
  __STL_REQUIRES(_BidirectionalIter1, _BidirectionalIterator);
  __STL_REQUIRES(_BidirectionalIter2, _BidirectionalIterator);
  typedef reverse_iterator<_BidirectionalIter1> _RevIter1;
  typedef reverse_iterator<_BidirectionalIter2> _RevIter2;

  _RevIter1 __rlast1(__first1);//  这里直接把迭代器的方向变为与原来相反。
  _RevIter2 __rlast2(__first2);
  _RevIter1 __rresult = search(_RevIter1(__last1), __rlast1,		// 然后去查找反向之后的result的位置。
                               _RevIter2(__last2), __rlast2);

  if (__rresult == __rlast1)
    return __last1;
  else {
    _BidirectionalIter1 __result = __rresult.base();
    advance(__result, -distance(__first2, __last2));	// 用反向查找到的result减去区间二的长度，就是正向的result。
    return __result;
  }
}

template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BinaryPredicate>
_BidirectionalIter1
__find_end(_BidirectionalIter1 __first1, _BidirectionalIter1 __last1,
           _BidirectionalIter2 __first2, _BidirectionalIter2 __last2,
           bidirectional_iterator_tag, bidirectional_iterator_tag, 
           _BinaryPredicate __comp)
{
  __STL_REQUIRES(_BidirectionalIter1, _BidirectionalIterator);
  __STL_REQUIRES(_BidirectionalIter2, _BidirectionalIterator);
  typedef reverse_iterator<_BidirectionalIter1> _RevIter1;
  typedef reverse_iterator<_BidirectionalIter2> _RevIter2;

  _RevIter1 __rlast1(__first1);
  _RevIter2 __rlast2(__first2);
  _RevIter1 __rresult = search(_RevIter1(__last1), __rlast1,
                               _RevIter2(__last2), __rlast2,
                               __comp);

  if (__rresult == __rlast1)
    return __last1;
  else {
    _BidirectionalIter1 __result = __rresult.base();
    advance(__result, -distance(__first2, __last2));
    return __result;
  }
}
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// Dispatching functions for find_end.

template <class _ForwardIter1, class _ForwardIter2>
inline _ForwardIter1 
find_end(_ForwardIter1 __first1, _ForwardIter1 __last1, 
         _ForwardIter2 __first2, _ForwardIter2 __last2)
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);
  return __find_end(__first1, __last1, __first2, __last2,
                    __ITERATOR_CATEGORY(__first1),
                    __ITERATOR_CATEGORY(__first2));
}

template <class _ForwardIter1, class _ForwardIter2, 
          class _BinaryPredicate>
inline _ForwardIter1 
find_end(_ForwardIter1 __first1, _ForwardIter1 __last1, 
         _ForwardIter2 __first2, _ForwardIter2 __last2,
         _BinaryPredicate __comp)
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);

  return __find_end(__first1, __last1, __first2, __last2,
                    __ITERATOR_CATEGORY(__first1),
                    __ITERATOR_CATEGORY(__first2),
                    __comp);
}

// is_heap, a predicate testing whether or not a range is
// a heap.  This function is an extension, not part of the C++
// standard.

// is_heap 用于判断前n个是否是一个堆
template <class _RandomAccessIter, class _Distance>
bool __is_heap(_RandomAccessIter __first, _Distance __n)
{
  _Distance __parent = 0;
  for (_Distance __child = 1; __child < __n; ++__child) {
    if (__first[__parent] < __first[__child]) // 如果父节点小于子节点，表明不是最大堆，返回false。
      return false;
    if ((__child & 1) == 0)	// 如果child是偶数，说明是右儿子，父节点已经判断完毕，要判断下一个父节点了。
      ++__parent;
  }
  return true;
}
// compare

template <class _RandomAccessIter, class _Distance, class _StrictWeakOrdering>
bool __is_heap(_RandomAccessIter __first, _StrictWeakOrdering __comp,
               _Distance __n)
{
  _Distance __parent = 0;
  for (_Distance __child = 1; __child < __n; ++__child) {
    if (__comp(__first[__parent], __first[__child]))
      return false;
    if ((__child & 1) == 0)
      ++__parent;
  }
  return true;
}

template <class _RandomAccessIter>
inline bool is_heap(_RandomAccessIter __first, _RandomAccessIter __last)
{
  __STL_REQUIRES(_RandomAccessIter, _RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  return __is_heap(__first, __last - __first);
}


template <class _RandomAccessIter, class _StrictWeakOrdering>
inline bool is_heap(_RandomAccessIter __first, _RandomAccessIter __last,
                    _StrictWeakOrdering __comp)
{
  __STL_REQUIRES(_RandomAccessIter, _RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_StrictWeakOrdering, bool, 
         typename iterator_traits<_RandomAccessIter>::value_type, 
         typename iterator_traits<_RandomAccessIter>::value_type);
  return __is_heap(__first, __comp, __last - __first);
}

// is_sorted, a predicated testing whether a range is sorted in
// nondescending order.  This is an extension, not part of the C++
// standard.
//is_sorted用于判断元素是否是从小到大排序好的。
template <class _ForwardIter>
bool is_sorted(_ForwardIter __first, _ForwardIter __last)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)
    return true;

  _ForwardIter __next = __first;
  for (++__next; __next != __last; __first = __next, ++__next) {
    if (*__next < *__first)	// 如果后面的比前面的小，返回false。
      return false;
  }

  return true;
}

template <class _ForwardIter, class _StrictWeakOrdering>
bool is_sorted(_ForwardIter __first, _ForwardIter __last,
               _StrictWeakOrdering __comp)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_StrictWeakOrdering, bool, 
        typename iterator_traits<_ForwardIter>::value_type,
        typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last)
    return true;

  _ForwardIter __next = __first;
  for (++__next; __next != __last; __first = __next, ++__next) {
    if (__comp(*__next, *__first))
      return false;
  }

  return true;
}

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1209
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_ALGO_H */

// Local Variables:
// mode:C++
// End:
