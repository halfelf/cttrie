#ifndef _CTTRIE_H
#define _CTTRIE_H

#include <string_view>
#include <utility>

/* Usage:

  TRIE(str)
    ... error case
  CASE("abc")
    ... str matches abc ...
  CASE("ad")
    ... etc
  ENDTRIE;   // <- trailing semicolon!

  - compile with optimization enabled!
  - cases may return a value
    -> return types must match! (deduced from error case)

*/

#include "cstr.h"
#include "getindex.h"
#include <type_traits>

namespace CtTrie {

    using pack_tools::detail::int_c;

    template<int Char, typename Next>
    struct Transition {
    };

// multiple inheritance required for cttrie_sw256 ...
    template<typename... Transitions>
    struct TrieNode : Transitions ... {
    };

    namespace detail {
        struct nil {
        };

        // template <bool B> using Sfinae=char(*)[2*B-1];
        template<bool B> using Sfinae = typename std::enable_if<B>::type;

        // end of new chain
        template<unsigned int Index>
        constexpr auto
        transitionAdd([[maybe_unused]] nil _n, [[maybe_unused]] string_t<0> _s) -> Transition<-1, int_c<Index>>
        // strip trailing '\0'
        { return {}; }
//  constexpr Transition<-1,int_c<Index>> transitionAdd(nil,string_t<>) { return {}; }

        // create new chain
        template<unsigned int Index, unsigned char Ch0, unsigned char... Chars>
        constexpr auto transitionAdd([[maybe_unused]] nil _n, [[maybe_unused]] string_t<Ch0, Chars...> _s) ->
        Transition<Ch0, TrieNode<decltype(transitionAdd<Index>(nil(), string_t<Chars...>()))>> { return {}; }

        template<unsigned int Index, unsigned char... Chars, typename... Prefix, typename... Transitions,
                typename =Sfinae<(sizeof...(Chars) == 0 || sizeof...(Transitions) == 0)>>
        constexpr auto insertSorted([[maybe_unused]] nil _n,
                                    string_t<Chars...> s,
                                    [[maybe_unused]] TrieNode<Prefix...> _tn,
                                    [[maybe_unused]] Transitions... _t)
        -> TrieNode<Prefix..., decltype(transitionAdd<Index>(nil(), s)), Transitions...> { return {}; }

        template<unsigned int Index,
                unsigned char Ch0,
                unsigned char... Chars, typename... Prefix, int Ch, typename Next, typename... Transitions,
                typename =Sfinae<(Ch > Ch0)>>
        constexpr auto insertSorted([[maybe_unused]] nil _n,
                                    string_t<Ch0, Chars...> s,
                                    [[maybe_unused]] TrieNode<Prefix...> _tn,
                                    [[maybe_unused]] Transition<Ch, Next> _trs,
                                    [[maybe_unused]] Transitions... _t)
        -> TrieNode<Prefix..., decltype(transitionAdd<Index>(nil(),
                                                             s)), Transition<Ch, Next>, Transitions...> { return {}; }

        template<unsigned int Index, typename String, typename... Transitions>
        constexpr auto trieAdd([[maybe_unused]] TrieNode<Transitions...> _tn)
        -> decltype(insertSorted<Index>(nil(), String(), TrieNode<>(), Transitions()...)) // nil forces adl
        { return {}; }

        template<unsigned int Index, unsigned char Ch0, unsigned char... Chars,
                typename... Prefix, int Ch, typename Next, typename... Transitions,
                typename =Sfinae<(Ch == Ch0)>>
        constexpr auto
        insertSorted([[maybe_unused]] nil _n, string_t<Ch0, Chars...> s,
                     [[maybe_unused]] TrieNode<Prefix...> _tn,
                     [[maybe_unused]] Transition<Ch, Next> _t, [[maybe_unused]] Transitions... _ts)
        -> TrieNode<Prefix..., Transition<Ch, decltype(trieAdd<Index, string_t<Chars...>>(
                Next()))>, Transitions...> { return {}; }

        template<unsigned int Index, unsigned char Ch0, unsigned char... Chars, typename... Prefix,
                int Ch, typename Next, typename... Transitions,
                typename =Sfinae<(Ch < Ch0)>>
        constexpr auto
        insertSorted([[maybe_unused]] nil _n, string_t<Ch0, Chars...> s,
                     [[maybe_unused]] TrieNode<Prefix...> _tn, [[maybe_unused]] Transition<Ch, Next> _t,
                     [[maybe_unused]] Transitions... _ts)
        -> decltype(insertSorted<Index>(nil(), s, TrieNode<Prefix..., Transition<Ch, Next>>(),
                                        Transitions()...)) { return {}; }

