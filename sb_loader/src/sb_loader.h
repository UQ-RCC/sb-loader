#pragma once

#include <math.h>
#include <utility>
#include <unordered_set>
#include <string>
#include <vector>
#include "fmt/format.h"
#include "SBReadFile.h"
#include "phoebe_util/util.h"

namespace util
{
	template <int N = 0>
	class RangePrinter
	{
	public:
		int max;		
		int numDigits;
		RangePrinter() {}
		RangePrinter(int m) : numDigits(m + N == 0 ? 1 : (int)(floor(log10(m + N)) + 1)) {}
		std::string string(int v) const
		{			
			return fmt::format("{:0{}}", v + N, numDigits);
		}
	};
}

class CaptureDataFrame
{
public:

	III::SBReadFile * sb_read_file;

	CaptureIndex number_captures;
	PositionIndex number_positions;
	ChannelIndex number_channels;
	TimepointIndex number_timepoints;

	CaptureIndex capture_index;
	PositionIndex position_index;
	ChannelIndex channels_index;
	TimepointIndex timepoint_index;
	
	SInt32 xDim;
	SInt32 yDim;
	SInt32 zDim;
	bool has_voxel_size = false;
	float voxel_size[3];
	std::string image_name;
	std::string image_comments;
	std::string capture_date;
	std::string lens_name;
	std::vector<std::string> channel_names;
	std::vector<SInt32> exposure_time;

	util::RangePrinter<1> capture_index_fmt;
	util::RangePrinter<1> channel_index_fmt;
	util::RangePrinter<1> position_index_fmt;
	util::RangePrinter<1> timepoint_index_fmt;
	util::RangePrinter<0> elapsed_range_fmt;

	static std::string GetString(III::SBReadFile * sb_read_file, int capture_index, UInt32(III::SBReadFile::*sb_string_read)(char *, const CaptureIndex ci) const)
	{
		std::string sb_data;
		int char_count = (sb_read_file->*sb_string_read)(nullptr, capture_index);
		if (char_count > 0)
		{
			char * cbuff = new char[char_count];
			(sb_read_file->*sb_string_read)(cbuff, capture_index);
			sb_data = std::string(cbuff);
			delete[] cbuff;
		}
		return sb_data;
	}

	static std::string GetString(III::SBReadFile * sb_read_file, int capture_index, int channel_index, UInt32(III::SBReadFile::*sb_string_read)(char *, const CaptureIndex, const ChannelIndex) const)
	{
		std::string sb_data;
		int char_count = (sb_read_file->*sb_string_read)(nullptr, capture_index, channel_index);
		if (char_count > 0)
		{
			char * cbuff = new char[char_count];
			(sb_read_file->*sb_string_read)(cbuff, capture_index, channel_index);
			sb_data = std::string(cbuff);
			delete[] cbuff;
		}
		return sb_data;
	}

	CaptureDataFrame(III::SBReadFile * sb_read_file, CaptureIndex capture_index, PositionIndex position_index)
		: sb_read_file(sb_read_file)
		, capture_index(capture_index)
		, position_index(position_index)
		, number_captures(sb_read_file->GetNumCaptures())
		, number_channels(sb_read_file->GetNumChannels(capture_index))
		, number_positions(sb_read_file->GetNumPositions(capture_index))
		, number_timepoints(sb_read_file->GetNumTimepoints(capture_index))
		, xDim(sb_read_file->GetNumXColumns(capture_index))
		, yDim(sb_read_file->GetNumYRows(capture_index))
		, zDim(sb_read_file->GetNumZPlanes(capture_index))
		, image_name(GetString(sb_read_file, capture_index, &III::SBReadFile::GetImageName))
		, image_comments(GetString(sb_read_file, capture_index, &III::SBReadFile::GetImageComments))
		, capture_date(GetString(sb_read_file, capture_index, &III::SBReadFile::GetCaptureDate))
		, lens_name(GetString(sb_read_file, capture_index, &III::SBReadFile::GetLensName))
		, capture_index_fmt(number_captures)
		, channel_index_fmt(number_channels)
		, position_index_fmt(number_positions)
		, timepoint_index_fmt(number_timepoints)
		, elapsed_range_fmt(sb_read_file->GetElapsedTime(capture_index, number_timepoints - 1))
	{
		has_voxel_size = sb_read_file->GetVoxelSize(capture_index, voxel_size[0], voxel_size[1], voxel_size[2]);
		if (!has_voxel_size)
		{
			voxel_size[0] = voxel_size[1] = voxel_size[2] = 1.0;
		}

		for (int i = 0; i < number_channels; i++)
		{
			channel_names.push_back(GetString(sb_read_file, capture_index, i, &III::SBReadFile::GetChannelName));
			exposure_time.push_back(sb_read_file->GetExposureTime(capture_index, i));
		}
		
	}

	std::string GetHeader(int capture_index, int position_index)
	{
		return fmt::format("capture {} of {} : position {} of {}, time points: {}, channels: {}",
			capture_index + 1, number_captures, position_index + 1, number_positions,
			number_timepoints, number_channels);
	}

	std::string GetDetail()
	{
		std::string metaData;
		metaData += fmt::format("Image name: {}\n", image_name);
		metaData +=  fmt::format("Image size: [{},{},{}]\n", xDim, yDim, zDim);
		std::string voxel_status;
		if (!has_voxel_size)
		{
			voxel_status = "undefined defaulting ";
		}
		metaData += fmt::format("Voxel size: {}[{},{},{}]\n", voxel_status, voxel_size[0], voxel_size[1], voxel_size[2]);
		metaData += fmt::format("Image comments: {}\n", image_comments);
		metaData += fmt::format("Capture date: {}\n", capture_date);
		metaData += fmt::format("Lens name: {}\n", lens_name);

		if (number_channels == 1)
		{
			metaData += fmt::format("Channel name: {}\n", channel_names[0]);
			metaData += fmt::format("Channel exposure time: {}ms\n", exposure_time[0]);
		}
		else
		{
			util::RangePrinter<1> channelRange(number_channels);
			for (int c = 0; c < number_channels; c++)
			{
				metaData += fmt::format("Channel {}\n   name: {}\n   exposure time: {}ms\n",
					channelRange.string(c),
					channel_names[c],
					exposure_time[c]);
			}
		}
		return metaData;
	}

	std::string GetCaptureIndexString()
	{
		return capture_index_fmt.string(capture_index);
	}

	std::string GetChannelIndexString()
	{
		return channel_index_fmt.string(channels_index);
	}

	std::string GetPositionIndexString()
	{
		return position_index_fmt.string(position_index);
	}

	std::string GetTimepointIndexString()
	{
		return timepoint_index_fmt.string(timepoint_index);
	}

	std::string GetElapsedString()
	{		
		return elapsed_range_fmt.string(sb_read_file->GetElapsedTime(capture_index, timepoint_index));
	}
	
};
