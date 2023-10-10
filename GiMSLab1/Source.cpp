// bmp_editor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;


#pragma pack(2)

//��������� ����� BMP 
typedef struct tBITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
}sFileHead;

//��������� BitMap's
typedef struct tBITMAPINFOHEADER
{
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
}sInfoHead;

//��������� ����� BKA
typedef struct tBKAHEADER
{
	WORD bfType;
	WORD bfVersion;
	LONG bfProgramName;
	DWORD bfSize;
	WORD bfHeaderSize;
	DWORD bfRasterSize;
	BYTE bfBytePerPixel;
	DWORD bfWidth;
}sBKAHead;

sFileHead FileHead;
sInfoHead InfoHead;
sBKAHead BKAHead;

//�������
struct Color
{
	BYTE blue;
	BYTE green;
	BYTE red;
};

//������ 1-�� �������
int pixel_size = sizeof(Color);


//1 - BMP, 2 - BKA
int img_type = 0;

//�������� �����������
Color* src_image = 0;
//�������������� �����������
Color* dst_image = 0;

//������ �����������
int width = 0;
int height = 0;

//������� ��������� BMP �����
void ShowBMPHeaders(tBITMAPFILEHEADER fh, tBITMAPINFOHEADER ih)
{
	cout << "Type: " << (CHAR)fh.bfType << endl;
	cout << "Size: " << fh.bfSize << endl;
	cout << "Shift of bits: " << fh.bfOffBits << endl;
	cout << "Width: " << ih.biWidth << endl;
	cout << "Height: " << ih.biHeight << endl;
	cout << "Planes: " << ih.biPlanes << endl;
	cout << "BitCount: " << ih.biBitCount << endl;
	cout << "Compression: " << ih.biCompression << endl;
}

//������� ��������� BKA �����
void ShowBKAHeaders(tBKAHEADER fh)
{
	cout << "Type: " << (CHAR)fh.bfType << endl;
	cout << "Size: " << fh.bfSize << endl;
	cout << "Width: " << fh.bfWidth << endl;
	cout << "Height: " << (BKAHead.bfSize - BKAHead.bfHeaderSize) / BKAHead.bfWidth << endl;
	cout << "Version: " << fh.bfVersion << endl;
	cout << "Program Name: " << fh.bfProgramName << endl;
	cout << "Header Size: " << fh.bfHeaderSize << endl;
	cout << "Raster Size: " << fh.bfRasterSize << endl;
	cout << "Byte per pixel: " << fh.bfBytePerPixel << endl;
}


//������� ��� �������� �����������
bool OpenImage(string path)
{
	ifstream img_file;
	Color temp;
	char buf[3];

	//������� ���� �� ������
	img_file.open(path.c_str(), ios::in | ios::binary);
	if (!img_file)
	{
		cout << "File isn`t open!" << endl;
		return false;
	}

	switch (img_type)
	{
	case 1:
		//������� ��������� BMP
		img_file.read((char*)&FileHead, sizeof(FileHead));
		img_file.read((char*)&InfoHead, sizeof(InfoHead));

		ShowBMPHeaders(FileHead, InfoHead);
		//��������� ����� � ������ �����������
		width = InfoHead.biWidth;
		height = InfoHead.biHeight;
		break;
	case 2:
		img_file.read((char*)&BKAHead, sizeof(BKAHead));

		ShowBKAHeaders(BKAHead);
		//��������� ����� � ������ �����������
		width = BKAHead.bfWidth;
		height = (BKAHead.bfSize - BKAHead.bfHeaderSize) / BKAHead.bfWidth;
		break;
	default:
		break;
	}

	//�������� ����� ��� �����������
	src_image = new Color[width * height];

	int i, j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img_file.read((char*)&temp, pixel_size);
			src_image[i * width + j] = temp;
		}
		//�������� ���� ������������ ��� ������������ �� �������� �����
		img_file.read((char*)buf, j % 4);
	}
	img_file.close();

	return true;
}

