//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------

#include "Classes/Utils.h"
//------------------------------------------------------------------------------
#include "Classes/AppGlobals.h"

#include "Src/AssertCustom.h"

//------------------------------------------------------------------------------
#include <stdint.h>
// ��� memcpy
#include <string.h>

//******************************************************************************
/**
 * @brief ��������� ������� ������ � �������� (��������� ���� struct TagItem)
 * ��� "�������� �������" � ���� ������ � RAM.
 * ����� �������� ����������� ������ � �����, �� ��������� ��������� ��������.
 * ��� ������������ �������� ������������� ��������, � ������������ ���������
 * � �������� ��������� ���������� ���������� �������� �� 1.
 * @param \p ARG_ITEMS_AMOUNT - ���������� ��������� ����������� ������� �
 * �������.
 * @param \p ARG_ITEM_TEXT_SIZE - ������ ���������� ����� ������ �������
 * ��������.
 */
template <uint8_t ARG_ITEMS_AMOUNT, uint8_t ARG_ITEM_TEXT_SIZE>
class ClassNpStorage
{
protected:
	//----------------------------------------------------------------------
	/**
	 * @brief ��������� ������ �������� ������ �������� � �������.
	 * @param ARG_ITEM_TEXT_SIZE - ������ ���������� ������.
	 */
	template <uint8_t ARG_TEXT_SIZE>
	struct TagItem
	{
		uint8_t SortingIdx; // ������ ��� �������������� ��������� � ������������ �������� ��� �����, �������� 255 �������� ��� ������� �� �������� ������� (�����).
		uint8_t Text[ARG_TEXT_SIZE];
	};
	//----------------------------------------------------------------------

protected:
	TagItem<ARG_ITEM_TEXT_SIZE>    items[ARG_ITEMS_AMOUNT]; ///< �������� ���������.

	uint8_t numOfItems;									  ///< ������� ���������� ����������� ��������� � ���������
	static const uint8_t SORTING_IDX_DELETED_VALUE = 255; ///< �������, ��� ������� �� �������� (�����).

	//--------------------------------------------------------------------------

public:
	/**
	 * @brief Construct a new object
	 */
	ClassNpStorage()
	{
		numOfItems = 0;

		// �������� ��� �������� ��� �������� (���������).
		for (size_t i = 0; i < ARG_ITEMS_AMOUNT; i++)
		{
			items[i].SortingIdx = SORTING_IDX_DELETED_VALUE;
		}
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ���������� ������� ���������� ����������� ���������.
	 * 
	 * @return ������� ���������� ����������� ���������.
	 */
	uint8_t Get_numOfItems(void)
	{
		return numOfItems;
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ������� ������� �� �������. � ��������� � �������� ������ �������
	 *  ���������� ��������, ��� �� �������, ������� ����� ��������� �� 1.
	 * ������������ ������� ����������� ASSERT_CUSTOM.
	 * (�.�. �������� �������� ������ ����������� ������� ����).
	 * 
	 * @param \p ArgSortingIdx - ������ ��������.
	 */
	void DeleteItem(uint8_t ArgSortingIdx)
	{
		if ((ArgSortingIdx >= utils::size(items)) || (numOfItems == 0) || (ArgSortingIdx >= numOfItems))
		{
			// ������: ����� ������� �� �������, ��� ��� �� ������ ������������ ��������, ��� ��� �������� � ����� �������� (�����, �� ������).
			ASSERT_CUSTOM(0, false);
			return;
		}

		// � ��������� � �������� ������ ������� ����������
		// ��������, ��� �� �������, �������� ������� �� 1.
		// � ������ ����� �������� ������� ��� ��������.

		for (uint8_t i = 0; i < utils::size(items); i++)
		{
			if (items[i].SortingIdx == this->SORTING_IDX_DELETED_VALUE)
			{
				;
			}
			else if (items[i].SortingIdx > ArgSortingIdx)
			{
				items[i].SortingIdx--;
			}
			else if (items[i].SortingIdx == ArgSortingIdx)
			{
				items[i].SortingIdx = this->SORTING_IDX_DELETED_VALUE;
			}
		}

		numOfItems--;
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ������� ��� �������� (�������� ���������).
	 */
	void DeleteAllItems(void)
	{
		for (uint8_t i = 0; i < utils::size(items); i++)
		{
			items[i].SortingIdx = this->SORTING_IDX_DELETED_VALUE;
		}
		numOfItems = 0;
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ��������������� ���� ������ ��������� ������� � ��������� ��� �
	 * �������������� ������ �� \p ArgTxt.
	 * ���� �� ������� ��������� �������, �� ������ ��� ��������� ���������
	 * � ������ ��������� (���������� ���������������� �����):
	 * 1. ������� ������ ������� � ��������� ������ ��������� ��������� �� 1;
	 * 2. ��������� ����� ������� � �����.
	 * 
	 * @param \p ArgTxt - [in] ��������� �� ������� ��������� �����.
	 */
	void AddItemToBack(const char * ArgTxt)
	{
		if (numOfItems == utils::size(items))
		{ // ���� ��� ��������� ���������
			DeleteItem( 0 );
		}
		// ����� ������� ���������� �������� � ���������� ���
		for ( uint8_t idx = 0; idx < utils::size(items); idx++)
		{
			if (items[idx].SortingIdx == this->SORTING_IDX_DELETED_VALUE)
			{
				memcpy( items[idx].Text, ArgTxt, ARG_ITEM_TEXT_SIZE);
				items[idx].SortingIdx = numOfItems;
				numOfItems++;
				break;
			}
		}
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ��������� ����� �� �������� � ����� \p ArgTxt, ������ ��������
	 * ������ ���� �� ����� \p ARG_ITEM_TEXT_SIZE ����.
	 * ������������ ������� ����������� ASSERT_CUSTOM, ������� �� �������������
	 * ������� ������ ������� ������� ����.
	 *
	 * @param \p ArgSortingIdx - ������ �������� � 0.
	 * @param \p ArgTxt - [out] ��������� �� �����, � ������� ����� ���������� �����.
	 */
	void GetItemText(uint8_t ArgSortingIdx, char * ArgTxt)
	{
		if ((ArgSortingIdx >= utils::size(items)) || (numOfItems == 0) || (ArgSortingIdx >= numOfItems))
		{
			// ������: ����� ������� �� �������, ��� ��� �� ������ ������������ ��������, ��� ��� �������� � ����� �������� (�����, �� ������).
			ASSERT_CUSTOM(0, false);
			return;
		}
		//-----------------------------
		// ���������������� ����� �������� � ������ ��������
		for (uint8_t idx = 0; idx < utils::size(items); idx++)
		{
			if (items[idx].SortingIdx == ArgSortingIdx)
			{ // ���� ������ ������� � ������ ��������:
				memcpy(ArgTxt, items[idx].Text, ARG_ITEM_TEXT_SIZE);
				break;
			}
		}
	}
	//--------------------------------------------------------------------------
};

//******************************************************************************
