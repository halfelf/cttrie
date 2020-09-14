#ifndef _GETINDEX_H
#define _GETINDEX_H

// provides  pack_tools::get_index<I>(Ts&&... ts)

// (similar to what  std::get<I>(std::make_tuple(ts...))  does)

namespace pack_tools {

namespace detail {
  template <unsigned int> struct int_c {};

  template <unsigned int I>
  constexpr auto get_index_impl([[maybe_unused]] int_c<I> _i) -> void* // invalid index
  {
    return {};
  }

  template <typename T0,typename... Ts>
  constexpr auto get_index_impl([[maybe_unused]] int_c<0> _i,T0&& t0,Ts&&... ts) -> T0&&
  {
    return (T0&&)t0;
  }

  template <unsigned int I,typename T0,typename... Ts>
  constexpr auto get_index_impl([[maybe_unused]] int_c<I> _,T0&& t0,Ts&&... ts)
    -> decltype(get_index_impl(int_c<I-1>(),(Ts&&)ts...))
  {
    return get_index_impl(int_c<I-1>(),(Ts&&)ts...);
  }
} // namespace detail

template <unsigned int I,typename... Ts>
constexpr auto get_index(Ts&&... ts)
  -> decltype(detail::get_index_impl(detail::int_c<I>(),(Ts&&)ts...))
{
  static_assert((I<sizeof...(Ts)),"Invalid Index");
  return detail::get_index_impl(detail::int_c<I>(),(Ts&&)ts...);
}

} // namespace pack_tools

#endif
