//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include <cstdio>
#include <cstring>

#include "ClassNpStorage.h"
#include "Classes/AppGlobals.h"
#include "Src/AssertCustom.h"
#include "Classes/Utils.h"
#include "Classes/Texts.h"	// для строки с названием системной папки

//******************************************************************************
/**
 * @brief Функционал блокнота с хранением текстовых имён папок и текстовых
 * записей.
 * Максимальное количество папок равно NOTEPAD__FOLDERS_AMOUNT,
 * максимальное количество текстовых
 * записей в каждой папке равно NOTEPAD__FOLDER_SIZE.
 * Размер каждой строки для хранения имени папки (включая терминирующий '\0')
 * равен NOTEPAD_FOLDER_NAME_TEXT_SIZE,
 * размер каждой строки для хранения текстовой записи (включая терминирующий
 * '\0') равен NOTEPAD__RECORD_TEXT_SIZE.
 * Имеет текстовые буферы для работы с хранилищами.
 * При создании автоматически создаётся системная папка (индекс 0).
 * Системную папку (с индексом 0) нельзя удалить/удалять.
 * Соответствие индексов "индекс имени папки" и "индекс хранилища записей для
 * папки" устанавливается через внутреннее поле класса - массив.
 */
class ClassNotepad
{
public:

	/// @brief Буфер для чтения/записи текста имени папки блокнота.
	char BufTxtFolderName[NOTEPAD_FOLDER_NAME_TEXT_SIZE];

	/// @brief Буфер для чтения/записи текста записи из папки блокнота.
	char BufTxtRecord[NOTEPAD__RECORD_TEXT_SIZE];

	/**
	 * @brief Вызывается, когда были сделаны изменения в составе папок.
	 * 
	 * @param \p ArgCaller - указатель на объект Notepad, который вызвал
	 * Call Back.
	 */
	void (*PtrCallbackOnFoldersChanged)(ClassNotepad * ArgCaller) = nullptr;


	/**
	 * @brief Вызывается, когда в папке были сделаны изменения в составе записей.
	 * 
	 * @param \p ArgCaller - указатель на объект Notepad, который вызвал
	 * Call Back.
	 */
	void (*PtrCallbackOnRecordsChangedInFolder)(ClassNotepad * ArgCaller) = nullptr;

	//--------------------------------------------------------------------------
private:
	/**
	 * @brief Хранилище с текстовыми именами папок.
	 */
	ClassNpStorage< NOTEPAD__FOLDERS_AMOUNT, NOTEPAD_FOLDER_NAME_TEXT_SIZE >    storageOfFoldersNames;

	/**
	 * @brief Массив хранилищ записей для папок. Каждый элемент массива - хранилище
	 * с записями для папки (содержимое папки).
	 */
	ClassNpStorage< NOTEPAD__FOLDER_SIZE, NOTEPAD__RECORD_TEXT_SIZE >           storagesOfRecords[NOTEPAD__FOLDERS_AMOUNT];
	/**
	 * @brief Каждый элемент массива = сумме strlen для всех записей в папке
	 * (strlen считает не включая терминирующий '\0'). Обращение - как к хранилищу записей, через
	 * карту соответствия.
	 */
	uint32_t summmasOfRecordsLengths[NOTEPAD__FOLDERS_AMOUNT];

	/**
	 * @brief Карта (таблица) для сопоставления <индекса существующего элемента из хранилища с текстовыми
	 * именами папки ("индекс существующей папки") с <индексом из массива хранилищ записей папок>
	 * ("индекс хранилища").
	 * Индекс массива соответствует "индексу существующей папки", а содержимое - "индексу хранилища".
	 * В этом массиве не отслеживается, существует ли папка. Это отслеживается по
	 * хранилищу с именами папок.
	 */
	uint8_t map[NOTEPAD__FOLDERS_AMOUNT];

