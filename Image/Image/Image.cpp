#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

#include "Image.h"

namespace Image
{

	void readTextFile(const std::string& textFile, int& widthImage, int& heightImage, int& bytesPerPixel)
	{
		std::ifstream textFilePath;
		textFilePath.open(textFile);

		if (!textFilePath.is_open())
		{
			std::cout << "Unable to open the file " << textFile << std::endl;
			return;
		}


		textFilePath >> widthImage >> heightImage >> bytesPerPixel;
		if (textFilePath.fail())
		{
			std::cerr << "Invalid characters in text file: " << textFile << std::endl;
			return;
		}
		textFilePath.close();

		if (widthImage <= 0 || heightImage <= 0)
		{
			std::cerr << "Invalid image size in text file: width: " << widthImage << " height: " << heightImage << std::endl;
			return;
		}

		if (bytesPerPixel < 1 || bytesPerPixel  > 4)
		{
			std::cerr << "Invalid the size of a pixel: " << bytesPerPixel << std::endl;
			return;
		}
	}

	void readBinaryFile(const std::string& binaryFile, std::vector< std::vector<int> >& pixel_map, const int& imageSize, const int& widthImage, const int& heightImage, const int& bytesPerPixel)
	{
		std::ifstream binaryFilePath;
		binaryFilePath.open(binaryFile, std::ios::in | std::ios::binary | std::ios::ate);
		if (!binaryFilePath)
		{
			std::cout << "Unable to open the file " << binaryFile << std::endl;
			return;
		}

		auto binaryFileSize = (int)binaryFilePath.tellg();

		if (binaryFileSize != imageSize)
		{
			std::cout << "The binary file, " << binaryFile << ", was incorrect created " << std::endl;
			return;
		}

		binaryFilePath.seekg(0, std::ios::beg);
		uint32_t pixel = 0;
		pixel_map.resize(heightImage);

		for (int i = 0; i < heightImage; i++)
		{
			for (int j = 0; j < widthImage; j++)
			{
				binaryFilePath.read((char*)&pixel, bytesPerPixel);
				pixel_map[i].push_back(pixel);
			}
		}
		binaryFilePath.close();
	}

	bool verifyTransformationMatrix(const TransformationMatrix tr)
	{
		for (unsigned i = 0; i < tr.size(); i++)
		{
			int elementsNotZero = 0;

			for (unsigned j = 0; j < tr[0].size(); j++)
			{
				if (tr[i][j] < -1 || tr[i][j] > 1)
					return false;

				if (tr[i][j] != 0)
					++elementsNotZero;
			}

			if (elementsNotZero != 1)
				return false;
		}

		for (unsigned j = 0; j < tr[0].size(); j++)
		{
			int elementsNotZero = 0;

			for (unsigned i = 0; i < tr.size(); ++i)
				if (tr[i][j] != 0)
					++elementsNotZero;

			if (elementsNotZero != 1)
				return false;
		}

		return true;
	}

	void multiplyMatrix(const std::vector< std::vector<int> >& position, const TransformationMatrix tr, std::vector< std::vector<int> >& newPosition)
	{
		if (position[0].size() != tr.size())
		{
			std::cout << "These  matrices can't be multiplied!" << std::endl;
			return;
		}

		int resultElements;
		for (size_t i = 0; i < position.size(); i++)
		{
			for (size_t j = 0; j < tr[0].size(); j++)
			{
				resultElements = 0;

				for (size_t k = 0; k < position[0].size(); k++)
				{
					resultElements += position[i][k] * tr[k][j];
				}

				newPosition[i][j] = resultElements;
			}
		}
	}

	bool writeTextFile(const std::string& fileName, int widthImage, int heightImage, int bytesPerPixel)
	{
		std::ofstream textFile(fileName);
		if (!textFile)
		{
			std::cout << "Unable to open/create the text file " << std::endl;
			return false;
		}

		textFile << widthImage << std::endl << heightImage << std::endl << bytesPerPixel << std::endl;

		textFile.close();

		return true;
	}

	bool writeBinaryFile(const std::string& fileName, const std::vector< std::vector<int> >& pixel_map, const int& bytesPerPixel)
	{
		if (bytesPerPixel < 1 || bytesPerPixel > 4)
			return false;

		std::ofstream binaryFile(fileName, std::ios::binary);

		if (!binaryFile)
		{
			std::cout << "Unable to open/create the binary file " << std::endl;
			return false;
		}

		for (auto& line : pixel_map)
			for (uint32_t pixel : line)
				binaryFile.write(reinterpret_cast<char *>(&pixel), bytesPerPixel);

		return true;
	}

	void transformPosition(int& tx, int& ty, int x, int y, const TransformationMatrix tr)
	{
		tx = x * tr[0][0] + y * tr[0][1];
		ty = x * tr[1][0] + y * tr[1][1];
	}

	void transposeMatrix(std::vector< std::vector<int> >& pixel_map, const TransformationMatrix tm)
	{
		int minX, maxX, minY, maxY;
		int width = pixel_map[0].size();
		int height = pixel_map.size();

		transformPosition(minX, minY, 0, 0, tm);
		transformPosition(maxX, maxY, width - 1, height - 1, tm);

		if (minX > maxX)
			std::swap(minX, maxX);
		if (minY > maxY)
			std::swap(minY, maxY);

		auto newHeight = maxY - minY + 1;
		auto newWidth = maxX - minX + 1;

		std::vector< std::vector<int> > transMap(newHeight);
		for (auto& line : transMap)
			line.resize(newWidth);

		for (size_t y = 0; y < pixel_map.size(); y++)
		{
			for (size_t x = 0; x < pixel_map[y].size(); x++)
			{
				int tx, ty;
				transformPosition(tx, ty, x, y, tm);
				transMap[ty - minY][tx - minX] = pixel_map[y][x];
			}
		}

		pixel_map = transMap;
	}

}