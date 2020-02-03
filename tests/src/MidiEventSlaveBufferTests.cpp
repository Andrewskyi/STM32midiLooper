
#include <vector>
#include <gtest/gtest.h>
#include <MidiEventSlaveBuffer.h>

#ifndef TEST_F
#define TEST_F(x, y) struct x##y : public x { void something(); }; x##y::something()
#endif

class MidiEventSlaveBufferTest : public ::testing::Test
{
protected:

	MidiEventSlaveBufferTest() : uut(buf, 1000)
	{
	}

	void SetUp()
	{

	}

	uint8_t buf[1000];
	MidiEventSlaveBuffer uut;
};

TEST_F(MidiEventSlaveBufferTest, someAssumptions)
{
	EXPECT_EQ(4, sizeof(MidiEventStorage));
	EXPECT_EQ(3, sizeof(MidiEventWithoutStatusStorage));
}

TEST_F(MidiEventSlaveBufferTest, writeRead_basic)
{
    const std::vector<MidiEvent> events{
        {128, 12, 33, 0, 10},
        {128, 13, 34, 1, 13},
        {129, 14, 35, 1, 56},
        {129, 100, 95, 2, 126},

        {128, 12, 33, 3, 126 + 2*0x7F + 2},
        {128, 13, 34, 3, 126 + 0x7F + 1},
        {129, 14, 35, 13, 126 + 3*0x7F + 12},
        {129, 14, 35, 14, 126 + 4*0x7F + 1444},

        {128, 12, 33, 14, 126 + 0xFFF + 1},
        {128, 13, 34, 15, 126 + 3*0xFFF + 12},
        {129, 14, 35, 16, 126 + 2*0xFFF + 2},
        {129, 14, 35, 17, 126 + 4*0xFFF + 1444},

        {128, 12, 33, 24, 126 + 0x1FFFF + 1},
        {128, 13, 34, 25, 126 + 3*0x1FFFF + 12},
        {129, 14, 35, 26, 126 + 2*0x1FFFF + 2},
        {129, 14, 35, 27, 126 + 4*0x1FFFF + 1444},
    };

    uut.reset();

    for(const auto& event : events)
    {
        uut.event = event;
        uut.saveEvent();
        uut.nextEventToWrite();
    }

    uut.reset();

    for(const auto& event : events)
    {
        EXPECT_EQ(event.b1, uut.event.b1);
        EXPECT_EQ(event.b2, uut.event.b2);
        EXPECT_EQ(event.b3, uut.event.b3);
        EXPECT_EQ(event.loopIdx, uut.event.loopIdx);
        EXPECT_EQ(event.time, uut.event.time);

        uut.nextEventToRead();
    }
}


