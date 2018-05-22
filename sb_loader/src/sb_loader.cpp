#include "sb_loader.h"


void ConvertSBImages(const std::string & filename);

int main(int argc, char ** argv)
{	
	if (argc != 2)
	{
		fmt::print("filename requried\n {}", argc);
		EXIT(0);
	}
	std::string filename = argv[1];
	fmt::print("Slidebook test converter v0.1\n");	
	fmt::print("{}\n", filename);
	ConvertSBImages(filename);
	fmt::print("done\n");
	EXIT(0);
}

void ConvertSBImages(const std::string & filename) try
{
	
	auto sb_read_file = III_NewSBReadFile(filename.c_str(), III::kNoExceptionsMasked);		
	fmt::print("sb file loaded\n");
	
	auto captures = sb_read_file->GetNumCaptures();
	fmt::print("captures: {}\n", captures);
	
	const int Dimension = 3;

	CaptureIndex number_captures = sb_read_file->GetNumCaptures();
	for (int capture_index = 0; capture_index < number_captures; capture_index++)
	{
		CaptureDataFrame cp(sb_read_file, capture_index, 0);
		fmt::print("{}\n", cp.GetHeader(capture_index, cp.position_index));
		
		using PixelType = UInt16;
		std::size_t planeSize = cp.xDim * cp.yDim;
		int bufferSize = cp.xDim * cp.yDim * cp.zDim;
		PixelType * buffer = new UInt16[bufferSize];
		
		int cappedTime = cp.number_timepoints;
		/*		
		if (options.max_time > -1)
		{
			cappedTime = std::min(cappedTime, options.max_time);
		}
		*/

		for (int timepoint_index = 0; timepoint_index < cappedTime; timepoint_index++)
		{
			cp.timepoint_index = timepoint_index;
			for (int c = 0; c < cp.number_channels; c++)
			{
				cp.channels_index = c;
				for (int z = 0; z < cp.zDim; z++)
				{
					sb_read_file->ReadImagePlaneBuf(buffer + (z * planeSize), capture_index, 0, timepoint_index, z, c);
				}
				fmt::print("read buffer capture: {} time: {} channel: {}\n", capture_index, timepoint_index, c);				
			}
		}
		delete[] buffer;
	
	}
}
catch (const III::Exception * e)
{
	fmt::print("Failed with exception: {}\n", e->GetDescription());	
	std::string inString;
	EXIT(1);
	delete e;
}