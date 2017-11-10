/*
 * Copyright (c) 2014-2017, German Aerospace Center (DLR)
 *
 * This file is part of the development version of OUTPOST.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2014-2017, Fabian Greif (DLR RY-AVS)
 */
// ----------------------------------------------------------------------------

#include <outpost/utils/coding/cobs.h>

#include <unittest/harness.h>

using ::testing::ElementsAreArray;
using outpost::utils::CobsEncodingGenerator;

static size_t
getEncodedArray(CobsEncodingGenerator& generator,
                uint8_t* data,
                size_t maximumLength)
{
    size_t length = 0;
    while (!generator.isFinished() && (length < maximumLength))
    {
        data[length] = generator.getNextByte();
        ++length;
    }

    return length;
}

TEST(CobsGeneratorTest, emptyStringIsEncodedAsEmptyString)
{
    CobsEncodingGenerator generator(outpost::Slice<uint8_t>::empty());

    EXPECT_EQ(0x01, generator.getNextByte());
    EXPECT_TRUE(generator.isFinished());
}

TEST(CobsGeneratorTest, encodingOfSingleBlockWithoutZero)
{
    uint8_t input[] = { 0x01 };

    uint8_t actual[512];
    uint8_t expected[] = { 0x02, 0x01 };

    CobsEncodingGenerator generator(outpost::asSlice(input));
    size_t encodedLength = getEncodedArray(generator, actual, sizeof(actual));

    ASSERT_EQ(sizeof(expected), encodedLength);
    EXPECT_ARRAY_EQ(uint8_t, expected, actual, sizeof(expected));
}

TEST(CobsGeneratorTest, singleZeroEncoding)
{
    uint8_t input[] = { 0 };

    uint8_t actual[128];
    uint8_t expected[] = { 0x01, 0x01};

    CobsEncodingGenerator generator(outpost::asSlice(input));
    size_t encodedLength = getEncodedArray(generator, actual, sizeof(actual));

    ASSERT_EQ(sizeof(expected), encodedLength);
    EXPECT_ARRAY_EQ(uint8_t, expected, actual, sizeof(expected));
}

TEST(CobsGeneratorTest, doubleZeroEncoding)
{
    uint8_t input[] = { 0, 0 };

    uint8_t actual[128];
    uint8_t expected[] = { 0x01, 0x01, 0x01 };

    CobsEncodingGenerator generator(outpost::asSlice(input));
    size_t encodedLength = getEncodedArray(generator, actual, sizeof(actual));

    ASSERT_EQ(sizeof(expected), encodedLength);
    EXPECT_ARRAY_EQ(uint8_t, expected, actual, sizeof(expected));
}

TEST(CobsGeneratorTest, doubleBlockEncoding)
{
    uint8_t input[] = { 10, 11, 12, 0, 13, 14 };

    uint8_t actual[128];
    uint8_t expected[] = { 0x04, 10, 11, 12, 0x03, 13, 14 };

    CobsEncodingGenerator generator(outpost::asSlice(input));
    size_t encodedLength = getEncodedArray(generator, actual, sizeof(actual));

    ASSERT_EQ(sizeof(expected), encodedLength);
    EXPECT_ARRAY_EQ(uint8_t, expected, actual, sizeof(expected));
}

TEST(CobsGeneratorTest, exampleFromPaper)
{
    uint8_t input[] = {
        0x45, 0x00, 0x00, 0x2C, 0x4C, 0x79, 0x00, 0x00,
        0x40, 0x06, 0x4F, 0x37
    };

    uint8_t actual[128];
    uint8_t expected[] = {
        0x02, 0x45, 0x01, 0x04, 0x2C, 0x4C, 0x79, 0x01,
        0x05, 0x40, 0x06, 0x4F, 0x37
    };

    CobsEncodingGenerator generator(outpost::asSlice(input));
    size_t encodedLength = getEncodedArray(generator, actual, sizeof(actual));

    ASSERT_EQ(sizeof(expected), encodedLength);
    EXPECT_ARRAY_EQ(uint8_t, expected, actual, sizeof(expected));
}


TEST(CobsGeneratorTest, doubleZeroPrefix)
{
    uint8_t input[] = { 0, 0, 1 };

    uint8_t actual[128];
    uint8_t expected[] = { 0x01, 0x01, 0x02, 0x01 };

    CobsEncodingGenerator generator(outpost::asSlice(input));
    size_t encodedLength = getEncodedArray(generator, actual, sizeof(actual));

    EXPECT_EQ(sizeof(expected), encodedLength);
    EXPECT_ARRAY_EQ(uint8_t, expected, actual, sizeof(expected));
}

