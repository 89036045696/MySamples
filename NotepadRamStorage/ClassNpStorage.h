//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------

#include "Classes/Utils.h"
//------------------------------------------------------------------------------
#include "Classes/AppGlobals.h"

#include "Src/AssertCustom.h"

//------------------------------------------------------------------------------
#include <stdint.h>
// для memcpy
#include <string.h>

//******************************************************************************
/**
 * @brief Хранилище массива блоков с текстами (элементов типа struct TagItem)
 * для "блокнота прибора" в пуле памяти в RAM.
 * Новые элементы добавляются только в конец, со следующим незанятым индексом.
 * При произвольном удалении существующего элемента, у существующих элементов
 * с бОльшими индексами происходит уменьшение индексов на 1.
 * @param \p ARG_ITEMS_AMOUNT - количество элементов одинакового размера в
 * массиве.
 * @param \p ARG_ITEM_TEXT_SIZE - размер текстового блока внутри каждого
 * элемента.
 */
template <uint8_t ARG_ITEMS_AMOUNT, uint8_t ARG_ITEM_TEXT_SIZE>
class ClassNpStorage
{
protected:
	//----------------------------------------------------------------------
	/**
	 * @brief Описывает формат хранения одного элемента с текстом.
	 * @param ARG_ITEM_TEXT_SIZE - размер текстового буфера.
	 */
	template <uint8_t ARG_TEXT_SIZE>
	struct TagItem
	{
		uint8_t SortingIdx; // Индекс для упорядочивания элементов и отслеживания признака что удалён, значение 255 означает что элемент не заполнен данными (удалён).
		uint8_t Text[ARG_TEXT_SIZE];
	};
	//----------------------------------------------------------------------

protected:
	TagItem<ARG_ITEM_TEXT_SIZE>    items[ARG_ITEMS_AMOUNT]; ///< Элементы хранилища.

	uint8_t numOfItems;									  ///< Текущее количество заполненных элементов в хранилище
	static const uint8_t SORTING_IDX_DELETED_VALUE = 255; ///< Признак, что элемент не заполнен (удалён).

	//--------------------------------------------------------------------------

public:
	/**
	 * @brief Construct a new object
	 */
	ClassNpStorage()
	{
		numOfItems = 0;

		// Помечаем все элементы как удалённые (свободные).
		for (size_t i = 0; i < ARG_ITEMS_AMOUNT; i++)
		{
			items[i].SortingIdx = SORTING_IDX_DELETED_VALUE;
		}
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Возвращает текущее количество заполненных элементов.
	 * 
	 * @return текущее количество заполненных элементов.
	 */
	uint8_t Get_numOfItems(void)
	{
		return numOfItems;
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Удалить элемент по индексу. У элементов с индексом больше индекса
	 *  удаляемого элемента, при их наличии, индексы будут уменьшены на 1.
	 * Правильность индекса проверяется ASSERT_CUSTOM.
	 * (т.е. проверка индексов должна выполняться уровнем выше).
	 * 
	 * @param \p ArgSortingIdx - индекс элемента.
	 */
	void DeleteItem(uint8_t ArgSortingIdx)
	{
		if ((ArgSortingIdx >= utils::size(items)) || (numOfItems == 0) || (ArgSortingIdx >= numOfItems))
		{
			// Ошибка: выход индекса за границу, или нет ни одного заполненного элемента, или нет элемента с таким индексом (удалён, не создан).
			ASSERT_CUSTOM(0, false);
			return;
		}

		// У элементов с индексом больше индекса удаляемого
		// элемента, при их наличии, уменьшим индексы на 1.
		// А индекс этого элемента пометим как удалённый.

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
	 * @brief Удалить все элементы (пометить удалёнными).
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
	 * @brief Последовательно ищет первый свободный элемент и заполняет его с
	 * использованием текста из \p ArgTxt.
	 * Если не находит свободный элемент, то значит что хранилище заполнено
	 * и делает следующее (циклически перезаписываемый буфер):
	 * 1. удаляет первый элемент и уменьшает номера остальных элементов на 1;
	 * 2. добавляет новый элемент в конец.
	 * 
	 * @param \p ArgTxt - [in] указатель на внешний текстовый буфер.
	 */
	void AddItemToBack(const char * ArgTxt)
	{
		if (numOfItems == utils::size(items))
		{ // если нет свободных элементов
			DeleteItem( 0 );
		}
		// поиск первого свободного элемента и заполнение его
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
	 * @brief Считывает текст из элемента в буфер \p ArgTxt, размер которого
	 * должен быть не менее \p ARG_ITEM_TEXT_SIZE байт.
	 * Правильность индекса проверяется ASSERT_CUSTOM, поэтому за правильностью
	 * индекса должен следить уровень выше.
	 *
	 * @param \p ArgSortingIdx - индекс элемента с 0.
	 * @param \p ArgTxt - [out] указатель на буфер, в который будет скопирован текст.
	 */
	void GetItemText(uint8_t ArgSortingIdx, char * ArgTxt)
	{
		if ((ArgSortingIdx >= utils::size(items)) || (numOfItems == 0) || (ArgSortingIdx >= numOfItems))
		{
			// Ошибка: выход индекса за границу, или нет ни одного заполненного элемента, или нет элемента с таким индексом (удалён, не создан).
			ASSERT_CUSTOM(0, false);
			return;
		}
		//-----------------------------
		// последовательный поиск элемента с нужным индексом
		for (uint8_t idx = 0; idx < utils::size(items); idx++)
		{
			if (items[idx].SortingIdx == ArgSortingIdx)
			{ // если найден элемент с нужным индексом:
				memcpy(ArgTxt, items[idx].Text, ARG_ITEM_TEXT_SIZE);
				break;
			}
		}
	}
	//--------------------------------------------------------------------------
};

//******************************************************************************
