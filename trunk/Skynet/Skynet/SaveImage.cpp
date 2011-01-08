#include "StdAfx.h"
#include "SaveImage.h"

#ifndef OPENCV_DISABLED
#include <cv.h>
#include <highgui.h>
#endif

#include <string>

using namespace ImageUtil;

SaveImage::SaveImage( int height, int width, int channels )
{
	_height = height;
	_width = width;
	_channels = channels;
	//_fourcc = 0;
	//_fourcc = -1;
	_fourcc = CV_FOURCC('M','J','P','G');		// SET A GOOD DEFAULT: mpeg
	//_fourcc = CV_FOURCC('f','f','d','s');
	_fps = 30.0;
}


SaveImage::~SaveImage(void)
{
	stopVideo();
}


#ifndef OPENCV_DISABLED
cv::Mat
SaveImage::convertBuffer( float * buffer, float scale )
{
	cv::Mat retVal;
	typedef cv::Vec<float, 3> VT;
	int fullWidth = _width * _channels;

	retVal = cv::Mat( _height, _width, CV_32FC3 );

	cv::MatIterator_<VT> matIter = retVal.begin<VT>();

	// interlaced so iterate over rows / 2
	for( int row = 0; row < _height / 2; ++row )
	{
		// replicate each row twice
		for( int interLace = 0; interLace < 2; ++interLace )
		{
			for( int col = 0; col < _width; ++col )
			{
				// B G R format
				float r = buffer[row * fullWidth + col * _channels + 0] * scale;
				float g = buffer[row * fullWidth + col * _channels + 1] * scale;
				float b = buffer[row * fullWidth + col * _channels + 2] * scale;
				*matIter = VT( b, g, r );
				++matIter;
			}
		}
	}

	return retVal;
}
#endif

void
SaveImage::saveFrame( float * buffer, std::string path  )
{
	
#ifndef OPENCV_DISABLED
	cv::imwrite( path, convertBuffer( buffer, 255.0f ) );
#endif
}

void
SaveImage::saveFrame( float * buffer, std::string path, std::string pathbase, float * homography, double heading  )
{
	
#ifndef OPENCV_DISABLED
	typedef cv::Vec<float, 1> VT;
	cv::Mat image = convertBuffer( buffer, 255.0f );
	cv::Mat warped, rotated;
	cv::Mat rotation = cv::getRotationMatrix2D( cv::Point2f( image.cols/2, image.rows/2 ), heading, 1.0 );
	
	cv::Mat homo(3, 3, CV_32FC1 );
	homo.at<VT>( 0, 0 )[0] = homography[0];
	homo.at<VT>( 0, 1 )[0] = homography[1];
	homo.at<VT>( 0, 2 )[0] = homography[2];
	homo.at<VT>( 1, 0 )[0] = homography[3];
	homo.at<VT>( 1, 1 )[0] = homography[4];
	homo.at<VT>( 1, 2 )[0] = homography[5];
	homo.at<VT>( 2, 0 )[0] = homography[6];
	homo.at<VT>( 2, 1 )[0] = homography[7];
	homo.at<VT>( 2, 2 )[0] = homography[8];

	//cv::warpAffine( image, rotated, rotation, image.size() );
	//cv::imwrite( pathbase + "_rotated.jpg", rotated );

	//cv::warpPerspective( image, warped, homo.t(), image.size() );
	cv::warpAffine( image, rotated, rotation, image.size() );
	cv::imwrite( pathbase + "_rectified.jpg", rotated );

	cv::imwrite( path, image );
#endif
}

void
SaveImage::writeFrame( float * buffer )
{
	
#ifndef OPENCV_DISABLED
	if( _writer.isOpened() )
	{
		_writer << convertBuffer( buffer, 1.0f );
	}
#endif
}

void 
SaveImage::setupVideo( std::string path )
{
#ifndef OPENCV_DISABLED
	_writer = cv::VideoWriter( path, _fourcc, _fps, cv::Size( _width, _height ) );
#endif
}

void 
SaveImage::stopVideo( void )
{
#ifndef OPENCV_DISABLED
	if( _writer.isOpened() )
		_writer.~VideoWriter();
#endif
}

void
SaveImage::changeEncoding( std::string encoding )
{
	return;
	if( encoding == "None" )
	{
		//_fourcc = 0;
	}
	else if( encoding == "XviD" )
	{
		_fourcc = CV_FOURCC('x','v','i','d');
	}
	else if( encoding == "MJPG" )
	{
		_fourcc = CV_FOURCC('M','J','P','G');
	}
}