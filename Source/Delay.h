#include <iostream>
#include <vector>
#include <math.h>

#pragma once

class DelayUnit
{
	std::vector<float> m_buffer;
	int m_bufferSize;
	int m_writePos;
	
	public:
		DelayUnit(int delaySize) :	
			m_bufferSize(delaySize),
			m_writePos(0)
		{	
			for(int n = 0 ; n < m_bufferSize ; ++n)
			{
				m_buffer.push_back(0);
			}
			std::cout << "Delay Constructed Size: " << m_bufferSize << std::endl;
			// Don't call this in the audio,
			// printing audio stuff in actual use is silly
		}
	void printBuffer()
	{
		for(auto sample : m_buffer)
		{
			std::cout << sample<<std::endl;
		}
	}

	void clearBuffer()
	{
		for(int n = 0 ; n < m_bufferSize ; ++n)
		{
			m_buffer[n] = 0;
		}
	}

	void sum(float value)
	{
		m_buffer[m_writePos] += value;		
		++m_writePos;
		while(m_writePos >= m_bufferSize)m_writePos -= m_bufferSize;		
	}

	void replace(float value)
	{
		m_buffer[m_writePos] = value;
		++m_writePos;
		while(m_writePos >= m_bufferSize)m_writePos -= m_bufferSize;
	}
	
	float getSample( int delayTime )
	{

		int delayPos = m_writePos - delayTime;

		while(delayPos < 0)delayPos += m_bufferSize;
		while(delayPos >= m_bufferSize)delayPos -= m_bufferSize;

		return m_buffer[delayPos];
	}

	float getSample(double delayTime)
	{
		float delayPos = m_writePos - delayTime;
		
		double prevPos;
		float fraction = modf(delayPos, &prevPos);
		
		double nextPos = prevPos + 1;

		while(prevPos < 0)prevPos += m_bufferSize;
		while(prevPos >= m_bufferSize)prevPos -= m_bufferSize;

		while(nextPos < 0)prevPos += m_bufferSize;
		while(nextPos >= m_bufferSize)nextPos -= m_bufferSize;

		float prevVal = m_buffer[prevPos];
		float nextVal = m_buffer[nextPos];

		return ((1-fraction)*prevVal) + (nextVal * fraction);

	}
	
};
