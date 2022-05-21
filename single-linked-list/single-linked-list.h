#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include <algorithm>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value{};
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
       
        friend class SingleLinkedList;
        
        explicit BasicIterator(Node* node)
            : node_(node) {
        }

    public:
        
        using iterator_category = std::forward_iterator_tag;    
        using value_type = Type;     
        using difference_type = std::ptrdiff_t;   
        using pointer = ValueType*;  
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept
            : node_(other.node_) {
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto this_copy(*this);
            ++(*this);
            return this_copy;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &node_->value;
        }
    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() = default;
    SingleLinkedList(std::initializer_list<Type> values) {
        Assign(values.begin(), values.end());  // Может бросить исключение
    }
    SingleLinkedList(const SingleLinkedList& other) {
        Assign(other.begin(), other.end());  // Может бросить исключение
    }
    SingleLinkedList& operator=(const SingleLinkedList& rhs) {

        if (this != &rhs) {
            if (rhs.IsEmpty()) { 
                Clear();
            }
            else {
                auto rhs_copy(rhs);  // может бросить исключение
                swap(rhs_copy);
            }
        }
        return *this;
    }
    ~SingleLinkedList() {
        Clear();
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{ &head_ };
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {

        Node* new_element = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_element;
        size_++;
        return Iterator{ new_element };
    }

    void PopFront() noexcept {
        assert(!this->IsEmpty());

        Node* next_node = head_.next_node->next_node;
        delete head_.next_node;
        head_.next_node = next_node;
        size_--;
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_->next_node != nullptr);

        Node* last_element = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = last_element;
        size_--;
        return Iterator{ last_element };;
    }
 
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{ head_.next_node };
    }

    [[nodiscard]] Iterator end() noexcept {
        return {};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }
 
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{ head_.next_node };
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return {};
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return GetSize() == 0;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* next_node = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = next_node;
        }
        size_ = 0;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

private:
    template <typename InputIterator>
    void Assign(InputIterator from, InputIterator to) {

        SingleLinkedList<Type> tmp;
        auto it_begin = tmp.cbefore_begin();

        while (from != to) {
            auto it_next = tmp.InsertAfter(it_begin, *from);
            it_begin = it_next;
            ++from;
        }
        swap(tmp);
    }

    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (&lhs == &rhs) 
        || (lhs.GetSize() == rhs.GetSize()
            && std::equal(lhs.begin(), lhs.end(), rhs.begin()));  // может бросить исключение
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);  // может бросить исключение
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());  // может бросить исключение
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);  // Может бросить исключение
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (rhs < lhs);  // Может бросить исключение
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);  // Может бросить исключение
}