TEST(CobsGeneratorTest, zeroPrefixEndingInZero)
{
    uint8_t input[] = { 0, 1, 0 };

    uint8_t actual[128];
    uint8_t expected[] = { 0x01, 0x02, 0x01, 0x01 };

    CobsEncodingGenerator generator(outpost::asSlice(input));
    size_t encodedLength = getEncodedArray(generator, actual, sizeof(actual));

    EXPECT_EQ(sizeof(expected), encodedLength);
    EXPECT_ARRAY_EQ(uint8_t, expected, actual, sizeof(expected));
}

// ----------------------------------------------------------------------------
/*
# Python Code to generate the expected data.
# Requires the installed cobs module, see https://pythonhosted.org/cobs/intro.html
import sys
import cobs.cobs

t = [chr((x % 255) + 1) for x in range(0, 512)]
s = cobs.cobs.encode(''.join(t))
e = [ord(x) for x in s]

for i, element in enumerate(e):
    sys.stdout.write('0x%02x, ' % element)
    if i % 16 == 15:
        sys.stdout.write('\n')
sys.stdout.write('\n')*/
TEST(CobsGeneratorTest, blockOfDataWithoutZero)
{
    uint8_t input[512];

    for (size_t i = 0; i < 512; ++i)
    {
        input[i] = (i % 255) + 1;
    }

    uint8_t actual[515];
    uint8_t expected[515] = {
        0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
        0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
        0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
        0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
        0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
        0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
        0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
        0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0x05, 0xfe,
        0xff, 0x01, 0x02,
    };

    CobsEncodingGenerator generator(outpost::asSlice(input));
    size_t encodedLength = getEncodedArray(generator, actual, sizeof(actual));

    ASSERT_EQ(sizeof(expected), encodedLength);
    EXPECT_THAT(actual, ElementsAreArray(expected));
}