//������� ��� ���������� �����������
bool SaveImage(string path)
{
	ofstream img_file;
	char buf[3];

	//������� ���� �� ������
	img_file.open(path.c_str(), ios::out | ios::binary);
	if (!img_file)
	{
		return false;
	}

	switch (img_type)
	{
	case 1:
		img_file.write((char*)&FileHead, sizeof(FileHead));
		img_file.write((char*)&InfoHead, sizeof(InfoHead));

		//����������� �� ��������� � �������������� �����������
		dst_image = new Color[width * height];
		memcpy(dst_image, src_image, width * height * sizeof(Color));
		break;
	case 2:
		img_file.write((char*)&BKAHead, sizeof(BKAHead));

		//����������� �� ��������� � �������������� �����������
		dst_image = new Color[width * height];
		memcpy(dst_image, src_image, width * height * sizeof(Color));
		break;
	default:
		break;
	}


	//�������� ����
	int i, j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img_file.write((char*)&dst_image[i * width + j], pixel_size);
		}
		img_file.write((char*)buf, j % 4);
	}
	img_file.close();

	return true;
}

//����������� ���������� ��������������� ����������� � ���������
void CopyDstToSrc()
{
	if (dst_image != 0)
	{
		memcpy(src_image, dst_image, width * height * sizeof(Color));
	}
}

//���������� ����������� � �������� ����� �����������
void AddNoise(double probability)
{
	int size = width * height;
	int count = (int)(size * probability) / 100;
	int x, y;
	long pos;
	for (int i = 0; i < count; i++)
	{
		x = rand() % width;
		y = rand() % height;
		pos = y * width + x;
		src_image[pos].blue = rand() % 255;
		src_image[pos].green = rand() % 255;
		src_image[pos].red = rand() % 255;
	}
	cout << "Point was added: " << count << endl;
}

//���������� ������� ����������� � ������� ������ ������������ ������������
void ShowImage(string path)
{
	switch (img_type)
	{
	case 1:
		ShowBMPHeaders(FileHead, InfoHead);
		system(path.c_str());
		break;
	case 2:
	{
		//ShowBKAHeaders(BKAHead);

		sFileHead fileHeader;
		sInfoHead infoHeader;

		fileHeader.bfType = 0x4D42;
		fileHeader.bfSize = sizeof(sFileHead) + sizeof(sInfoHead) + height * width * sizeof(Color);
		fileHeader.bfReserved1 = 0;
		fileHeader.bfReserved2 = 0;
		fileHeader.bfOffBits = sizeof(sFileHead) + sizeof(sInfoHead);

		infoHeader.biSize = sizeof(sInfoHead);
		infoHeader.biWidth = width;
		infoHeader.biHeight = height;
		infoHeader.biPlanes = 1;
		infoHeader.biBitCount = sizeof(Color) * 8;
		infoHeader.biCompression = 0;
		infoHeader.biSizeImage = height * width * sizeof(Color);
		infoHeader.biXPelsPerMeter = 0;
		infoHeader.biYPelsPerMeter = 0;
		infoHeader.biClrUsed = 0;
		infoHeader.biClrImportant = 0;

		string newName = string((path.substr(0, path.length() - 4) + ".bmp"));
		std::ofstream file(newName, std::ios::binary | std::ios::trunc);

		char buf[3];
		if (file)
		{
			// ���������� ��������� ����� BMP
			file.write(reinterpret_cast<char*>(&fileHeader), sizeof(sFileHead));
			file.write(reinterpret_cast<char*>(&infoHeader), sizeof(sInfoHead));

			// ���������� ������ ��������
			int i, j;
			for (i = 0; i < height; i++)
			{
				for (j = 0; j < width; j++)
				{
					file.write((char*)&src_image[i * width + j], pixel_size);
				}
				file.write((char*)buf, j % 4);
			}

			file.close();
			std::cout << "BMP image created successfully: " << newName << std::endl;
			system(newName.c_str());
		}
		else
		{
			std::cout << "Failed to create BMP image: " << string((path.substr(path.length() - 4) + ".bmp")) << std::endl;
		}
		break;
	}
	default:
		break;
	}
}

//������� ���� � �����������
void ReadPath(string& str)
{
	string bmp = ".bmp", bka = ".bka";
	while (true)
	{
		str.clear();
		cout << "Enter path to image" << endl;
		cin >> str;
		
		/*if (bmp == str.substr(str.length() - bmp.length()) || bka == str.substr(str.length() - bka.length()))
		{
			break;
		}*/
		
		if (bmp == str.substr(str.length() - bmp.length()))
		{
			img_type = 1;
			break;
		}
		if (bka == str.substr(str.length() - bka.length()))
		{
			img_type = 2;
			break;
		}
		cout << "Wrong file format!" << endl;
	}
}

