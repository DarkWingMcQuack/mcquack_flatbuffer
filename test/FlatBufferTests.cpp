#include <gtest/gtest.h>

#include <mcquack_flatbuffer/dynamic_flatbuffer.hpp>
#include <numeric>

using mcquack::dynamic_flatbuffer;

TEST(DynamicFlatBufferTest, AccessWithinRange)
{
    dynamic_flatbuffer<int, 3> buf(2);
    std::iota(buf.elements().begin(), buf.elements().end(), 0);

    auto span = buf[0];
    EXPECT_EQ(span[0], 0);
    EXPECT_EQ(span[1], 1);
    EXPECT_EQ(span[2], 2);

    span = buf[1];
    EXPECT_EQ(span[0], 3);
    EXPECT_EQ(span[1], 4);
    EXPECT_EQ(span[2], 5);
}

TEST(DynamicFlatBufferTest, AppendOperation)
{
    dynamic_flatbuffer<int, 3> buf;
    std::array<int, 3> data1 = {1, 2, 3};
    std::array<int, 3> data2 = {4, 5, 6};

    buf.append(data1);
    buf.append(data2);

    EXPECT_EQ(buf.number_of_slots(), 2);

    EXPECT_EQ(buf[0][0], 1);
    EXPECT_EQ(buf[0][1], 2);
    EXPECT_EQ(buf[0][2], 3);
    EXPECT_EQ(buf[1][0], 4);
    EXPECT_EQ(buf[1][1], 5);
    EXPECT_EQ(buf[1][2], 6);
}


TEST(DynamicFlatBufferTest, ResizeAndDataIntegrity)
{
    dynamic_flatbuffer<int, 4> buf(2);
    auto first_slot = buf[0];
    for(int i = 0; i < 4; ++i) {
        first_slot[i] = i + 1; // 1, 2, 3, 4
    }

    auto second_slot = buf[1];
    for(int i = 0; i < 4; ++i) {
        second_slot[i] = i + 5; // 5, 6, 7, 8
    }

    buf.resize(4); // Double the number of slots
    EXPECT_EQ(buf.number_of_slots(), 4);

    // Test existing data remains intact
    EXPECT_EQ(buf[0][0], 1);
    EXPECT_EQ(buf[1][3], 8);

    // Check new slots are zero-initialized
    EXPECT_EQ(buf[2][0], 0);
    EXPECT_EQ(buf[3][3], 0);
}

TEST(DynamicFlatBufferTest, LargeScaleOperations)
{
    dynamic_flatbuffer<int, 5> buf(1000);

    for(int i = 0; i < 1000; ++i) {
        auto slot = buf[i];
        for(int j = 0; j < 5; ++j) {
            slot[j] = i * 10 + j;
        }
    }

    for(int i = 0; i < 1000; ++i) {
        auto slot = buf[i];
        for(int j = 0; j < 5; ++j) {
            int expected_value = i * 10 + j;
            EXPECT_EQ(slot[j], expected_value) << "Mismatch at slot " << i << ", position " << j;
        }
    }
}

TEST(DynamicFlatBufferTest, AppendData)
{
    dynamic_flatbuffer<int, 2> buffer;
    std::vector<int> new_data = {7, 8};
    buffer.append(std::move(new_data));
    EXPECT_EQ(buffer[0][0], 7);
    EXPECT_EQ(buffer[0][1], 8);
}

TEST(DynamicFlatBufferTest, BoundaryConditions)
{
    dynamic_flatbuffer<int, 3> buf;
    EXPECT_EQ(buf.number_of_slots(), 0);

    buf.resize(3); // Resize to hold exactly one slot of 3 integers.
    buf[0][0] = 1;
    buf[0][1] = 2;
    buf[0][2] = 3;

    EXPECT_EQ(buf[0][0], 1);
    EXPECT_EQ(buf[0][2], 3);
}

// Additional test for zero initialization and multiple types
TEST(DynamicFlatBufferTest, ZeroInitialization)
{
    dynamic_flatbuffer<float, 4> buffer(1); // One slot with four floats
    EXPECT_FLOAT_EQ(buffer[0][0], 0.0);
    EXPECT_FLOAT_EQ(buffer[0][1], 0.0);
    EXPECT_FLOAT_EQ(buffer[0][2], 0.0);
    EXPECT_FLOAT_EQ(buffer[0][3], 0.0);
}

TEST(DynamicFlatBufferTest, SlotIterator)
{
    dynamic_flatbuffer<float, 3> buf(3); // One slot with four floats

    buf[0][0] = 1;
    buf[0][1] = 2;
    buf[0][2] = 3;

    buf[1][0] = 4;
    buf[1][1] = 5;
    buf[1][2] = 6;

    buf[2][0] = 7;
    buf[2][1] = 8;
    buf[2][2] = 9;

    auto exp = 1.0;
    for(const auto& slot : buf.slots()) {
        for(auto el : slot) {
            std::cout << el << ", " << exp << std::endl;
            EXPECT_EQ(el, exp);
            exp++;
        }
    }
}