// ----------------------------------------------------------------------------
/*
# Python Code to generate the input and expected data.
# Requires the installed cobs module, see https://pythonhosted.org/cobs/intro.html

import sys
import cobs.cobs
import random

random.seed(10)
t = [int(random.random() * 256) for x in range(0, 1024)]

for i, element in enumerate(t):
	sys.stdout.write('0x%02X, ' % element)
	if i % 16 == 15:
		sys.stdout.write('\n')
sys.stdout.write('\n')

s = cobs.cobs.encode(''.join([chr(x) for x in t]))
e = [ord(x) for x in s]

for i, element in enumerate(e):
	sys.stdout.write('0x%02X, ' % element)
	if i % 16 == 15:
		sys.stdout.write('\n')
sys.stdout.write('\n')
*/
TEST(CobsGeneratorTest, randomData)
{
    uint8_t input[1024] = {
		0x92, 0x6D, 0x93, 0x34, 0xD0, 0xD2, 0xA7, 0x29, 0x85, 0x53, 0x3F, 0xF3, 0xFF, 0x0B, 0xDC, 0x9A,
		0x61, 0x48, 0xAC, 0x74, 0xAF, 0xA9, 0x22, 0xC4, 0xFB, 0xF8, 0x9D, 0x0B, 0x01, 0x22, 0xF0, 0x4D,
		0x5D, 0xE5, 0x50, 0x8C, 0x6F, 0x10, 0x95, 0xD8, 0x28, 0x39, 0x69, 0x09, 0x7F, 0xD1, 0xA8, 0x88,
		0xDA, 0x26, 0x91, 0x5F, 0x99, 0x1C, 0xC6, 0x18, 0x2A, 0xCE, 0xF2, 0x6E, 0x6A, 0x3E, 0x46, 0x9E,
		0x2D, 0x1E, 0x74, 0x2A, 0xA6, 0xD2, 0xC7, 0x7A, 0x58, 0x6F, 0x01, 0xB6, 0x54, 0x51, 0x14, 0x72,
		0x95, 0x63, 0xDE, 0xAC, 0x3D, 0x86, 0xE9, 0x85, 0x9A, 0x10, 0x7D, 0x76, 0x66, 0x6B, 0x95, 0x89,
		0x7D, 0x2A, 0x70, 0xF7, 0x6A, 0x09, 0x00, 0x89, 0x0C, 0x17, 0x1B, 0x74, 0xFE, 0x7C, 0x75, 0x6F,
		0x80, 0x72, 0xB9, 0xE5, 0xB2, 0x6C, 0xA6, 0xE8, 0x27, 0x3C, 0x9E, 0xAF, 0xA4, 0x96, 0xD4, 0x7A,
		0xCE, 0xF6, 0x6F, 0xDB, 0xB7, 0xE3, 0x0B, 0xE9, 0xC4, 0xF6, 0xFB, 0x49, 0xAB, 0x24, 0x98, 0xEB,
		0x31, 0x59, 0x14, 0x31, 0x35, 0x18, 0xFF, 0xE3, 0x76, 0xBE, 0xB1, 0x85, 0x6C, 0xAA, 0x8E, 0x44,
		0x5D, 0xA0, 0xA0, 0x35, 0x9C, 0xDF, 0xDF, 0xA6, 0x11, 0x6C, 0x16, 0x0E, 0x41, 0x74, 0xC2, 0xA0,
		0x48, 0xB1, 0x81, 0x81, 0x43, 0x71, 0x57, 0xB9, 0x7B, 0x72, 0x7A, 0x7A, 0xB0, 0xB8, 0xCD, 0x67,
		0xCB, 0xF6, 0xFE, 0xB8, 0x10, 0xD8, 0xD6, 0x04, 0x53, 0x52, 0xBD, 0x9C, 0x48, 0x2F, 0x2B, 0x18,
		0xCA, 0xEF, 0x38, 0xBE, 0xF3, 0x31, 0x92, 0x70, 0xF9, 0xDE, 0x04, 0x33, 0xD9, 0x93, 0x85, 0xCF,
		0xC3, 0xF4, 0x53, 0x0C, 0xB1, 0x0C, 0x90, 0xDC, 0x65, 0x99, 0x2B, 0x28, 0x9B, 0xDD, 0xFF, 0x07,
		0xEB, 0x82, 0x59, 0x65, 0x93, 0x59, 0x25, 0xDD, 0xB4, 0x9C, 0xB8, 0xFC, 0x2C, 0xD2, 0xD2, 0x57,
		0x8F, 0x75, 0x31, 0x6E, 0x27, 0xEE, 0x2F, 0xA2, 0x91, 0xF9, 0xCB, 0x5C, 0x60, 0x0C, 0x0D, 0x4F,
		0xF3, 0x0C, 0x2D, 0xA2, 0x5C, 0x2F, 0xF2, 0x0A, 0x99, 0x6D, 0x58, 0x31, 0x29, 0x67, 0xC0, 0x77,
		0xC6, 0xE0, 0x2C, 0xE1, 0x94, 0x6B, 0x83, 0xE3, 0x64, 0x18, 0xF5, 0x1E, 0x1B, 0xB8, 0x4F, 0x4A,
		0xA6, 0xE4, 0x58, 0xE8, 0x9E, 0x9B, 0x88, 0x03, 0x28, 0xAD, 0xB1, 0x18, 0xA3, 0x79, 0x93, 0x66,
		0x37, 0x5A, 0xF8, 0x30, 0xDC, 0x87, 0x13, 0xCF, 0x04, 0xD6, 0x56, 0xEB, 0xBA, 0x5B, 0xA0, 0xFC,
		0x2B, 0x34, 0x4A, 0xD2, 0x3F, 0x3F, 0x9D, 0x95, 0x6B, 0x8D, 0x07, 0x01, 0x07, 0x0E, 0xC5, 0xFA,
		0x1F, 0x62, 0x03, 0x6F, 0xA9, 0x9F, 0xE6, 0x8D, 0x0D, 0x60, 0x75, 0x55, 0x08, 0xA0, 0x7D, 0xB5,
		0x5D, 0x12, 0x43, 0x2D, 0x75, 0x27, 0x14, 0x86, 0x24, 0x1A, 0x1E, 0xB4, 0xCE, 0xCF, 0x93, 0xA2,
		0xC0, 0x6D, 0x82, 0xB6, 0xA2, 0x9B, 0xA5, 0x89, 0x36, 0x4E, 0xB0, 0x6B, 0xDB, 0x5F, 0xAB, 0x1D,
		0x4C, 0xD9, 0x2C, 0xA0, 0x07, 0x9B, 0x73, 0xE6, 0xD3, 0xC5, 0x56, 0xAB, 0x27, 0x7D, 0x63, 0x78,
		0x46, 0x36, 0x85, 0xA7, 0x90, 0xF3, 0x8E, 0x26, 0xA4, 0x51, 0xF0, 0x21, 0x75, 0xAC, 0xF8, 0x85,
		0x61, 0xC6, 0xC9, 0xE9, 0xB8, 0x20, 0xFD, 0xEE, 0x6F, 0xA8, 0xD5, 0xEA, 0xB9, 0x17, 0x11, 0x5D,
		0x74, 0x4F, 0xBF, 0xC3, 0x66, 0xB2, 0x31, 0xF3, 0x3D, 0x27, 0x61, 0xDC, 0x99, 0xAF, 0xD1, 0x62,
		0xF5, 0xFC, 0x89, 0xBF, 0xDA, 0x97, 0x21, 0xA8, 0x74, 0xDA, 0x35, 0x03, 0x2D, 0x2B, 0x62, 0x35,
		0x55, 0x96, 0x27, 0x7C, 0x5B, 0x01, 0xAC, 0x80, 0xC6, 0xE0, 0x0C, 0xB6, 0x75, 0xDF, 0x1D, 0xD6,
		0x6D, 0x17, 0x76, 0xE0, 0x12, 0x60, 0x14, 0x01, 0xD3, 0x8C, 0x92, 0xE6, 0x15, 0xF1, 0xC0, 0xD9,
		0xD0, 0x64, 0x05, 0xFF, 0xA2, 0xB6, 0xB2, 0x4C, 0xA8, 0x73, 0x3A, 0x7F, 0x77, 0x4D, 0xBF, 0xE8,
		0x99, 0x05, 0x8B, 0xA1, 0xD0, 0x4D, 0x3C, 0x5F, 0x29, 0xC7, 0x06, 0x90, 0xD5, 0x08, 0x2A, 0xFD,
		0x4E, 0xC4, 0x07, 0x77, 0x0A, 0x3E, 0x04, 0xAD, 0x74, 0x5A, 0x6B, 0xAE, 0x42, 0x27, 0x0A, 0xA7,
		0xAA, 0x3D, 0x83, 0xF8, 0x79, 0xC4, 0x59, 0x8D, 0xC7, 0xB2, 0x98, 0x4B, 0x0E, 0x7B, 0xB9, 0x5C,
		0xB9, 0x78, 0x11, 0x94, 0x8C, 0x52, 0x32, 0xFC, 0x95, 0xB9, 0x0A, 0xBB, 0x29, 0x7B, 0x64, 0x63,
		0xCA, 0x33, 0xF1, 0xD9, 0x7D, 0xE0, 0x8C, 0xEB, 0x7D, 0xFA, 0xBE, 0xA8, 0xFE, 0xB2, 0x3E, 0xA5,
		0x2B, 0x67, 0xB4, 0x30, 0x6C, 0x3A, 0x69, 0x3B, 0xB3, 0xD5, 0x7B, 0xB6, 0xC1, 0xCE, 0xCD, 0xA8,
		0xBD, 0xBA, 0x85, 0xCB, 0x30, 0xB8, 0xA3, 0x8A, 0x9A, 0xEB, 0xCA, 0xB0, 0xE6, 0xDF, 0x53, 0xDC,
		0xE2, 0x95, 0xEC, 0x2C, 0xCD, 0x3E, 0x66, 0x82, 0x6B, 0x84, 0x17, 0x0F, 0xB9, 0x45, 0x47, 0xB3,
		0xC1, 0x88, 0xD1, 0x59, 0xE3, 0xA3, 0xD8, 0xD0, 0xB3, 0xEC, 0xC1, 0x73, 0x1E, 0x67, 0x9D, 0x05,
		0x98, 0x68, 0x79, 0x10, 0x14, 0xA6, 0x44, 0x27, 0xDD, 0x0E, 0xB3, 0xD2, 0xE5, 0xA8, 0x0D, 0x70,
		0xDF, 0x69, 0x89, 0xBE, 0x93, 0x5C, 0x97, 0x76, 0x55, 0x11, 0x42, 0xFE, 0x4A, 0x59, 0xFB, 0x1A,
		0x40, 0x60, 0x76, 0x67, 0x46, 0x26, 0x8E, 0x2B, 0x00, 0xC2, 0xC3, 0x5C, 0xE1, 0x28, 0xE6, 0xAE,
		0xAB, 0x0C, 0xC3, 0x2B, 0x89, 0x8C, 0x0F, 0x4B, 0xEB, 0xC2, 0x4E, 0xFA, 0x12, 0xA7, 0xCD, 0xD8,
		0xA5, 0xBD, 0x3F, 0xC8, 0xAE, 0x92, 0x44, 0xAC, 0x0C, 0x80, 0xA8, 0x56, 0x99, 0x0E, 0x55, 0x7D,
		0xD4, 0x89, 0xEE, 0x5F, 0xE4, 0x94, 0x78, 0x2E, 0xD3, 0xCB, 0x63, 0x52, 0x46, 0x57, 0x17, 0xDA,
		0x6F, 0x2D, 0xAE, 0x2C, 0xE1, 0x01, 0x8D, 0xA3, 0x57, 0x31, 0xE6, 0x97, 0xB1, 0x2A, 0x02, 0xE8,
		0x30, 0x0E, 0xC6, 0x91, 0x04, 0x8E, 0x82, 0xEC, 0x97, 0x26, 0xA0, 0xCC, 0xF7, 0x4F, 0x93, 0x0C,
		0xF4, 0x24, 0xA7, 0xDB, 0x7A, 0xFA, 0x68, 0x91, 0xDA, 0x04, 0xDC, 0xD8, 0x2A, 0xB2, 0x46, 0x31,
		0x82, 0xB6, 0x61, 0x57, 0x39, 0x93, 0x95, 0xDE, 0xD5, 0x02, 0x25, 0x21, 0x45, 0xBE, 0x7D, 0xBA,
		0xD7, 0x5E, 0x00, 0x36, 0x9D, 0x92, 0x34, 0xC1, 0x07, 0x27, 0x84, 0x9A, 0xEA, 0x62, 0xD0, 0x78,
		0x4D, 0xF3, 0x7E, 0xB5, 0x4A, 0xE2, 0x7A, 0x0B, 0x11, 0x5A, 0x6D, 0x1C, 0xD9, 0x8E, 0xF2, 0xA6,
		0xCC, 0x2B, 0x0B, 0x3A, 0x6B, 0x0C, 0xED, 0x4C, 0xCA, 0x0B, 0xD9, 0x55, 0x83, 0x9C, 0x9C, 0x4B,
		0x41, 0x63, 0x8E, 0x00, 0x3A, 0x11, 0xD5, 0xB3, 0xD4, 0xAB, 0x09, 0x53, 0xE1, 0x77, 0x52, 0x28,
		0xE9, 0x18, 0x5C, 0x50, 0xA3, 0xE9, 0xBD, 0xF7, 0x48, 0xBE, 0xB6, 0x0D, 0x92, 0xE0, 0x07, 0x3A,
		0x89, 0x9B, 0x8B, 0xE2, 0x61, 0xA8, 0x8B, 0xD2, 0x64, 0xEB, 0xD8, 0xB9, 0xEA, 0xA9, 0x4B, 0xF5,
		0x5A, 0x58, 0xA0, 0x80, 0xFF, 0x6A, 0xEE, 0x73, 0x39, 0x89, 0xAC, 0x4C, 0x96, 0x6F, 0x05, 0x10,
		0x61, 0xB3, 0x28, 0xF6, 0x5C, 0x1B, 0x2F, 0xEB, 0xD8, 0xA9, 0x0F, 0xA4, 0x05, 0x3C, 0x11, 0xAB,
		0xD1, 0x42, 0xFE, 0x8A, 0xD9, 0x32, 0xCB, 0xA1, 0x13, 0xC3, 0xC3, 0x60, 0x43, 0x13, 0x9A, 0x4C,
		0x3B, 0x72, 0x20, 0xA8, 0x52, 0x4B, 0x6E, 0x6F, 0x6F, 0x41, 0xC0, 0xE5, 0x90, 0xB8, 0xCF, 0x49,
		0xDE, 0x91, 0xA2, 0x8D, 0xE2, 0x7D, 0xB5, 0x42, 0x28, 0x8A, 0xD3, 0x24, 0xB9, 0x46, 0x1C, 0xCF,
		0x0E, 0x6B, 0xA7, 0xB3, 0xC4, 0x19, 0x21, 0x4E, 0xD7, 0xE5, 0x6C, 0x48, 0x9D, 0x6E, 0x05, 0x9B,
	};

    uint8_t actual[1027];
    uint8_t expected[1027] = {
		0x67, 0x92, 0x6D, 0x93, 0x34, 0xD0, 0xD2, 0xA7, 0x29, 0x85, 0x53, 0x3F, 0xF3, 0xFF, 0x0B, 0xDC,
		0x9A, 0x61, 0x48, 0xAC, 0x74, 0xAF, 0xA9, 0x22, 0xC4, 0xFB, 0xF8, 0x9D, 0x0B, 0x01, 0x22, 0xF0,
		0x4D, 0x5D, 0xE5, 0x50, 0x8C, 0x6F, 0x10, 0x95, 0xD8, 0x28, 0x39, 0x69, 0x09, 0x7F, 0xD1, 0xA8,
		0x88, 0xDA, 0x26, 0x91, 0x5F, 0x99, 0x1C, 0xC6, 0x18, 0x2A, 0xCE, 0xF2, 0x6E, 0x6A, 0x3E, 0x46,
		0x9E, 0x2D, 0x1E, 0x74, 0x2A, 0xA6, 0xD2, 0xC7, 0x7A, 0x58, 0x6F, 0x01, 0xB6, 0x54, 0x51, 0x14,
		0x72, 0x95, 0x63, 0xDE, 0xAC, 0x3D, 0x86, 0xE9, 0x85, 0x9A, 0x10, 0x7D, 0x76, 0x66, 0x6B, 0x95,
		0x89, 0x7D, 0x2A, 0x70, 0xF7, 0x6A, 0x09, 0xFF, 0x89, 0x0C, 0x17, 0x1B, 0x74, 0xFE, 0x7C, 0x75,
		0x6F, 0x80, 0x72, 0xB9, 0xE5, 0xB2, 0x6C, 0xA6, 0xE8, 0x27, 0x3C, 0x9E, 0xAF, 0xA4, 0x96, 0xD4,
		0x7A, 0xCE, 0xF6, 0x6F, 0xDB, 0xB7, 0xE3, 0x0B, 0xE9, 0xC4, 0xF6, 0xFB, 0x49, 0xAB, 0x24, 0x98,
		0xEB, 0x31, 0x59, 0x14, 0x31, 0x35, 0x18, 0xFF, 0xE3, 0x76, 0xBE, 0xB1, 0x85, 0x6C, 0xAA, 0x8E,
		0x44, 0x5D, 0xA0, 0xA0, 0x35, 0x9C, 0xDF, 0xDF, 0xA6, 0x11, 0x6C, 0x16, 0x0E, 0x41, 0x74, 0xC2,
		0xA0, 0x48, 0xB1, 0x81, 0x81, 0x43, 0x71, 0x57, 0xB9, 0x7B, 0x72, 0x7A, 0x7A, 0xB0, 0xB8, 0xCD,
		0x67, 0xCB, 0xF6, 0xFE, 0xB8, 0x10, 0xD8, 0xD6, 0x04, 0x53, 0x52, 0xBD, 0x9C, 0x48, 0x2F, 0x2B,
		0x18, 0xCA, 0xEF, 0x38, 0xBE, 0xF3, 0x31, 0x92, 0x70, 0xF9, 0xDE, 0x04, 0x33, 0xD9, 0x93, 0x85,
		0xCF, 0xC3, 0xF4, 0x53, 0x0C, 0xB1, 0x0C, 0x90, 0xDC, 0x65, 0x99, 0x2B, 0x28, 0x9B, 0xDD, 0xFF,
		0x07, 0xEB, 0x82, 0x59, 0x65, 0x93, 0x59, 0x25, 0xDD, 0xB4, 0x9C, 0xB8, 0xFC, 0x2C, 0xD2, 0xD2,
		0x57, 0x8F, 0x75, 0x31, 0x6E, 0x27, 0xEE, 0x2F, 0xA2, 0x91, 0xF9, 0xCB, 0x5C, 0x60, 0x0C, 0x0D,
		0x4F, 0xF3, 0x0C, 0x2D, 0xA2, 0x5C, 0x2F, 0xF2, 0x0A, 0x99, 0x6D, 0x58, 0x31, 0x29, 0x67, 0xC0,
		0x77, 0xC6, 0xE0, 0x2C, 0xE1, 0x94, 0x6B, 0x83, 0xE3, 0x64, 0x18, 0xF5, 0x1E, 0x1B, 0xB8, 0x4F,
		0x4A, 0xA6, 0xE4, 0x58, 0xE8, 0x9E, 0x9B, 0x88, 0x03, 0x28, 0xAD, 0xB1, 0x18, 0xA3, 0x79, 0x93,
		0x66, 0x37, 0x5A, 0xF8, 0x30, 0xDC, 0x87, 0x13, 0xCF, 0x04, 0xD6, 0x56, 0xEB, 0xBA, 0x5B, 0xA0,
		0xFC, 0x2B, 0x34, 0x4A, 0xD2, 0x3F, 0x3F, 0x9D, 0x95, 0x6B, 0x8D, 0x07, 0x01, 0x07, 0x0E, 0xC5,
		0xFA, 0x1F, 0x62, 0x03, 0x6F, 0xA9, 0xFF, 0x9F, 0xE6, 0x8D, 0x0D, 0x60, 0x75, 0x55, 0x08, 0xA0,
		0x7D, 0xB5, 0x5D, 0x12, 0x43, 0x2D, 0x75, 0x27, 0x14, 0x86, 0x24, 0x1A, 0x1E, 0xB4, 0xCE, 0xCF,
		0x93, 0xA2, 0xC0, 0x6D, 0x82, 0xB6, 0xA2, 0x9B, 0xA5, 0x89, 0x36, 0x4E, 0xB0, 0x6B, 0xDB, 0x5F,
		0xAB, 0x1D, 0x4C, 0xD9, 0x2C, 0xA0, 0x07, 0x9B, 0x73, 0xE6, 0xD3, 0xC5, 0x56, 0xAB, 0x27, 0x7D,
		0x63, 0x78, 0x46, 0x36, 0x85, 0xA7, 0x90, 0xF3, 0x8E, 0x26, 0xA4, 0x51, 0xF0, 0x21, 0x75, 0xAC,
		0xF8, 0x85, 0x61, 0xC6, 0xC9, 0xE9, 0xB8, 0x20, 0xFD, 0xEE, 0x6F, 0xA8, 0xD5, 0xEA, 0xB9, 0x17,
		0x11, 0x5D, 0x74, 0x4F, 0xBF, 0xC3, 0x66, 0xB2, 0x31, 0xF3, 0x3D, 0x27, 0x61, 0xDC, 0x99, 0xAF,
		0xD1, 0x62, 0xF5, 0xFC, 0x89, 0xBF, 0xDA, 0x97, 0x21, 0xA8, 0x74, 0xDA, 0x35, 0x03, 0x2D, 0x2B,
		0x62, 0x35, 0x55, 0x96, 0x27, 0x7C, 0x5B, 0x01, 0xAC, 0x80, 0xC6, 0xE0, 0x0C, 0xB6, 0x75, 0xDF,
		0x1D, 0xD6, 0x6D, 0x17, 0x76, 0xE0, 0x12, 0x60, 0x14, 0x01, 0xD3, 0x8C, 0x92, 0xE6, 0x15, 0xF1,
		0xC0, 0xD9, 0xD0, 0x64, 0x05, 0xFF, 0xA2, 0xB6, 0xB2, 0x4C, 0xA8, 0x73, 0x3A, 0x7F, 0x77, 0x4D,
		0xBF, 0xE8, 0x99, 0x05, 0x8B, 0xA1, 0xD0, 0x4D, 0x3C, 0x5F, 0x29, 0xC7, 0x06, 0x90, 0xD5, 0x08,
		0x2A, 0xFD, 0x4E, 0xC4, 0x07, 0x77, 0x0A, 0x3E, 0x04, 0xAD, 0x74, 0x5A, 0x6B, 0xAE, 0x42, 0x27,
		0x0A, 0xA7, 0xAA, 0x3D, 0x83, 0xF8, 0x79, 0xC4, 0x59, 0x8D, 0xC7, 0xB2, 0x98, 0x4B, 0x0E, 0x7B,
		0xB9, 0x5C, 0xB9, 0x78, 0x11, 0x94, 0x8C, 0x52, 0x32, 0xFC, 0x95, 0xB9, 0x0A, 0xBB, 0x29, 0x7B,
		0x64, 0x63, 0xCA, 0x33, 0xF1, 0xD9, 0x7D, 0xE0, 0x8C, 0xEB, 0x7D, 0xFA, 0xBE, 0xA8, 0xFE, 0xB2,
		0x3E, 0xA5, 0x2B, 0x67, 0xB4, 0x66, 0x30, 0x6C, 0x3A, 0x69, 0x3B, 0xB3, 0xD5, 0x7B, 0xB6, 0xC1,
		0xCE, 0xCD, 0xA8, 0xBD, 0xBA, 0x85, 0xCB, 0x30, 0xB8, 0xA3, 0x8A, 0x9A, 0xEB, 0xCA, 0xB0, 0xE6,
		0xDF, 0x53, 0xDC, 0xE2, 0x95, 0xEC, 0x2C, 0xCD, 0x3E, 0x66, 0x82, 0x6B, 0x84, 0x17, 0x0F, 0xB9,
		0x45, 0x47, 0xB3, 0xC1, 0x88, 0xD1, 0x59, 0xE3, 0xA3, 0xD8, 0xD0, 0xB3, 0xEC, 0xC1, 0x73, 0x1E,
		0x67, 0x9D, 0x05, 0x98, 0x68, 0x79, 0x10, 0x14, 0xA6, 0x44, 0x27, 0xDD, 0x0E, 0xB3, 0xD2, 0xE5,
		0xA8, 0x0D, 0x70, 0xDF, 0x69, 0x89, 0xBE, 0x93, 0x5C, 0x97, 0x76, 0x55, 0x11, 0x42, 0xFE, 0x4A,
		0x59, 0xFB, 0x1A, 0x40, 0x60, 0x76, 0x67, 0x46, 0x26, 0x8E, 0x2B, 0x7A, 0xC2, 0xC3, 0x5C, 0xE1,
		0x28, 0xE6, 0xAE, 0xAB, 0x0C, 0xC3, 0x2B, 0x89, 0x8C, 0x0F, 0x4B, 0xEB, 0xC2, 0x4E, 0xFA, 0x12,
		0xA7, 0xCD, 0xD8, 0xA5, 0xBD, 0x3F, 0xC8, 0xAE, 0x92, 0x44, 0xAC, 0x0C, 0x80, 0xA8, 0x56, 0x99,
		0x0E, 0x55, 0x7D, 0xD4, 0x89, 0xEE, 0x5F, 0xE4, 0x94, 0x78, 0x2E, 0xD3, 0xCB, 0x63, 0x52, 0x46,
		0x57, 0x17, 0xDA, 0x6F, 0x2D, 0xAE, 0x2C, 0xE1, 0x01, 0x8D, 0xA3, 0x57, 0x31, 0xE6, 0x97, 0xB1,
		0x2A, 0x02, 0xE8, 0x30, 0x0E, 0xC6, 0x91, 0x04, 0x8E, 0x82, 0xEC, 0x97, 0x26, 0xA0, 0xCC, 0xF7,
		0x4F, 0x93, 0x0C, 0xF4, 0x24, 0xA7, 0xDB, 0x7A, 0xFA, 0x68, 0x91, 0xDA, 0x04, 0xDC, 0xD8, 0x2A,
		0xB2, 0x46, 0x31, 0x82, 0xB6, 0x61, 0x57, 0x39, 0x93, 0x95, 0xDE, 0xD5, 0x02, 0x25, 0x21, 0x45,
		0xBE, 0x7D, 0xBA, 0xD7, 0x5E, 0x31, 0x36, 0x9D, 0x92, 0x34, 0xC1, 0x07, 0x27, 0x84, 0x9A, 0xEA,
		0x62, 0xD0, 0x78, 0x4D, 0xF3, 0x7E, 0xB5, 0x4A, 0xE2, 0x7A, 0x0B, 0x11, 0x5A, 0x6D, 0x1C, 0xD9,
		0x8E, 0xF2, 0xA6, 0xCC, 0x2B, 0x0B, 0x3A, 0x6B, 0x0C, 0xED, 0x4C, 0xCA, 0x0B, 0xD9, 0x55, 0x83,
		0x9C, 0x9C, 0x4B, 0x41, 0x63, 0x8E, 0x8D, 0x3A, 0x11, 0xD5, 0xB3, 0xD4, 0xAB, 0x09, 0x53, 0xE1,
		0x77, 0x52, 0x28, 0xE9, 0x18, 0x5C, 0x50, 0xA3, 0xE9, 0xBD, 0xF7, 0x48, 0xBE, 0xB6, 0x0D, 0x92,
		0xE0, 0x07, 0x3A, 0x89, 0x9B, 0x8B, 0xE2, 0x61, 0xA8, 0x8B, 0xD2, 0x64, 0xEB, 0xD8, 0xB9, 0xEA,
		0xA9, 0x4B, 0xF5, 0x5A, 0x58, 0xA0, 0x80, 0xFF, 0x6A, 0xEE, 0x73, 0x39, 0x89, 0xAC, 0x4C, 0x96,
		0x6F, 0x05, 0x10, 0x61, 0xB3, 0x28, 0xF6, 0x5C, 0x1B, 0x2F, 0xEB, 0xD8, 0xA9, 0x0F, 0xA4, 0x05,
		0x3C, 0x11, 0xAB, 0xD1, 0x42, 0xFE, 0x8A, 0xD9, 0x32, 0xCB, 0xA1, 0x13, 0xC3, 0xC3, 0x60, 0x43,
		0x13, 0x9A, 0x4C, 0x3B, 0x72, 0x20, 0xA8, 0x52, 0x4B, 0x6E, 0x6F, 0x6F, 0x41, 0xC0, 0xE5, 0x90,
		0xB8, 0xCF, 0x49, 0xDE, 0x91, 0xA2, 0x8D, 0xE2, 0x7D, 0xB5, 0x42, 0x28, 0x8A, 0xD3, 0x24, 0xB9,
		0x46, 0x1C, 0xCF, 0x0E, 0x6B, 0xA7, 0xB3, 0xC4, 0x19, 0x21, 0x4E, 0xD7, 0xE5, 0x6C, 0x48, 0x9D,
		0x6E, 0x05, 0x9B
	};

    CobsEncodingGenerator generator(outpost::asSlice(input));
    size_t encodedLength = getEncodedArray(generator, actual, sizeof(actual));

    ASSERT_EQ(sizeof(expected), encodedLength);
    EXPECT_THAT(actual, ElementsAreArray(expected));
}

