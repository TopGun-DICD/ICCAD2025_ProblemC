    #pragma once
#include "DEFReader.hpp"

#ifndef SPLIT_HPP
#define SPLIT_HPP


namespace my_boost {

    template<typename CharT>
    class is_any_of {
        std::basic_string<CharT> m_chars;
    public:
        explicit is_any_of(const CharT* chars) : m_chars(chars) {}
        explicit is_any_of(const std::basic_string<CharT>& chars) : m_chars(chars) {}

        bool operator()(CharT ch) const {
            return m_chars.find(ch) != std::basic_string<CharT>::npos;
        }
    };

    template<typename SequenceSequenceT,
        typename RangeT,
        typename PredicateT>
    void split(SequenceSequenceT& Result,
        const RangeT& Input,
        PredicateT Pred,
        bool bCompress = false)
    {
        using char_type = typename std::remove_reference_t<
            typename std::remove_const_t<
            decltype(*std::begin(Input))>>;

        auto itBegin = std::begin(Input);
        auto itEnd = std::end(Input);
        auto itCurrent = itBegin;

        while (itCurrent != itEnd) {
            auto itNext = std::find_if(itCurrent, itEnd, Pred);

            if (!bCompress || itCurrent != itNext) {
                Result.emplace_back(itCurrent, itNext);
            }

            itCurrent = itNext;
            if (itCurrent != itEnd) {
                ++itCurrent;
            }
        }
    }



        // Версия с поддержкой локали (использует std::isspace из <locale>)
        template<typename CharT>
        void trim_left(std::basic_string<CharT>& str,
            const std::locale& loc = std::locale()) {
            auto it = std::find_if_not(str.begin(), str.end(),
                [&loc](CharT ch) { return std::isspace(ch, loc); });
            str.erase(str.begin(), it);
        }

        // Аналогично для правой стороны
        template<typename CharT>
        void trim_right(std::basic_string<CharT>& str,
            const std::locale& loc = std::locale()) {
            auto it = std::find_if_not(str.rbegin(), str.rend(),
                [&loc](CharT ch) { return std::isspace(ch, loc); });
            str.erase(it.base(), str.end());
        }

        // Основная функция trim
        template<typename CharT>
        void trim(std::basic_string<CharT>& str,
            const std::locale& loc = std::locale()) {
            trim_left(str, loc);
            trim_right(str, loc);
        }

        // Версия без локали (только для ASCII)
        template<typename CharT>
        void trim_ascii(std::basic_string<CharT>& str) {
            auto is_space = [](CharT ch) {
                return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' || ch == '\f';
            };

            auto it = std::find_if_not(str.begin(), str.end(), is_space);
            str.erase(str.begin(), it);

            it = std::find_if_not(str.rbegin(), str.rend(), is_space).base();
            str.erase(it, str.end());
        }

    } // namespace my_boost


#endif // SPLIT_HPP


