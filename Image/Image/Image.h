#pragma once

#include <string>
#include <vector>
#include <array>

namespace Image
{
	typedef std::array< std::array<int, 2>, 2> TransformationMatrix;
	typedef std::vector< std::vector<int> > PixelMap;

	void readTextFile(const std::string& textFile, int& widthImage, int& heightImage, int& bytesPerPixel);
	void readBinaryFile(const std::string& binaryFile, std::vector< std::vector<int> >& pixel_map, const int& imageSize, const int& widthImage, const int& heightImage, const int& bytesPerPixel);
	bool verifyTransformationMatrix(const TransformationMatrix tr);
	void multiplyMatrix(const std::vector< std::vector<int> >& position, const TransformationMatrix tr, std::vector< std::vector<int> >& newPosition);
	bool writeTextFile(const std::string& fileName, int widthImage, int heightImage, int bytesPerPixel);
	bool writeBinaryFile(const std::string& fileName, const std::vector< std::vector<int> >& pixel_map, const int& bytesPerPixel);
	void transformPosition(int& tx, int& ty, int x, int y, const TransformationMatrix tr);
	void transposeMatrix(std::vector< std::vector<int> >& imageData, const TransformationMatrix tr);
}
