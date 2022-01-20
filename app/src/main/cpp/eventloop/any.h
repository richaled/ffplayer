#pragma once

#include "typeinfo"

namespace test {

    class any;
    template<typename T> T any_cast(const any &operand) noexcept (false);

    class any {
        template<typename T> friend T any_cast(const any &operand);

    public:
        any() = default;

        any(const any &rhs)
                : place_holder_(rhs.place_holder_ ? rhs.place_holder_->clone() : nullptr) {
            //printf("any copy any...\n");
        }

        template <typename ValueType>
        any(ValueType &&val,
            typename std::enable_if<!std::is_same<any&, ValueType>::value>::type* = nullptr,
            typename std::enable_if<!std::is_const<ValueType>::value>::type* = nullptr)
                : place_holder_(new holder<typename std::decay<ValueType>::type>(std::forward<ValueType>(val))) {
            //printf("any perfect forward value...\n");
        }

        any(any &&rhs) noexcept : place_holder_(std::move(rhs.place_holder_)) {
            rhs.place_holder_.reset();
            //printf("any move constructor\n");
        }

        template <typename ValueType>
        any(const ValueType &val)
                : place_holder_(new holder<typename std::decay<ValueType>::type>(val)) {
            //printf("any copy value...\n");
        }

        any& operator=(const any &rhs) noexcept(false) {
            any(rhs).swap(*this);
            return *this;
        }

        any& operator=(any &&rhs) noexcept {
            swap(rhs);
            any().swap(rhs);
            return *this;
        }

        template <typename ValueType> any& operator=(ValueType &&val) {
            any(std::forward<ValueType>(val)).swap(*this);
            return *this;
        }

        any& swap(any &rhs) noexcept {
            place_holder_.swap(rhs.place_holder_);
            return *this;
        }

        ~any() = default;

        bool empty() const noexcept {
            return place_holder_ == nullptr;
        }

        const std::type_info& type() const noexcept {
            return place_holder_ ? place_holder_->type() : typeid(void);
        }

    private:
        class placeholder {
        public:
            virtual ~placeholder() {}

            virtual placeholder* clone() const = 0;
            virtual const std::type_info& type() const = 0;
        };

        template <typename ValueType>
        class holder : public placeholder {
        public:
            ~holder() {
            }

            holder(ValueType &&val) : val_(std::forward<ValueType>(val)) {
//      printf("holder move....\n");
            }

            holder(const ValueType& value) : val_(value){
                //printf("holder copy....\n");
            }

            virtual placeholder* clone() const {
                return new holder(val_);
            }

            const std::type_info& type() const noexcept {
                return typeid(ValueType);
            }

        public:
            ValueType val_;
        };

    private:
        std::unique_ptr<placeholder> place_holder_;

    };

    template<typename ValueType>
    ValueType any_cast(const any &operand) noexcept(false) {
        using DECAY_TYPE = typename std::decay<ValueType>::type;
        if (strcmp(typeid(DECAY_TYPE).name(), operand.type().name())) {
            throw std::bad_cast();
        }
        any::holder<DECAY_TYPE> *holder =
                static_cast<any::holder<DECAY_TYPE>*>(operand.place_holder_.get());
        return holder->val_;
    }


}
