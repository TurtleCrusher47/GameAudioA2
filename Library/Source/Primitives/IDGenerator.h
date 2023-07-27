/**
 CIDGenerator
 By: Toh Da Jun
 Date: Jan 2023
 */
#pragma once

 // Include SingletonTemplate
#include "../DesignPatterns/SingletonTemplate.h"

class CIDGenerator : public CSingletonTemplate<CIDGenerator>
{
	friend CSingletonTemplate<CIDGenerator>;
public:
	// Constructor(s)
    CIDGenerator(void);

	// Destructor
	virtual ~CIDGenerator(void);

	// Init
	virtual bool Init(void);

	// Generate an ID
	virtual unsigned int GenerateID(void);

protected:
	unsigned int m_iCurrentID;
};