void MedianFilter()
{
	int filterWidth = 5, filterHeight = 1;
	BYTE redChannel[5], greenChannel[5], blueChannel[5];

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int counter = 0;
			for (int k = j - 2; k <= j + 2; k++)
			{
				redChannel[counter] = src_image[abs(i * width + k)].red;
				greenChannel[counter] = src_image[abs(i * width + k)].green;
				blueChannel[counter] = src_image[abs(i * width + k)].blue;
				++counter;
			}
			/*int counter = 0;
			for (int k = j - 2; k <= j + 2; k++)
			{
				
				if (k < 0)
				{
					redChannel[counter] = src_image[i * width + 0].red;
					greenChannel[counter] = src_image[i * width + 0].green;
					blueChannel[counter] = src_image[i * width + 0].blue;
					++counter;
					continue;
				}
				if (k > width)
				{
					redChannel[counter] = src_image[i * width + width].red;
					greenChannel[counter] = src_image[i * width + width].green;
					blueChannel[counter] = src_image[i * width + width].blue;
					++counter;
					continue;
				}
				redChannel[counter] = src_image[i * width + k].red;
				greenChannel[counter] = src_image[i * width + k].green;
				blueChannel[counter] = src_image[i * width + k].blue;
				++counter;
			}*/
			
			sort(redChannel, redChannel + 5);
			sort(greenChannel, greenChannel + 5);
			sort(blueChannel, blueChannel + 5);			

			src_image[i * width + j].red = redChannel[(filterWidth - 1) / 2];
			src_image[i * width + j].green = greenChannel[(filterWidth - 1) / 2];
			src_image[i * width + j].blue = blueChannel[(filterWidth - 1) / 2];
		}
	}
}


void ClearMemory(void) {
	//���������� ������ ��������� �����������
	if (src_image != 0)
	{
		delete[] src_image;
	}
	//���������� ������ �������������� �����������
	if (dst_image != 0)
	{
		delete[] dst_image;
	}
}

int main(int argc, char* argv[])
{
	srand((unsigned)time(NULL));

	//���� � �������� �����������
	string path_to_image, temp;


	ReadPath(path_to_image);
	OpenImage(path_to_image);
	ShowImage(path_to_image);
	AddNoise(15);
	MedianFilter();

	BKAHead.bfType = 0x4D42; // 'BM'
	BKAHead.bfVersion = 1;
	BKAHead.bfProgramName = 0x474E494B; // 'GNIK'
	BKAHead.bfSize = sizeof(sBKAHead) + width * height;
	BKAHead.bfHeaderSize = sizeof(sBKAHead);
	BKAHead.bfRasterSize = width * height;
	BKAHead.bfBytePerPixel = 3;
	BKAHead.bfWidth = width;

	ReadPath(temp);
	SaveImage(temp);
	ShowImage(temp);
	ClearMemory();
	cout << "END!" << endl;
	return 0;
}


