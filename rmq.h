#include <vector>
#include <algorithm>
#include <assert.h>

template <typename Value, typename Functor>
class Segment_Tree
{
public:
    Segment_Tree(std::vector<Value> input_data, const Functor &new_function,
                 Value new_neutral_value);
    virtual Value count_RMQ(size_t seek_left, size_t seek_right);
protected:
    //size_t get_parent(const size_t index) const;
    size_t get_son_left(const size_t index) const;
    size_t get_son_right(const size_t index) const;

    void init_tree(std::vector<Value> input_data,
                   const Functor &new_function, Value new_neutral_value);

    void build(const std::vector<Value> &input_data);

    virtual Value count_RMQ(size_t index, size_t seek_left, size_t seek_right,
                       size_t bound_left, size_t bound_right);

    std::vector<Value> data;
    Functor function;
    Value neutral_value;
    size_t list_number;
    size_t real_number;
};

/*
template <typename Value, typename Functor>
size_t Segment_Tree<Value, Functor>::get_parent(const size_t index) const
{
    return index / 2;
}
*/

template <typename Value, typename Functor>
size_t Segment_Tree<Value, Functor>::get_son_left(const size_t index) const
{
    return index * 2;
}

template <typename Value, typename Functor>
size_t Segment_Tree<Value, Functor>::get_son_right(const size_t index) const
{
    return (index * 2) + 1;
}

template <typename Value, typename Functor>
void Segment_Tree<Value, Functor>::build(const std::vector<Value> &input_data)
{
    list_number = 1 << ((int) (log2(input_data.size() - 1)) + 1);
    data.assign(list_number * 2, neutral_value);

    for (int i = list_number; i < list_number + input_data.size(); ++i)
    {
        data[i] = input_data[i - list_number];
    }

    for (int i = list_number - 1; i > 0; --i)
    {
        data[i] = function(data[get_son_left(i)], data[get_son_right(i)]);
    }

}

template <typename Value, typename Functor>
void Segment_Tree<Value, Functor>::init_tree(std::vector<Value> input_data, const Functor &new_function,
                                             Value new_neutral_value)
{
    function = new_function;
    neutral_value = new_neutral_value;
    real_number = input_data.size();
    build(input_data);
}

template <typename Value, typename Functor>
Segment_Tree<Value, Functor>::Segment_Tree(std::vector<Value> input_data, const Functor &new_function,
                                   Value new_neutral_value)
{
    init_tree(input_data, new_function, new_neutral_value);
}

/*
template <typename Value, typename Functor>
Segment_Tree<Value, Functor>::Segment_Tree()
{
}
*/

template <typename Value, typename Functor>
Value Segment_Tree<Value, Functor>::count_RMQ(size_t index, size_t seek_left, size_t seek_right,
                                     size_t bound_left, size_t bound_right)
{
    if (seek_left > seek_right)
    {
        return neutral_value;
    }

    if (bound_left == seek_left && bound_right == seek_right)
    {
        return data[index];
    }

    size_t middle = (bound_left + bound_right) / 2;

    return function(count_RMQ(get_son_left(index), seek_left, std::min(middle, seek_right),
                              bound_left, middle),

                    count_RMQ(get_son_right(index), std::max(seek_left, middle + 1), seek_right,
                              middle + 1, bound_right));
}

template <typename Value, typename Functor>
Value Segment_Tree<Value, Functor>::count_RMQ(size_t seek_left, size_t seek_right)
{
    if (seek_right >= real_number)
    {
        return neutral_value;
    }

    return count_RMQ(1, seek_left, seek_right, 0, list_number - 1);
}
