#include <gtest/gtest.h>
#include "../include/ringbuffer.h"
#include <thread>
#include <vector>
#include <algorithm>

class RingBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        ringbuffer_init(5);
    }

    void TearDown() override {
        ringbuffer_cleanup();
    }
};

TEST_F(RingBufferTest, InitialStateIsEmpty) {
    EXPECT_TRUE(ringbuffer_is_empty());
    EXPECT_FALSE(ringbuffer_is_full());
    EXPECT_EQ(ringbuffer_size(), 0);
}

TEST_F(RingBufferTest, AddAndRemoveElements) {
    EXPECT_EQ(ringbuffer_add(10), 0);
    EXPECT_EQ(ringbuffer_add(20), 0);
    EXPECT_EQ(ringbuffer_size(), 2);

    int value;
    EXPECT_EQ(ringbuffer_remove(&value), 0);
    EXPECT_EQ(value, 10);
    EXPECT_EQ(ringbuffer_size(), 1);

    EXPECT_EQ(ringbuffer_remove(&value), 0);
    EXPECT_EQ(value, 20);
    EXPECT_TRUE(ringbuffer_is_empty());
}

TEST_F(RingBufferTest, BufferFullBehavior) {
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(ringbuffer_add(i), 0);
    }
    
    EXPECT_TRUE(ringbuffer_is_full());
    EXPECT_EQ(ringbuffer_add(100), -1);
}

TEST_F(RingBufferTest, RemoveFromEmptyBuffer) {
    int value;
    EXPECT_EQ(ringbuffer_remove(&value), -1);
}

TEST_F(RingBufferTest, CircularBehavior) {
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(ringbuffer_add(i), 0);
    }

    int value;
    ringbuffer_remove(&value);
    ringbuffer_remove(&value);

    EXPECT_EQ(ringbuffer_add(100), 0);
    EXPECT_EQ(ringbuffer_add(101), 0);

    std::vector<int> expected = {2, 3, 4, 100, 101};
    std::vector<int> actual;
    while (!ringbuffer_is_empty()) {
        ringbuffer_remove(&value);
        actual.push_back(value);
    }
    
    EXPECT_EQ(actual, expected);
}

void producer(int start, int count) {
    for (int i = start; i < start + count; i++) {
        while (ringbuffer_add(i) == -1) {
            std::this_thread::yield();
        }
    }
}

void consumer(std::vector<int>& values, int count) {
    int value;
    for (int i = 0; i < count; i++) {
        while (ringbuffer_remove(&value) == -1) {
            std::this_thread::yield();
        }
        values.push_back(value);
    }
}

TEST_F(RingBufferTest, ThreadSafety) {
    const int ITEMS_PER_PRODUCER = 100;
    std::vector<int> consumed_values;
    
    std::thread producer1(producer, 0, ITEMS_PER_PRODUCER);
    std::thread producer2(producer, ITEMS_PER_PRODUCER, ITEMS_PER_PRODUCER);
    std::thread consumer1(consumer, std::ref(consumed_values), ITEMS_PER_PRODUCER * 2);

    producer1.join();
    producer2.join();
    consumer1.join();

    EXPECT_EQ(consumed_values.size(), ITEMS_PER_PRODUCER * 2);
    
    std::sort(consumed_values.begin(), consumed_values.end());
    
    for (int i = 0; i < ITEMS_PER_PRODUCER * 2; i++) {
        EXPECT_EQ(consumed_values[i], i);
    }
}