/*
// bmp_editor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;


#pragma pack(2)

//��������� ����� BMP 
typedef struct tBITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
}sFileHead;

//��������� BitMap's
typedef struct tBITMAPINFOHEADER
{
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
}sInfoHead;

//��������� ����� BKA
typedef struct tBKAHEADER
{
	WORD bfType;
	WORD bfVersion;
	LONG bfProgramName;
	DWORD bfSize;
	WORD bfHeaderSize;
	DWORD bfRasterSize;
	BYTE bfBytePerPixel;
	DWORD bfWidth;
}sBKAHead;

sFileHead FileHead;
sInfoHead InfoHead;
sBKAHead BKAHead;

//�������
struct Color
{
	BYTE blue;
	BYTE green;
	BYTE red;
};

//������ 1-�� �������
int pixel_size = sizeof(Color);


//1 - BMP, 2 - CMP
int img_type = 0;

//�������� �����������
Color* src_image = 0;
//�������������� �����������
Color* dst_image = 0;

//������ �����������
int width = 0;
int height = 0;

//������� ��������� BMP �����
void ShowBMPHeaders(tBITMAPFILEHEADER fh, tBITMAPINFOHEADER ih)
{
	cout << "Type: " << (CHAR)fh.bfType << endl;
	cout << "Size: " << fh.bfSize << endl;
	cout << "Shift of bits: " << fh.bfOffBits << endl;
	cout << "Width: " << ih.biWidth << endl;
	cout << "Height: " << ih.biHeight << endl;
	cout << "Planes: " << ih.biPlanes << endl;
	cout << "BitCount: " << ih.biBitCount << endl;
	cout << "Compression: " << ih.biCompression << endl;
}

//������� ��������� BKA �����
void ShowBMPHeaders(tBKAHEADER fh, tBITMAPINFOHEADER ih)
{
	cout << "Type: " << (CHAR)fh.bfType << endl;
	cout << "Size: " << fh.bfSize << endl;
	//cout << "Shift of bits: " << fh.bfOffBits << endl;
	cout << "Width: " << ih.biWidth << endl;
	cout << "Height: " << ih.biHeight << endl;
	cout << "Planes: " << ih.biPlanes << endl;
	cout << "BitCount: " << ih.biBitCount << endl;
	cout << "Compression: " << ih.biCompression << endl;
}

//������� ��� �������� �����������
bool OpenImage(string path)
{
	ifstream img_file;
	Color temp;
	char buf[3];

	//������� ���� �� ������
	img_file.open(path.c_str(), ios::in | ios::binary);
	if (!img_file)
	{
		cout << "File isn`t open!" << endl;
		return false;
	}

	//������� ��������� BMP
	img_file.read((char*)&FileHead, sizeof(FileHead));
	img_file.read((char*)&InfoHead, sizeof(InfoHead));

	img_type = 1;
	ShowBMPHeaders(FileHead, InfoHead);
	//��������� ����� � ������ �����������
	width = InfoHead.biWidth;
	height = InfoHead.biHeight;


	//�������� ����� ��� �����������
	src_image = new Color[width * height];

	int i, j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img_file.read((char*)&temp, pixel_size);
			src_image[i * width + j] = temp;
		}
		//�������� ���� ������������ ��� ������������ �� �������� �����
		img_file.read((char*)buf, j % 4);
	}
	img_file.close();

	return true;
}

//������� ��� ���������� �����������
bool SaveImage(string path)
{
	ofstream img_file;
	char buf[3];

	//������� ���� �� ������
	img_file.open(path.c_str(), ios::out | ios::binary);
	if (!img_file)
	{
		return false;
	}

	img_file.write((char*)&FileHead, sizeof(FileHead));
	img_file.write((char*)&InfoHead, sizeof(InfoHead));

	//����������� �� ��������� � �������������� �����������
	dst_image = new Color[width * height];
	memcpy(dst_image, src_image, width * height * sizeof(Color));


	//�������� ����
	int i, j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img_file.write((char*)&dst_image[i * width + j], pixel_size);
		}
		img_file.write((char*)buf, j % 4);
	}
	img_file.close();

	return true;
}

//����������� ���������� ��������������� ����������� � ���������
void CopyDstToSrc()
{
	if (dst_image != 0)
	{
		memcpy(src_image, dst_image, width * height * sizeof(Color));
	}
}

//���������� ����������� � �������� ����� �����������
void AddNoise(double probability)
{
	int size = width * height;
	int count = (int)(size * probability) / 100;
	int x, y;
	long pos;
	for (int i = 0; i < count; i++)
	{
		x = rand() % width;
		y = rand() % height;
		pos = y * width + x;
		src_image[pos].blue = rand() % 255;
		src_image[pos].green = rand() % 255;
		src_image[pos].red = rand() % 255;
	}
	cout << "Point was added: " << count << endl;
}

//���������� ������� ����������� � ������� ������ ������������ ������������
void ShowImage(string path)
{
	ShowBMPHeaders(FileHead, InfoHead);
	system(path.c_str());
}

//������� ���� � �����������
void ReadPath(string& str)
{
	string bmp = ".bmp", bka = ".bka";
	while (true)
	{
		str.clear();
		cout << "Enter path to image" << endl;
		cin >> str;
		if (bmp == str.substr(str.length() - bmp.length()))
		{
			img_type = 1;
			break;
		}
		if (bka == str.substr(str.length() - bka.length()))
		{
			img_type = 2;
			break;
		}
		cout << "Wrong file format!" << endl;
	}
}

void MedianFilter()
{
	int filterWidth = 5, filterHeight = 1;
	BYTE redChannel[5], greenChannel[5], blueChannel[5];

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int counter = 0;
			for (int k = j - 2; k <= j + 2; k++)
			{
				redChannel[counter] = src_image[abs(i * width + k)].red;
				greenChannel[counter] = src_image[abs(i * width + k)].green;
				blueChannel[counter] = src_image[abs(i * width + k)].blue;
				++counter;
			}

			sort(redChannel, redChannel + 5);
			sort(greenChannel, greenChannel + 5);
			sort(blueChannel, blueChannel + 5);

			src_image[i * width + j].red = redChannel[(filterWidth - 1) / 2];
			src_image[i * width + j].green = greenChannel[(filterWidth - 1) / 2];
			src_image[i * width + j].blue = blueChannel[(filterWidth - 1) / 2];
		}
	}
}


void ClearMemory(void) {
	//���������� ������ ��������� �����������
	if (src_image != 0)
	{
		delete[] src_image;
	}
	//���������� ������ �������������� �����������
	if (dst_image != 0)
	{
		delete[] dst_image;
	}
}

int main(int argc, char* argv[])
{
	srand((unsigned)time(NULL));

	//���� � �������� �����������
	string path_to_image, temp;

	sBKAHead header;
	header.bfType = 0x4D42; // 'BM'
	header.bfVersion = 1;
	header.bfProgramName = 0x474E494B; // 'GNIK'
	header.bfSize = sizeof(sBKAHead);
	header.bfHeaderSize = sizeof(sBKAHead);
	header.bfRasterSize = 0;
	header.bfBytePerPixel = 3;
	header.bfWidth = 640;

	ReadPath(path_to_image);
	OpenImage(path_to_image);
	//ShowImage(path_to_image);
	AddNoise(15);
	MedianFilter();
	ReadPath(temp);
	SaveImage(temp);
	//ShowImage(temp);
	ClearMemory();
	cout << "END!" << endl;
	return 0;
}*/

