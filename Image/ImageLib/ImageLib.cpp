// ImageLib.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include <iostream>
#include "../Image/Image.h"

using std::cerr;

extern "C" {
	__declspec(dllexport) void __stdcall transformImage(const char* txtPath, const char* binPath, const char* transformedSuffix, const int* transformationMatrix, const int sizeOfTransformationMatrix)
	{
		if (!txtPath || !strlen(txtPath))
		{
			cerr << "usage: <image_txt_file>" << std::endl;
			return;
		}

		if (!*binPath)
		{
			cerr << "usage: <image_bin_file>" << std::endl;
			return;
		}

		if (!*transformedSuffix)
		{
			std::cerr << "usage: " << "<name_for_new_image>" << std::endl;
			return;
		}

		if (transformationMatrix == NULL)
		{
			std::cerr << "usage: " << "<transformation_matrix>" << std::endl;
			return;
		}

		if (sizeOfTransformationMatrix != 4)
		{
			std::cerr << "usage: " << "<transformation_matrix_size>" << std::endl;
			return;
		}

		const std::string textFile = txtPath;
		const std::string binaryFile = binPath;
		int widthImage, heightImage, bytesPerPixel;
		Image::readTextFile(textFile, widthImage, heightImage, bytesPerPixel);

		int imageSize;
		imageSize = widthImage * heightImage * bytesPerPixel;

		std::vector< std::vector<int> > pixel_map;

		Image::readBinaryFile(binaryFile, pixel_map, imageSize, widthImage, heightImage, bytesPerPixel);
		int i = 0;
		Image::TransformationMatrix tr;
		for (auto& line : tr)
			for (auto& value : line)
				value = transformationMatrix[i++];

		if (!Image::verifyTransformationMatrix(tr))
		{
			std::cerr << "The transformation matrix is incorrect" << std::endl;
			return;
		}

		Image::transposeMatrix(pixel_map, tr);
		widthImage = pixel_map.front().size();
		heightImage = pixel_map.size();
		auto transformedTextFile = textFile.substr(0, textFile.find_last_of('.')) + transformedSuffix + ".txt";
		auto transformedBinaryFile = binaryFile.substr(0, binaryFile.find_last_of('.')) + transformedSuffix + ".bin";
		Image::writeTextFile(transformedTextFile, widthImage, heightImage, bytesPerPixel);
		Image::writeBinaryFile(transformedBinaryFile, pixel_map, bytesPerPixel);
	}
}