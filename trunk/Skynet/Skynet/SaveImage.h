#pragma once
#include <cv.h>
#include <highgui.h>
#include <string>

namespace ImageUtil
{
	class SaveImage
	{
	public:
		SaveImage( int height, int width, int channels );
		~SaveImage(void);

		void setupVideo( std::string path );
		void stopVideo( void );
		void changeEncoding( std::string encoding );

		void saveFrame( float * frame, std::string path );
		void saveFrame( float * buffer, std::string path, std::string pathbase, float * homography, double heading  );
		void writeFrame( float * frame );

	protected:
		cv::Mat convertBuffer( float * frame, float scale );

	private:
		int _height;
		int _width;
		int _channels;
		double _fps;
		int _fourcc;
		cv::VideoWriter _writer;
	};
}
