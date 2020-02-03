
#include <vector>
#include <gtest/gtest.h>
#include <MidiEventMasterBuffer.h>

#ifndef TEST_F
#define TEST_F(x, y) struct x##y : public x { void something(); }; x##y::something()
#endif

class MidiEventMasterBufferTest : public ::testing::Test
{
protected:

	MidiEventMasterBufferTest() : uut(buf, 1000)
	{
	}

	void SetUp()
	{

	}

	uint8_t buf[1000];
	MidiEventMasterBuffer uut;
};

TEST_F(MidiEventMasterBufferTest, someAssumptions)
{
	EXPECT_EQ(4, sizeof(MidiEventStorage));
	EXPECT_EQ(3, sizeof(MidiEventWithoutStatusStorage));
}

TEST_F(MidiEventMasterBufferTest, writeRead_basic)
{
    const std::vector<MidiEvent> events{
        {128, 12, 33, 0, 10}, // v2

        {128, 13, 34, 0, 13},
        {129, 14, 35, 0, 56},
        {129, 100, 95, 0, 126}, // v0

        {128, 12, 33, 0, 126 + 0x7F + 1},
        {128, 13, 34, 0, 126 + 2*0x7F + 7},
        {129, 14, 35, 0, 126 + 3*0x7F + 19}, // v3

        {129, 14, 35, 0, 126 + 4*0x7F + 1444},
        {128, 12, 33, 0, 126 + 0x7FFF + 1}, // v1

        {128, 13, 34, 0, 126 + 2*0x7FFF + 2},
        {129, 14, 35, 0, 126 + 3*0x7FFF + 12},
        {129, 14, 35, 0, 126 + 4*0x7FFF + 1444}, // v2
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
        EXPECT_EQ(event.time, uut.event.time);

        uut.nextEventToRead();
    }
}