/*#include <iostream>
#include <fstream>
#include <Windows.h>

typedef struct tBKAHEADER
{
	WORD bfType;
	WORD bfVersion;
	LONG bfProgramName;
	DWORD bfSize;
	WORD bfHeaderSize;
	DWORD bfRasterSize;
	BYTE bfBytePerPixel;
	DWORD bfWidth;
} sBKAHead;

int main() {
	std::string imageFilePath = "1.bmp";
	std::string bkaFilePath = "image.bka";

	// ��������� ����������� BMP
	std::ifstream imageFile(imageFilePath, std::ios::binary);
	if (!imageFile) {
		std::cout << "������ ��� �������� ����������� BMP" << std::endl;
		return 1;
	}

	// �������� ������ �����
	imageFile.seekg(0, std::ios::end);
	std::streampos imageSize = imageFile.tellg();
	imageFile.seekg(0, std::ios::beg);

	// ������� � ��������� ��������� BKA
	sBKAHead bkaHeader;
	bkaHeader.bfType = 0x4D42; // "BM" � little endian
	bkaHeader.bfVersion = 1;
	bkaHeader.bfProgramName = 0;
	bkaHeader.bfSize = sizeof(sBKAHead) + imageSize;
	bkaHeader.bfHeaderSize = sizeof(sBKAHead);
	bkaHeader.bfRasterSize = imageSize;
	bkaHeader.bfBytePerPixel = 3; // ������������, ��� ����������� � ������� RGB
	bkaHeader.bfWidth = 640; // TODO: ������� ������ ����������� BMP

	// ��������� ���� BKA ��� ������
	std::ofstream bkaFile(bkaFilePath, std::ios::binary);
	if (!bkaFile) {
		std::cout << "������ ��� �������� ����� BKA" << std::endl;
		return 1;
	}

	// ���������� ��������� BKA
	bkaFile.write(reinterpret_cast<const char*>(&bkaHeader), sizeof(sBKAHead));

	// �������� ���������� ����������� BMP � ���� BKA
	bkaFile << imageFile.rdbuf();

	// ��������� �����
	imageFile.close();
	bkaFile.close();

	// ���������� ���� BKA
	std::string systemCommand = "start " + bkaFilePath;
	std::system(systemCommand.c_str());

	return 0;
}*/