	//--------------------------------------------------------------------------
public:
	ClassNotepad()
	{
		// Создать системную папку с фиксированным именем.
		std::snprintf(BufTxtFolderName, NOTEPAD_FOLDER_NAME_TEXT_SIZE, "%s", Strs_FolderNameSystem[LanguageId]);
		CreateFolderByBuffer();

		BufTxtFolderName[0] = '\0';
		BufTxtRecord[0] = '\0';
	}
	//--------------------------------------------------------------------------
private:
	/* Вызов колбэк-функции.
	 * 
	 * ArgPtrCallback - указатель на колбэк-функцию.
	 */
	void callCallback(void (*ArgPtrCallback)(ClassNotepad * ArgCaller))
	{
		if( ArgPtrCallback != nullptr ) { (*ArgPtrCallback)(this); }
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Возвращает текущее количество созданных папок.
	 * 
	 * @return текущее количество созданных папок.
	 */
	uint8_t GetNumOfFolders(void)
	{
		return storageOfFoldersNames.Get_numOfItems();
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Возвращает текущее количество заполненных записей в хранилище папки
	 * по индексу папки.
	 * При неправильном \p ArgFolderId будет вызван ASSERT_CUSTOM, поэтому
	 * правильность индекса должна быть обеспечена уровнем выше.
	 * 
	 * @param \p ArgFolderId - индекс папки.
	 * @return текущее количество заполненных записей в хранилище папки.
	 */
	uint8_t GetNumOfRecords(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= storageOfFoldersNames.Get_numOfItems()) ||
		     (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT)
		   )
		{
			// Ошибка! Нет созданной папки с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return 0;
		}

		return storagesOfRecords[map[ArgFolderId]].Get_numOfItems();
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Выполняет команду интерфейса пользователя "Удалить все папки".
	 * По окончании вызывает PtrCallbackOnFoldersChanged.
	 * Технически, помечает удалёнными содержимое хранилища имён всех
	 * пользовательских папок (кроме системной папки с индексом 0).
	 */
	void DeleteAllUserFolders(void)
	{
		// удаление с конца, так немного эффективнее
		for (auto fi = storageOfFoldersNames.Get_numOfItems() - 1; fi > 0; fi--)
		{
			storageOfFoldersNames.DeleteItem(fi);
			summmasOfRecordsLengths[map[fi]] = 0;
			storagesOfRecords[map[fi]].DeleteAllItems();
		}
		callCallback(PtrCallbackOnFoldersChanged);
	}

	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Выполняет команду интерфейса пользователя "Удалить папку".
	 * По окончании вызывает коллбэк PtrCallbackOnFoldersChanged.
	 * Технически, помечает удалёнными содержимое имени папки и записи
	 * в хранилище записей. Удаляет связь в карте. Уменьшает индексы следующих
	 * папок (при наличии) на 1.
	 * При неправильном \p ArgFolderId будет вызван ASSERT_CUSTOM, поэтому
	 * правильность индекса должна быть обеспечена уровнем выше.
	 * 
	 * @param \p ArgFolderId - индекс пользовательской папки, кроме системной
	 * папки (с индексом 0).
	 */
	void DeleteUserFolder(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= storageOfFoldersNames.Get_numOfItems()) ||
		     (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT)
		   )
		{
			// Критическая ошибка! Нет созданной папки с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		if (ArgFolderId == 0)
		{
			// Критическая ошибка! Нельзя удалять системную папку (с индексом 0) !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		
		//-------------------------
		storagesOfRecords[map[ArgFolderId]].DeleteAllItems();
		summmasOfRecordsLengths[map[ArgFolderId]] = 0;
		storageOfFoldersNames.DeleteItem(ArgFolderId);
		// при удалении элемента в хранилище папки, при наличии следующих элементов,
		// могли сдвинуться индексы последующих элементов и тогда потребуется
		// сдвинуть привязки в карте. Проверим:
		auto n = storageOfFoldersNames.Get_numOfItems(); // новое количество элементов в хранилище папок.
		// Старый максимальный индекс папки теперь равен числу элементов в хранилище папок.
		if (n <= ArgFolderId)
		{ // если нет папок с индексами больше чем у удалённой:
			; // сдвига индексов не произошло
		}
		else
		{
			// сдвинуть привязки индексов в карте:
			for (auto fi = ArgFolderId; fi < n; fi++)
			{
				map[fi] = map[fi + 1];
			}
		}
		callCallback(PtrCallbackOnFoldersChanged);
	}
	//--------------------------------------------------------------------------
private:
	/* Поиск по карте свободного хранилища записей (его индекса):
	 * проверяем возможные индексы хранилищ записей (rsi) от 0 в карте по индексам
	 * папок (fi) от 0 до max: ищем rsi != map[fi=0..max]
	 * 
	 * ArgFolderIdMax - максимальный индекс папки.
	 */
	uint8_t findEmptyRecordsStorageId(uint8_t ArgFolderIdMax)
	{
		bool isOccupied;
		for (uint8_t rsi = 0; rsi < NOTEPAD__FOLDERS_AMOUNT; rsi++)
		{
			isOccupied = false;
			for ( uint8_t fi = 0; fi <= ArgFolderIdMax; fi++ )
			{
				isOccupied = isOccupied || (rsi == map[fi]);
			}
			if ( isOccupied == false )
			{
				return rsi;
			}
		}
		ASSERT_CUSTOM(0, false); // Критическая ошибка! все хранилища заняты!
		return 0; // default при критической ошибке
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Создать папку.
	 * По окончании вызывает коллбэк PtrCallbackOnFoldersChanged.
	 * Папке будет присвоено имя, помещённое во внутреннем текстовом буфере
	 * \p BufTxtFolderName[NOTEPAD_FOLDER_NAME_TEXT_SIZE].
	 * Папке будет присвоен индекс, следующий за текущим максимальным.
	 * Максимально будет скопировано \p NOTEPAD_FOLDER_NAME_TEXT_SIZE байтов.
	 * Максимально возможное количество папок \p NOTEPAD__FOLDERS_AMOUNT.
	 * При отсутствии места для новой папки (если текущее
	 * количество > NOTEPAD__FOLDERS_AMOUNT), будет вызван ASSERT_CUSTOM и
	 * папка не будет создана, поэтому проверка  должна быть обеспечена
	 * уровнем выше.
	 */
	void CreateFolderByBuffer(void)
	{
		if( storageOfFoldersNames.Get_numOfItems() == NOTEPAD__FOLDERS_AMOUNT )
		{
			// Ошибка! Нет места для новой папки!
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//------------------------------
		storageOfFoldersNames.AddItemToBack(BufTxtFolderName);
		uint8_t idxFolder = storageOfFoldersNames.Get_numOfItems() - 1;
		// Сопоставляем папке хранилище записей:
		// Если индекс папки == 0, то считаем все хранилища пустыми и занимаем 0-е.
		// Иначе - делаем поиск свободного хранилища записей по карте (индекса
		// свободного хранилища записей).
		uint8_t idxStorage = 0;
		if ( idxFolder != 0 )
		{
			idxStorage = findEmptyRecordsStorageId(idxFolder - 1);
		}
		// Занесение номера хранилища в карту:
		map[idxFolder] = idxStorage;
		summmasOfRecordsLengths[map[idxFolder]] = 0;

		callCallback(PtrCallbackOnFoldersChanged);
	}
	//--------------------------------------------------------------------------
private:
	/* Суммирует strlen строк в хранилище записей.
	 * ArgIdOfRecordsStorage - индекс хранилища записей
	 * Возвращает сумму strlen от записей.
	 */
	uint32_t calcSumOfLengthsInStorage(uint8_t ArgIdOfRecordsStorage)
	{
		uint32_t ret = 0; // в случае ошибки возвратим 0
		//------------------------
		if ( ArgIdOfRecordsStorage >= NOTEPAD__FOLDERS_AMOUNT )
		{
			// Ошибка! Нет хранилища записей в массиве хранилищ записей с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return ret;
		}
		//------------------------
		for(uint8_t i = 0, n = storagesOfRecords[ArgIdOfRecordsStorage].Get_numOfItems(); i < n; i++)
		{
			storagesOfRecords[ArgIdOfRecordsStorage].GetItemText(i, BufTxtRecord);
			ret += std::strlen(BufTxtRecord);
		}
		return ret;
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Создать текстовую запись в папке с индексом \p ArgFolderId.
	 * По окончании будет вызван колбэк PtrCallbackOnRecordsChangedInFolder.
	 * \n Записи будет присвоен индекс, следующий за текущим максимальным.
	 * В запись будет скопирован текст из внутреннего текстового буфера
	 * \p BufTxtRecord[NOTEPAD__RECORD_TEXT_SIZE].
	 * Максимально из буфера будет скопировано \p NOTEPAD__RECORD_TEXT_SIZE байтов.
	 * При исчерпании места в хранилище записей, предварительно будет удалена самая
	 * первая запись (с индексом 0).
	 * @param \p ArgFolderId - [in] индекс папки. При неправильном \p ArgFolderId
	 * будет вызван ASSERT_CUSTOM, поэтому
	 * правильность индекса должна быть обеспечена уровнем выше.
	 */
	void CreateRecordByBuffer(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= storageOfFoldersNames.Get_numOfItems()) ||
		     (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT)
		   )
		{
			// Ошибка! Нет созданной папки с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//----------------------------------------
		// Определим, как вычислить summmasOfRecordsLengths[] после манипуляций с хранилищем записей:
		auto n = storagesOfRecords[map[ArgFolderId]].Get_numOfItems(); // количество записей до изменения
		bool choice = n == NOTEPAD__FOLDER_SIZE; /* true - значит, внутри storagesOfRecords будет 
												  * удаляться элемент с индексом 0 и новые данные
												  * добавляться по индексу [NOTEPAD__FOLDER_SIZE - 1]
												  * поэтому нужно пересчитывать заново;
												  * false - значит, будет добавлен элемент по индексу [n]
												  * поэтому просто прибавим размер этого элемента к сумме.
												  */
		storagesOfRecords[map[ArgFolderId]].AddItemToBack(BufTxtRecord);
		if (choice)
		{
			summmasOfRecordsLengths[map[ArgFolderId]] = calcSumOfLengthsInStorage(map[ArgFolderId]);
		}
		else
		{
			summmasOfRecordsLengths[map[ArgFolderId]] += std::strlen(BufTxtRecord);
		}
		//-----------------------------------
		if (summmasOfRecordsLengths[map[ArgFolderId]] >= NOTEPAD__FOLDER_SIZE * NOTEPAD__RECORD_TEXT_SIZE )
		{
			// Критическая ошибка! Сумма длин записей превысила максимум!
			ASSERT_CUSTOM( 0, false );
			return;
		}
		callCallback(PtrCallbackOnRecordsChangedInFolder);
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Удаление записи в папке по индексам.
	 * По окончании будет вызван колбэк PtrCallbackOnRecordsChangedInFolder.
	 * Правильность индексов проверяется ASSERT_CUSTOM.
	 * (т.е. проверка индексов должна выполняться уровнем выше).
	 * 
	 * @param \p ArgFolderId - индекс папки.
	 * @param \p ArgRecordId - индекс записи в папке.
	 */
	void DeleteRecord(uint8_t ArgFolderId, uint8_t ArgRecordId)
	{
		if ( (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT) ||
			 (ArgFolderId >= storageOfFoldersNames.Get_numOfItems())
		   )
		{
			// Ошибка! Нет созданной папки с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		if (ArgFolderId >= storagesOfRecords[map[ArgFolderId]].Get_numOfItems())
		{
			// Ошибка! Нет созданной записи с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//--------------------------------
		// Сначала предварительно уменьшим сумму
		storagesOfRecords[map[ArgFolderId]].GetItemText(ArgRecordId, BufTxtRecord);
		summmasOfRecordsLengths[map[ArgFolderId]] -= std::strlen(BufTxtRecord);

		storagesOfRecords[map[ArgFolderId]].DeleteItem(ArgRecordId);

		callCallback(PtrCallbackOnRecordsChangedInFolder);
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Удаление всех записей в папке.
	 * По окончании будет вызван колбэк PtrCallbackOnRecordsChangedInFolder.
	 * Правильность индекса проверяется ASSERT_CUSTOM.
	 * (т.е. проверка индекса должна выполняться уровнем выше).
	 * 
	 * @param \p ArgFolderId - индекс папки.
	 */
	void DeleteAllRecordsInFolder(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT) ||
			 (ArgFolderId >= storageOfFoldersNames.Get_numOfItems())
		   )
		{
			// Ошибка! Нет созданной папки с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//---------------------------
		summmasOfRecordsLengths[map[ArgFolderId]] = 0;

		storagesOfRecords[map[ArgFolderId]].DeleteAllItems();

		callCallback(PtrCallbackOnRecordsChangedInFolder);
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Считывает текст с именеи папки во внутренний буфер имени папки
	 * \p BufTxtFolderName[NOTEPAD_FOLDER_NAME_TEXT_SIZE].
	 * При неправильном \p ArgFolderId будет вызван ASSERT_CUSTOM, поэтому
	 * правильность индекса должна быть обеспечена уровнем выше.
	 * 
	 * @param \p ArgFolderId - индекс папки.
	 */
	void ReadFolderNameToBuffer(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT) ||
		     (ArgFolderId >= storageOfFoldersNames.Get_numOfItems())
		   )
		{
			// Ошибка! Нет созданной папки с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//-------------------------
		storageOfFoldersNames.GetItemText(ArgFolderId, BufTxtFolderName);
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Чтание записи в папке по индексам во внутренний буфер BufTxtRecord
	 * \p BufTxtRecord[NOTEPAD__RECORD_TEXT_SIZE].
	 * Правильность индексов проверяется ASSERT_CUSTOM.
	 * (т.е. проверка индексов должна выполняться уровнем выше).
	 * 
	 * @param \p ArgFolderId - индекс папки.
	 * @param \p ArgRecordId - индекс записи в папке.
	 */
	void ReadRecordToBuffer(uint8_t ArgFolderId, uint8_t ArgRecordId)
	{
		if ( (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT) ||
		     (ArgFolderId >= storageOfFoldersNames.Get_numOfItems())
		   )
		{
			// Ошибка! Нет созданной папки с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		if (ArgRecordId >= storagesOfRecords[map[ArgFolderId]].Get_numOfItems())
		{
			// Ошибка! Нет созданной записи с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//--------------------------------
		storagesOfRecords[map[ArgFolderId]].GetItemText(ArgRecordId, BufTxtRecord);
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Возвращает сумму strlen для всех записей в папке
	 * (strlen считает не включая терминирующий '\0').
	 * 
	 * @param \p ArgFolderId - индекс папки, проверяется с помощью ASSERT_CUSTOM.
	 */
	uint32_t GetSumOfRecordsLengths(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT) ||
		     (ArgFolderId >= storageOfFoldersNames.Get_numOfItems())
		   )
		{
			// Ошибка! Нет созданной папки с таким индексом !
			ASSERT_CUSTOM( 0, false );
			return 0; // 0 в случае ошибки
		}
		//-------------------------
		return summmasOfRecordsLengths[map[ArgFolderId]];
	}
	//==========================================================================
};
//******************************************************************************
