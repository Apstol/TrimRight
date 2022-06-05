#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <thread>
#include <vector>

void TrimRight(char *s)
{
	const std::size_t length = std::strlen(s);

	if (!length)
	{
		return;
	}

	char *end = s + length - 1;

	while ((end != s) && isspace(*end))
	{
		--end;
	}

	*(end + 1) = '\0';
}

void TrimRightMultiThreaded(char *s)
{
	const std::size_t length = std::strlen(s);

	if (!length)
	{
		return;
	}

	const int numThreads = 10;
	const int step       = length / numThreads;

	std::vector<std::thread> threads(numThreads);
	std::vector<char *> result(numThreads);

	for (int i = 0; i < numThreads; ++i)
	{
		char *chunkStart = s + i * step;
		char *chunkEnd   = chunkStart + step;

		if (i == numThreads - 1)
		{
			const int remainder = (length % numThreads) - 1;
			chunkEnd += remainder;
		}

		threads.push_back(std::thread([i, chunkStart, chunkEnd, &result]() mutable {
			while ((chunkEnd != chunkStart) && std::isspace(*chunkEnd))
			{
				--chunkEnd;
			}
			result[i] = chunkEnd;
		}));
	}

	for (std::thread &t : threads)
	{
		if (t.joinable())
		{
			t.join();
		}
	}

	for (auto it = result.rbegin(); it != result.rend(); ++it)
	{
		if (!std::isspace(*(*it)))
		{
			*(*it + 1) = '\0';
			break;
		}
	}
}

char *getTextBuffer()
{
	FILE *fileStream;
	errno_t errorNo = fopen_s(&fileStream, "lorem.txt", "rb");
	if (errorNo != 0)
	{
		throw std::runtime_error("Unable to open file");
	}
	if (std::fseek(fileStream, 0, SEEK_END) != 0)
	{
		throw std::runtime_error("Unable to read file");
	}
	const long length = std::ftell(fileStream);
	if (std::fseek(fileStream, 0, SEEK_SET) != 0)
	{
		throw std::runtime_error("Unable to read file");
	}
	char *buffer                = (char *)std::malloc(length + 1);
	buffer[length]              = '\0';
	const std::size_t readCount = std::fread(buffer, sizeof *buffer, length, fileStream);
	if (readCount != length)
	{
		throw std::runtime_error("Unable to read file");
	}
	std::fclose(fileStream);
	return buffer;
}

int main(int argc, char *argv[])
{
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	std::cout << "Testing TrimRight..." << std::endl;
	for (int i = 0; i < 10; ++i)
	{
		char *buffer         = getTextBuffer();
		const auto startTime = high_resolution_clock::now();
		TrimRight(buffer);
		const auto endTime                         = high_resolution_clock::now();
		const duration<double, std::milli> elapsed = endTime - startTime;
		std::cout << elapsed.count() << "ms" << std::endl;
		std::free(buffer);
	}
	std::cout << "======================================" << std::endl;

	std::cout << "Testing TrimRightMultiThreaded..." << std::endl;
	for (int i = 0; i < 10; ++i)
	{
		char *buffer         = getTextBuffer();
		const auto startTime = high_resolution_clock::now();
		TrimRightMultiThreaded(buffer);
		const auto endTime                         = high_resolution_clock::now();
		const duration<double, std::milli> elapsed = endTime - startTime;
		std::cout << elapsed.count() << "ms" << std::endl;
		std::free(buffer);
	}
	std::cout << "======================================" << std::endl;

	return 0;
}
