//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Classes/Utils.h"

//==============================================================================

/**
 * @brief ����� ��������� ��������� ������ ���� ���� (������� LIFO).
 * ��������� ������ ��� ��������� ����������� ����������� � ���� ����������
 * ��� �������� ������� (��. ���������� ��������� ��-��������� � ������������).
 * 
 * @tparam T - ��� �������� (item'�).
 */
template <typename T>
class ClassStackStatic
{
protected:
	uint8_t nItemsMax;	// ������������ ���������� ��������� � �����.
	uint8_t iEnd;		// �� ����� ������� ���������� ����� ��������.
	T * pData;			// ��������� �� ��������� (�� ������ ���������).
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Construct a new Class Stack Static object
	 * 
	 * @param ArgNItemsMax - ����. ���������� ��������� � �����. ��-��������� - 5.
	 */
	ClassStackStatic( uint8_t ArgNItemsMax = 5 )
	{
		ASSERT_CUSTOM( 0, ArgNItemsMax != 0 );
		nItemsMax = ArgNItemsMax;
		pData = utils::AssertPtr(0, new T[ArgNItemsMax]);

		Clear();
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief �������� ������� �� ������� ����� (� ����� �������).
	 * ��� ������������ ���������� ASSERT_CUSTOM.
	 * ����������� �� ��������� � ������� ������ ����������� ����� ��������
	 * "�����" (����������).
	 *
	 * @param ArgItem - �������� ������ ��� Item'�.
	 */
	void AddToBack(const T & ArgItem)
	{
		if (iEnd >= nItemsMax)
		{ // ��� ����� ��� ���������� ������ ��������, ������� ������������ �����! �� ��������� �������!
			ASSERT_CUSTOM(0, false);
		}
		else
		{
			pData[iEnd] = ArgItem;
			iEnd++;
		}
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ���������� !!! ��� �������� !!! ��������� (����� �����) ������� �����.
	 * ��������, ��� ���� ����, ����������� ASSERT_CUSTOM.
	 * 
	 * @return \p const T & - ������ �� �������.
	 */
	const T & ReadLast(void)
	{
		if (iEnd == 0)
		{ 	// ����������� ������! ���� ����, ��� ���������!!!
			ASSERT_CUSTOM(0, false);
			return pData[0]; // � ������ ������ ��������� ������� ����-������ �������� ���������!
		}

		return pData[iEnd-1];
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief �������� ���������� ��������, ���� �� ����.
	 * ���������� �������� ������ ������, ��� ��������� ������.
	 */
	// TODO: ����������� ������������?
	void DeleteLast(void)
	{
		if (iEnd != 0)
		{
			iEnd--;
		}
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ������� ����� � ��������� ���� ���������. ���� ���� "������".
	 * �������� ������� � ���������, ����� ���� ����� ������������ �
	 * ������� �����.
	 */
	void Clear(void)
	{
		memset(pData, 0, nItemsMax * sizeof(T));

		iEnd = 0;
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ���������� ���������� ��������� � �����.
	 * 
	 * @return ���������� ��������� � �����.
	 */
	uint8_t GetCount(void)
	{
		return iEnd;
	}
	//--------------------------------------------------------------------------
};
//******************************************************************************