        template<unsigned int I>
        constexpr auto makeTrie([[maybe_unused]] nil _n) -> TrieNode<> // nil forces adl
        { return {}; }

        template<unsigned int I, typename String0, typename... Strings>
        constexpr auto makeTrie([[maybe_unused]] nil _n, [[maybe_unused]] String0 _s0, [[maybe_unused]] Strings... _ss)
        -> decltype(trieAdd<I, String0>(makeTrie<I + 1>(nil(), Strings()...))) { return {}; }

#if 1

#include "cttrie_sw32.tcc"
#include "cttrie_sw256.tcc"

#else
        template <typename Stringview,typename... Transitions,typename FnE,typename... Fns>
        auto Switch(unsigned char ch,Stringview&& str,TrieNode<Transitions...>,FnE&& fne,Fns&&... fns)
          -> decltype(fne())
        {
          switch (ch) {
          { case (Transitions::Char):
              return checkTrie(Transitions::Next(),(Stringview&&)str,(FnE&&)fne,(Fns&&)fns...); }...
          }
          return fne();
        }
#endif

        // handle trivial cases already here
        template<typename Stringview, typename FnE, typename... Fns>
        constexpr auto
        checkTrie(TrieNode<> trie, Stringview&& str, FnE&& fne, Fns&& ... fns) // possible via Transition<-1>
        -> decltype(fne()) {
            return fne();
        }

        template<int Char, typename Next, typename Stringview, typename FnE, typename... Fns, typename =Sfinae<(Char >=
                                                                                                                0)>>
        constexpr auto checkTrie(TrieNode<Transition<Char, Next>> trie, Stringview&& str, FnE&& fne, Fns&& ... fns)
        -> decltype(fne()) {
            return (!str.empty() && (str[0] == Char)) ? checkTrie(Next(), str.substr(1), (FnE&&) fne, (Fns&&) fns...)
                                                      : fne();
        }

        template<typename... Transitions, typename Stringview, typename FnE, typename... Fns>
        constexpr auto checkTrie(TrieNode<Transitions...> trie, Stringview&& str, FnE&& fne, Fns&& ... fns)
        -> decltype(fne()) {
            return (!str.empty()) ? Switch(str[0], str.substr(1), trie, (FnE&&) fne, (Fns&&) fns...) : fne();
        }

        template<unsigned int Index, typename... Transitions, typename Stringview, typename FnE, typename... Fns>
        constexpr auto checkTrie(
                [[maybe_unused]] TrieNode<Transition<-1, int_c<Index>>, Transitions...> _tn,
                Stringview&& str,
                FnE&& fne,
                Fns&& ... fns)
        -> decltype(fne()) {
            return (str.empty()) ? pack_tools::get_index<Index>((Fns&&) fns...)()
                                 : checkTrie(TrieNode<Transitions...>(), (Stringview&&) str, (FnE&&) fne,
                                             (Fns&&) fns...);
        }

        template<std::size_t... Is, typename Stringview, typename ArgE, typename... Args>
        constexpr auto doTrie(
                [[maybe_unused]] std::index_sequence<Is...> _is,
                Stringview&& str, ArgE&& argE, Args&& ... args)
        -> decltype(argE()) {
            return checkTrie(makeTrie<0>(nil(), pack_tools::get_index<(2 * Is)>((Args&&) args...)...),
                             (Stringview&&) str, (ArgE&&) argE,
                             pack_tools::get_index<(2 * Is + 1)>((Args&&) args...)...);
        }

    } // namespace detail

    template<typename TrieNode, typename Stringview, typename FnE, typename... Fns>
    constexpr auto checkTrie(TrieNode trie, Stringview&& str, FnE&& fne, Fns&& ... fns)
    -> decltype(fne()) {
        return detail::checkTrie(trie, (Stringview&&) str, (FnE&&) fne, (Fns&&) fns...); // also used for adl dance
    }

    template<typename Stringview, typename ArgE, typename... Args>
    constexpr auto doTrie(Stringview&& str, ArgE&& argE, Args&& ... args)
    -> decltype(argE()) {
        return detail::doTrie(std::make_index_sequence<sizeof...(args) / 2>(),
                              (Stringview&&) str,
                              (ArgE&&) argE,
                              (Args&&) args...);
    }

// Strings must be string_t
    template<typename... Strings>
    constexpr auto makeTrie(Strings... strs)
    -> decltype(detail::makeTrie<0>(detail::nil(), strs...)) { return {}; }

} // namespace CtTrie

#define TRIE(str)  TRIE2(std::string_view,str)
#define TRIE2(sv, str)  CtTrie::doTrie<sv>((str),[&]{
#define CASE(str)      },CSTR(str),[&]{
#define ENDTRIE        })

#endif
