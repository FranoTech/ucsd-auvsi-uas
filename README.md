# ucsd-auvsi-uas
Automatically exported from code.google.com/p/ucsd-auvsi-uas

ucsd-auvsi-uas

Software to support autonomous airplane

We are a UCSD student team dedicated to building and flying an autonomous airplane. This software runs on the ground to support the plane and analyze results.

So far, two projects have been posted. The first ("Skynet") is the main GUI for the ground station. It requires 64-bit Windows and Visual Studio 2010 to compile. See http://190skynet.blogspot.com/ for information about included OCR code. For a general understanding of how our GUI operates, dive in!

The second ("AUVSI_SALIENCY") is our saliency program (built into a library). It handles saliency analysis of each video frame. It requires a CUDA-capable GPU, uses Visual Studio 2008 to compile and 64-bit Windows. See http://inperc.com/wiki/index.php?title=Saliency for more information on saliency.

How to build main GUI:

1) download and install Microsoft Visual Studio 2010 (free, legal)

go to dreamspark.com select VS 2010 professional (express might work too) jump through various hoops (verify through school, when you get to that step) download it install it

2) download the source code (free, legal)

install an SVN client (AnkhSVN, or tortoiseSVN), I recommend Tortoise. connect to svn server (https://ucsd-auvsi-uas.googlecode.com/svn/trunk/) wait for it to finish keep waiting now its done

3) open project

open the folder "Skynet", double click on project file

4) we need OpenCV:

you will need to install OpenCV 2.1. Download teh correct files (2.1, NOT 2.0), and follow this guide: http://opencv.willowgarage.com/wiki/VisualC++_VS2010_CMake

In visual studio, you will need to modify the locations of OpenCV. This is done by right clicking on the project and going to properties, you'll see something called "VC Directories".  You'll need to tell it where to find the header files (like cv.h), which is independent of debug/release, and also where to find the library files (things like cv210.lib) which are different for debug and release. Change all opncv related path names to the correct path names (like change D:/... to C://...).

Finally, we need to make one more change. Add the following to the beginning of cxmat.hpp (in the OpenCV folder):

pragma managed (push, off)

Add the following to the end of cxmatt.hpp:

pragma managed (pop)

5) to build

you should be able to figure out how to build it (hint: click on green triangle)

6) now it should work
