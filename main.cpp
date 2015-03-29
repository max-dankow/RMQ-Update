#include <iostream>
#include <vector>
#include <limits>
#include <assert.h>
#include <algorithm>
#include <random>
#include <stdlib.h>
#include "rmq.h"

typedef int DataType;
static const size_t TEST_NUMBER = 100;

struct min_functor
{
    DataType operator()(DataType a, DataType b)
    {
        return std::min(a, b);
    }
};

class Slow_RMQ
{
public:
    Slow_RMQ(const std::vector<DataType> &init_data)
    {
        data = init_data;
    }

    void update(size_t bound_left, size_t bound_right, DataType delta)
    {
        size_t left = std::max(bound_left, (size_t) 0);
        size_t right = std::min(data.size(), bound_right + 1);

        for (size_t i = left; i < right; ++i)
        {
            data[i] += delta;
        }
    }

    DataType get_min(size_t bound_left, size_t bound_right)
    {
        size_t left = std::max(bound_left, (size_t) 0);
        size_t right = std::min(data.size(), bound_right + 1);
        DataType range_min = std::numeric_limits<DataType>::max();

        for (size_t i = left; i < right; ++i)
        {
            range_min = std::min(data[i], range_min);
        }

        return range_min;
    }

    void print_data()const
    {
        for (size_t i = 0; i < data.size(); ++i)
        {
            std::cout << data[i] << ' ';
        }

        std::cout << '\n';
    }

private:
    std::vector<DataType> data;
};

class RMQ: public Segment_Tree<DataType, min_functor>
{
public:
    RMQ(std::vector<DataType> input_data,
        const min_functor &new_function,
        DataType new_neutral_value):Segment_Tree<DataType, min_functor>
                                    (input_data,
                                     new_function,
                                     new_neutral_value)
    {
        delta.assign(data.size(), 0);
    }

    void update(int seek_left, int seek_right, int add_delta);

    DataType count(size_t seek_left, size_t seek_right)
    {
        if (seek_right >= real_number)
        {
            return neutral_value;
        }

        return count_RMQ(1, seek_left, seek_right, 0, list_number - 1);
    }

private:
    void push(size_t index);

    void update(int index, int seek_left, int seek_right,
           int bound_left, int bound_right, int add_delta);

    virtual DataType count_RMQ(size_t index, size_t seek_left, size_t seek_right,
                       size_t bound_left, size_t bound_right);

    std::vector<DataType> delta;
};

void RMQ::push(size_t index)
{
    if (delta[index] == 0 || get_son_left(index) >= data.size())
        return;

    delta[get_son_left(index)] += delta[index];
    delta[get_son_right(index)] += delta[index];
    delta[index] = 0;
}

void RMQ::update(int index, int seek_left, int seek_right,
                      int bound_left, int bound_right, int add_delta)
{
    if (seek_left > seek_right)
        return;

    if (bound_left == seek_left && seek_right == bound_right)
    {
        delta[index] += add_delta;
        /*if (bound_left == bound_right)
        {
            data[index] += delta[index];
            delta[index] = 0;
        }
        */
    }
    else
    {
        int middle = (bound_left + bound_right) / 2;

        update(get_son_left(index), seek_left, std::min(middle, seek_right),
               bound_left, middle, add_delta);

        update(get_son_right(index), std::max(seek_left, middle + 1), seek_right,
               middle + 1, bound_right, add_delta);

        data[index] = function(data[get_son_left(index)] + delta[get_son_left(index)],
                               data[get_son_right(index)] + delta[get_son_right(index)]);
        push(index);
    }
}

DataType RMQ::count_RMQ(size_t index, size_t seek_left, size_t seek_right,
                             size_t bound_left, size_t bound_right)
{
    if (seek_left > seek_right)
    {
        return neutral_value;
    }

    if (bound_left == seek_left && bound_right == seek_right)
    {
        return data[index] + delta[index];
    }

    push(index);

    size_t middle = (bound_left + bound_right) / 2;

    data[index] = function(data[get_son_left(index)] + delta[get_son_left(index)],
                           data[get_son_right(index)] + delta[get_son_right(index)]);

    return function(count_RMQ(get_son_left(index), seek_left, std::min(middle, seek_right),
                              bound_left, middle),

                    count_RMQ(get_son_right(index), std::max(seek_left, middle + 1), seek_right,
                              middle + 1, bound_right));
}

void RMQ::update(int seek_left, int seek_right, int add_delta)
{
    return update(1, seek_left, seek_right, 0, list_number - 1, add_delta);
}

std::default_random_engine generator;

std::vector<DataType> generate_vector(size_t size)
{
    std::vector<DataType> random_data;
    std::uniform_int_distribution<DataType> random_value(0, 10);

    for (size_t i = 0; i < size; ++i)
        random_data.push_back(random_value(generator));

    return random_data;
}


void test_solution()
{
    std::uniform_int_distribution<int> random_operation(0, 5);
    std::uniform_int_distribution<DataType> random_delta(0, 10);

    std::vector<DataType> test_data = generate_vector(3);

    Slow_RMQ slow(test_data);
    RMQ tree(test_data, min_functor(), std::numeric_limits<DataType>::max());

    for (size_t i = 0; i < TEST_NUMBER; ++i)
    {
        std::uniform_int_distribution<size_t> random_left(0, test_data.size());
        size_t left = random_left(generator);

        std::uniform_int_distribution<size_t> random_right(left, test_data.size());
        size_t right = random_right(generator);

        if (random_operation(generator) == 0)
        {
            DataType delta = random_delta(generator);

            std::cout << "CHANGE: from " << left << " to " << right << " at " << delta << '\n';
            slow.print_data();

            tree.update(left, right, delta);
            slow.update(left, right, delta);

            slow.print_data();
        }
        else
        {
            std::cout << "Get: from " << left << " to " << right << '\n';

            DataType result_slow = slow.get_min(left, right);
            DataType result_tree = tree.count(left, right);

            std::cout << result_slow << ' ' << result_tree << '\n';

            if (result_slow != result_tree)
            {
                std::cout << "FAILED.\n";
                exit(-1);
            }
        }
    }

    std::cout << "TESTS OK.\n";
}

int main()
{
    test_solution();
    return 0;
}

