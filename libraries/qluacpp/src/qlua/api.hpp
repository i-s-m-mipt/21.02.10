#pragma once

#include <map>

#include <luacpp/luacpp>

#include "api/macros.h"
#include "structs/datetime.hpp"
#include "structs/tables.hpp"
#include "structs/datasource.hpp"

namespace qlua {
    struct api {
        typedef api type;
        api(const lua::State & l) :
            l_(l) {
        }

        api(const type & other) :
            l_(other.l_) {
        }

        api(type && other) :
            l_(std::move(other.l_)) {
        }

        void swap(type & other) {
            std::swap(l_, other.l_);
        }

        type & operator=(const type & other) {
            type tmp(other);
            swap(tmp);
            return *this;
        }

        template <typename Constant>
        Constant constant(const char * name) const {
            l_.get_global(name);
            if (!l_.is_nil(-1)) {
                auto rslt = l_.at<Constant>(-1).get();
                l_.pop(1);
                return rslt;
            }
            else {
                l_.pop(1);
                throw std::runtime_error("QluaCpp error: QLua constant " + std::string(name) + " is nil in globals list");
            }
        }

        const lua::State & lua_state() const {
            return l_;
        }

        // Service "Сервисные функции"
#include "api/service.hpp"
    // Table data access Функции для обращения к строкам произвольных таблиц QUIK
#include "api/table_data.hpp"
    // Workplace "Функции взаимодействия скрипта Lua и Рабочего места QUIK"
#include "api/workplace.hpp"
    // Charts "Функции для работы с графиками"
#include "api/charts.hpp"
    // Table manipulations "Функции для работы с таблицами Рабочего места QUIK"
#include "api/table.hpp"

    protected:
        lua::State l_;
    };
}
