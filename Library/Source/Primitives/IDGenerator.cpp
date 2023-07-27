/**
 CIDGenerator
 By: Toh Da Jun
 Date: Jan 2023
 */
#include "IDGenerator.h"

#include <iostream>
using namespace std;

/**
@brief Default Constructor
*/
CIDGenerator::CIDGenerator()
	: m_iCurrentID(1)
{
}

/**
@brief Default Destructor
*/
CIDGenerator::~CIDGenerator(void)
{
}

/**
@brief Initialise this instance to default value of 1
*/
bool CIDGenerator::Init(void)
{
	m_iCurrentID = 1;
	return true;
}

/**
@brief Generate an ID
*/
unsigned int CIDGenerator::GenerateID(void)
{
	return m_iCurrentID++;
}