// ----------------------------------------------------------------------------
/*
 * The generator should be able to continue its work it it is being copied.
 */
TEST(CobsGeneratorTest, CopyDuringDoubleBlockEncoding)
{
    uint8_t input[] = { 10, 11, 0, 13 };
    // expected output: 0x03, 10, 11, 0x02, 13

    CobsEncodingGenerator generator(outpost::asSlice(input));

    EXPECT_EQ(0x03, generator.getNextByte());
    EXPECT_EQ(10, generator.getNextByte());

    CobsEncodingGenerator generator2(generator);

    EXPECT_FALSE(generator.isFinished());
    EXPECT_EQ(11, generator.getNextByte());
    EXPECT_EQ(0x02, generator.getNextByte());
    EXPECT_EQ(13, generator.getNextByte());
    EXPECT_TRUE(generator.isFinished());

    // generator2 continues where generator was copied.
    EXPECT_FALSE(generator2.isFinished());
    EXPECT_EQ(11, generator2.getNextByte());
    EXPECT_EQ(0x02, generator2.getNextByte());
    EXPECT_EQ(13, generator2.getNextByte());
    EXPECT_TRUE(generator2.isFinished());
}

TEST(CobsGeneratorTest, CopyAssignmentDuringDoubleBlockEncoding)
{
    uint8_t input[] = { 10, 11, 0, 13 };
    // expected output: 0x03, 10, 11, 0x02, 13

    CobsEncodingGenerator generator(outpost::asSlice(input));

    EXPECT_EQ(0x03, generator.getNextByte());
    EXPECT_EQ(10, generator.getNextByte());

    CobsEncodingGenerator generator2(generator);

    EXPECT_FALSE(generator.isFinished());
    EXPECT_EQ(11, generator.getNextByte());
    EXPECT_EQ(0x02, generator.getNextByte());

    generator2 = generator;

    EXPECT_EQ(13, generator.getNextByte());
    EXPECT_TRUE(generator.isFinished());

    // generator2 continues where is was assigned from generator.
    EXPECT_FALSE(generator2.isFinished());
    EXPECT_EQ(13, generator2.getNextByte());
    EXPECT_TRUE(generator2.isFinished());
}
