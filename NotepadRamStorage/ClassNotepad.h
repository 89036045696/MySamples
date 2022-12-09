//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include <cstdio>
#include <cstring>

#include "ClassNpStorage.h"
#include "Classes/AppGlobals.h"
#include "Src/AssertCustom.h"
#include "Classes/Utils.h"
#include "Classes/Texts.h"	// ��� ������ � ��������� ��������� �����

//******************************************************************************
/**
 * @brief ���������� �������� � ��������� ��������� ��� ����� � ���������
 * �������.
 * ������������ ���������� ����� ����� NOTEPAD__FOLDERS_AMOUNT,
 * ������������ ���������� ���������
 * ������� � ������ ����� ����� NOTEPAD__FOLDER_SIZE.
 * ������ ������ ������ ��� �������� ����� ����� (������� ������������� '\0')
 * ����� NOTEPAD_FOLDER_NAME_TEXT_SIZE,
 * ������ ������ ������ ��� �������� ��������� ������ (������� �������������
 * '\0') ����� NOTEPAD__RECORD_TEXT_SIZE.
 * ����� ��������� ������ ��� ������ � �����������.
 * ��� �������� ������������� �������� ��������� ����� (������ 0).
 * ��������� ����� (� �������� 0) ������ �������/�������.
 * ������������ �������� "������ ����� �����" � "������ ��������� ������� ���
 * �����" ��������������� ����� ���������� ���� ������ - ������.
 */
class ClassNotepad
{
public:

	/// @brief ����� ��� ������/������ ������ ����� ����� ��������.
	char BufTxtFolderName[NOTEPAD_FOLDER_NAME_TEXT_SIZE];

	/// @brief ����� ��� ������/������ ������ ������ �� ����� ��������.
	char BufTxtRecord[NOTEPAD__RECORD_TEXT_SIZE];

	/**
	 * @brief ����������, ����� ���� ������� ��������� � ������� �����.
	 * 
	 * @param \p ArgCaller - ��������� �� ������ Notepad, ������� ������
	 * Call Back.
	 */
	void (*PtrCallbackOnFoldersChanged)(ClassNotepad * ArgCaller) = nullptr;


	/**
	 * @brief ����������, ����� � ����� ���� ������� ��������� � ������� �������.
	 * 
	 * @param \p ArgCaller - ��������� �� ������ Notepad, ������� ������
	 * Call Back.
	 */
	void (*PtrCallbackOnRecordsChangedInFolder)(ClassNotepad * ArgCaller) = nullptr;

	//--------------------------------------------------------------------------
private:
	/**
	 * @brief ��������� � ���������� ������� �����.
	 */
	ClassNpStorage< NOTEPAD__FOLDERS_AMOUNT, NOTEPAD_FOLDER_NAME_TEXT_SIZE >    storageOfFoldersNames;

	/**
	 * @brief ������ �������� ������� ��� �����. ������ ������� ������� - ���������
	 * � �������� ��� ����� (���������� �����).
	 */
	ClassNpStorage< NOTEPAD__FOLDER_SIZE, NOTEPAD__RECORD_TEXT_SIZE >           storagesOfRecords[NOTEPAD__FOLDERS_AMOUNT];
	/**
	 * @brief ������ ������� ������� = ����� strlen ��� ���� ������� � �����
	 * (strlen ������� �� ������� ������������� '\0'). ��������� - ��� � ��������� �������, �����
	 * ����� ������������.
	 */
	uint32_t summmasOfRecordsLengths[NOTEPAD__FOLDERS_AMOUNT];

	/**
	 * @brief ����� (�������) ��� ������������� <������� ������������� �������� �� ��������� � ����������
	 * ������� ����� ("������ ������������ �����") � <�������� �� ������� �������� ������� �����>
	 * ("������ ���������").
	 * ������ ������� ������������� "������� ������������ �����", � ���������� - "������� ���������".
	 * � ���� ������� �� �������������, ���������� �� �����. ��� ������������� ��
	 * ��������� � ������� �����.
	 */
	uint8_t map[NOTEPAD__FOLDERS_AMOUNT];

	//--------------------------------------------------------------------------
public:
	ClassNotepad()
	{
		// ������� ��������� ����� � ������������� ������.
		std::snprintf(BufTxtFolderName, NOTEPAD_FOLDER_NAME_TEXT_SIZE, "%s", Strs_FolderNameSystem[LanguageId]);
		CreateFolderByBuffer();

		BufTxtFolderName[0] = '\0';
		BufTxtRecord[0] = '\0';
	}
	//--------------------------------------------------------------------------
private:
	/* ����� ������-�������.
	 * 
	 * ArgPtrCallback - ��������� �� ������-�������.
	 */
	void callCallback(void (*ArgPtrCallback)(ClassNotepad * ArgCaller))
	{
		if( ArgPtrCallback != nullptr ) { (*ArgPtrCallback)(this); }
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ���������� ������� ���������� ��������� �����.
	 * 
	 * @return ������� ���������� ��������� �����.
	 */
	uint8_t GetNumOfFolders(void)
	{
		return storageOfFoldersNames.Get_numOfItems();
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ���������� ������� ���������� ����������� ������� � ��������� �����
	 * �� ������� �����.
	 * ��� ������������ \p ArgFolderId ����� ������ ASSERT_CUSTOM, �������
	 * ������������ ������� ������ ���� ���������� ������� ����.
	 * 
	 * @param \p ArgFolderId - ������ �����.
	 * @return ������� ���������� ����������� ������� � ��������� �����.
	 */
	uint8_t GetNumOfRecords(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= storageOfFoldersNames.Get_numOfItems()) ||
		     (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT)
		   )
		{
			// ������! ��� ��������� ����� � ����� �������� !
			ASSERT_CUSTOM( 0, false );
			return 0;
		}

		return storagesOfRecords[map[ArgFolderId]].Get_numOfItems();
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ��������� ������� ���������� ������������ "������� ��� �����".
	 * �� ��������� �������� PtrCallbackOnFoldersChanged.
	 * ����������, �������� ��������� ���������� ��������� ��� ����
	 * ���������������� ����� (����� ��������� ����� � �������� 0).
	 */
	void DeleteAllUserFolders(void)
	{
		// �������� � �����, ��� ������� �����������
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
	 * @brief ��������� ������� ���������� ������������ "������� �����".
	 * �� ��������� �������� ������� PtrCallbackOnFoldersChanged.
	 * ����������, �������� ��������� ���������� ����� ����� � ������
	 * � ��������� �������. ������� ����� � �����. ��������� ������� ���������
	 * ����� (��� �������) �� 1.
	 * ��� ������������ \p ArgFolderId ����� ������ ASSERT_CUSTOM, �������
	 * ������������ ������� ������ ���� ���������� ������� ����.
	 * 
	 * @param \p ArgFolderId - ������ ���������������� �����, ����� ���������
	 * ����� (� �������� 0).
	 */
	void DeleteUserFolder(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= storageOfFoldersNames.Get_numOfItems()) ||
		     (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT)
		   )
		{
			// ����������� ������! ��� ��������� ����� � ����� �������� !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		if (ArgFolderId == 0)
		{
			// ����������� ������! ������ ������� ��������� ����� (� �������� 0) !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		
		//-------------------------
		storagesOfRecords[map[ArgFolderId]].DeleteAllItems();
		summmasOfRecordsLengths[map[ArgFolderId]] = 0;
		storageOfFoldersNames.DeleteItem(ArgFolderId);
		// ��� �������� �������� � ��������� �����, ��� ������� ��������� ���������,
		// ����� ���������� ������� ����������� ��������� � ����� �����������
		// �������� �������� � �����. ��������:
		auto n = storageOfFoldersNames.Get_numOfItems(); // ����� ���������� ��������� � ��������� �����.
		// ������ ������������ ������ ����� ������ ����� ����� ��������� � ��������� �����.
		if (n <= ArgFolderId)
		{ // ���� ��� ����� � ��������� ������ ��� � ��������:
			; // ������ �������� �� ���������
		}
		else
		{
			// �������� �������� �������� � �����:
			for (auto fi = ArgFolderId; fi < n; fi++)
			{
				map[fi] = map[fi + 1];
			}
		}
		callCallback(PtrCallbackOnFoldersChanged);
	}
	//--------------------------------------------------------------------------
private:
	/* ����� �� ����� ���������� ��������� ������� (��� �������):
	 * ��������� ��������� ������� �������� ������� (rsi) �� 0 � ����� �� ��������
	 * ����� (fi) �� 0 �� max: ���� rsi != map[fi=0..max]
	 * 
	 * ArgFolderIdMax - ������������ ������ �����.
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
		ASSERT_CUSTOM(0, false); // ����������� ������! ��� ��������� ������!
		return 0; // default ��� ����������� ������
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ������� �����.
	 * �� ��������� �������� ������� PtrCallbackOnFoldersChanged.
	 * ����� ����� ��������� ���, ���������� �� ���������� ��������� ������
	 * \p BufTxtFolderName[NOTEPAD_FOLDER_NAME_TEXT_SIZE].
	 * ����� ����� �������� ������, ��������� �� ������� ������������.
	 * ����������� ����� ����������� \p NOTEPAD_FOLDER_NAME_TEXT_SIZE ������.
	 * ����������� ��������� ���������� ����� \p NOTEPAD__FOLDERS_AMOUNT.
	 * ��� ���������� ����� ��� ����� ����� (���� �������
	 * ���������� > NOTEPAD__FOLDERS_AMOUNT), ����� ������ ASSERT_CUSTOM �
	 * ����� �� ����� �������, ������� ��������  ������ ���� ����������
	 * ������� ����.
	 */
	void CreateFolderByBuffer(void)
	{
		if( storageOfFoldersNames.Get_numOfItems() == NOTEPAD__FOLDERS_AMOUNT )
		{
			// ������! ��� ����� ��� ����� �����!
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//------------------------------
		storageOfFoldersNames.AddItemToBack(BufTxtFolderName);
		uint8_t idxFolder = storageOfFoldersNames.Get_numOfItems() - 1;
		// ������������ ����� ��������� �������:
		// ���� ������ ����� == 0, �� ������� ��� ��������� ������� � �������� 0-�.
		// ����� - ������ ����� ���������� ��������� ������� �� ����� (�������
		// ���������� ��������� �������).
		uint8_t idxStorage = 0;
		if ( idxFolder != 0 )
		{
			idxStorage = findEmptyRecordsStorageId(idxFolder - 1);
		}
		// ��������� ������ ��������� � �����:
		map[idxFolder] = idxStorage;
		summmasOfRecordsLengths[map[idxFolder]] = 0;

		callCallback(PtrCallbackOnFoldersChanged);
	}
	//--------------------------------------------------------------------------
private:
	/* ��������� strlen ����� � ��������� �������.
	 * ArgIdOfRecordsStorage - ������ ��������� �������
	 * ���������� ����� strlen �� �������.
	 */
	uint32_t calcSumOfLengthsInStorage(uint8_t ArgIdOfRecordsStorage)
	{
		uint32_t ret = 0; // � ������ ������ ��������� 0
		//------------------------
		if ( ArgIdOfRecordsStorage >= NOTEPAD__FOLDERS_AMOUNT )
		{
			// ������! ��� ��������� ������� � ������� �������� ������� � ����� �������� !
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
	 * @brief ������� ��������� ������ � ����� � �������� \p ArgFolderId.
	 * �� ��������� ����� ������ ������ PtrCallbackOnRecordsChangedInFolder.
	 * \n ������ ����� �������� ������, ��������� �� ������� ������������.
	 * � ������ ����� ���������� ����� �� ����������� ���������� ������
	 * \p BufTxtRecord[NOTEPAD__RECORD_TEXT_SIZE].
	 * ����������� �� ������ ����� ����������� \p NOTEPAD__RECORD_TEXT_SIZE ������.
	 * ��� ���������� ����� � ��������� �������, �������������� ����� ������� �����
	 * ������ ������ (� �������� 0).
	 * @param \p ArgFolderId - [in] ������ �����. ��� ������������ \p ArgFolderId
	 * ����� ������ ASSERT_CUSTOM, �������
	 * ������������ ������� ������ ���� ���������� ������� ����.
	 */
	void CreateRecordByBuffer(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= storageOfFoldersNames.Get_numOfItems()) ||
		     (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT)
		   )
		{
			// ������! ��� ��������� ����� � ����� �������� !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//----------------------------------------
		// ���������, ��� ��������� summmasOfRecordsLengths[] ����� ����������� � ���������� �������:
		auto n = storagesOfRecords[map[ArgFolderId]].Get_numOfItems(); // ���������� ������� �� ���������
		bool choice = n == NOTEPAD__FOLDER_SIZE; /* true - ������, ������ storagesOfRecords ����� 
												  * ��������� ������� � �������� 0 � ����� ������
												  * ����������� �� ������� [NOTEPAD__FOLDER_SIZE - 1]
												  * ������� ����� ������������� ������;
												  * false - ������, ����� �������� ������� �� ������� [n]
												  * ������� ������ �������� ������ ����� �������� � �����.
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
			// ����������� ������! ����� ���� ������� ��������� ��������!
			ASSERT_CUSTOM( 0, false );
			return;
		}
		callCallback(PtrCallbackOnRecordsChangedInFolder);
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief �������� ������ � ����� �� ��������.
	 * �� ��������� ����� ������ ������ PtrCallbackOnRecordsChangedInFolder.
	 * ������������ �������� ����������� ASSERT_CUSTOM.
	 * (�.�. �������� �������� ������ ����������� ������� ����).
	 * 
	 * @param \p ArgFolderId - ������ �����.
	 * @param \p ArgRecordId - ������ ������ � �����.
	 */
	void DeleteRecord(uint8_t ArgFolderId, uint8_t ArgRecordId)
	{
		if ( (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT) ||
			 (ArgFolderId >= storageOfFoldersNames.Get_numOfItems())
		   )
		{
			// ������! ��� ��������� ����� � ����� �������� !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		if (ArgFolderId >= storagesOfRecords[map[ArgFolderId]].Get_numOfItems())
		{
			// ������! ��� ��������� ������ � ����� �������� !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//--------------------------------
		// ������� �������������� �������� �����
		storagesOfRecords[map[ArgFolderId]].GetItemText(ArgRecordId, BufTxtRecord);
		summmasOfRecordsLengths[map[ArgFolderId]] -= std::strlen(BufTxtRecord);

		storagesOfRecords[map[ArgFolderId]].DeleteItem(ArgRecordId);

		callCallback(PtrCallbackOnRecordsChangedInFolder);
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief �������� ���� ������� � �����.
	 * �� ��������� ����� ������ ������ PtrCallbackOnRecordsChangedInFolder.
	 * ������������ ������� ����������� ASSERT_CUSTOM.
	 * (�.�. �������� ������� ������ ����������� ������� ����).
	 * 
	 * @param \p ArgFolderId - ������ �����.
	 */
	void DeleteAllRecordsInFolder(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT) ||
			 (ArgFolderId >= storageOfFoldersNames.Get_numOfItems())
		   )
		{
			// ������! ��� ��������� ����� � ����� �������� !
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
	 * @brief ��������� ����� � ������ ����� �� ���������� ����� ����� �����
	 * \p BufTxtFolderName[NOTEPAD_FOLDER_NAME_TEXT_SIZE].
	 * ��� ������������ \p ArgFolderId ����� ������ ASSERT_CUSTOM, �������
	 * ������������ ������� ������ ���� ���������� ������� ����.
	 * 
	 * @param \p ArgFolderId - ������ �����.
	 */
	void ReadFolderNameToBuffer(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT) ||
		     (ArgFolderId >= storageOfFoldersNames.Get_numOfItems())
		   )
		{
			// ������! ��� ��������� ����� � ����� �������� !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//-------------------------
		storageOfFoldersNames.GetItemText(ArgFolderId, BufTxtFolderName);
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ������ ������ � ����� �� �������� �� ���������� ����� BufTxtRecord
	 * \p BufTxtRecord[NOTEPAD__RECORD_TEXT_SIZE].
	 * ������������ �������� ����������� ASSERT_CUSTOM.
	 * (�.�. �������� �������� ������ ����������� ������� ����).
	 * 
	 * @param \p ArgFolderId - ������ �����.
	 * @param \p ArgRecordId - ������ ������ � �����.
	 */
	void ReadRecordToBuffer(uint8_t ArgFolderId, uint8_t ArgRecordId)
	{
		if ( (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT) ||
		     (ArgFolderId >= storageOfFoldersNames.Get_numOfItems())
		   )
		{
			// ������! ��� ��������� ����� � ����� �������� !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		if (ArgRecordId >= storagesOfRecords[map[ArgFolderId]].Get_numOfItems())
		{
			// ������! ��� ��������� ������ � ����� �������� !
			ASSERT_CUSTOM( 0, false );
			return;
		}
		//--------------------------------
		storagesOfRecords[map[ArgFolderId]].GetItemText(ArgRecordId, BufTxtRecord);
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ���������� ����� strlen ��� ���� ������� � �����
	 * (strlen ������� �� ������� ������������� '\0').
	 * 
	 * @param \p ArgFolderId - ������ �����, ����������� � ������� ASSERT_CUSTOM.
	 */
	uint32_t GetSumOfRecordsLengths(uint8_t ArgFolderId)
	{
		if ( (ArgFolderId >= NOTEPAD__FOLDERS_AMOUNT) ||
		     (ArgFolderId >= storageOfFoldersNames.Get_numOfItems())
		   )
		{
			// ������! ��� ��������� ����� � ����� �������� !
			ASSERT_CUSTOM( 0, false );
			return 0; // 0 � ������ ������
		}
		//-------------------------
		return summmasOfRecordsLengths[map[ArgFolderId]];
	}
	//==========================================================================
};
//******************************************************************************
