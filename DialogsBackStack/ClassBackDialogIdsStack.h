#include "ClassStackStatic.h"

//==============================================================================
/**
 * @brief ��������� ������ ����� ��������� ��� ��������.
 */
class ClassBackDialogIdsStack : public ClassStackStatic<uint8_t>
{
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief Construct a Back Dialog Ids Stack object
	 * 
	 * @param ArgNItemsMax - ����. ���������� ��������� � �����. ��-��������� - 5.
	 */
	ClassBackDialogIdsStack( uint8_t ArgNItemsMax = 5 ) : ClassStackStatic<uint8_t>(ArgNItemsMax)
	{
		
	}
	//--------------------------------------------------------------------------
public:
	/**
	 * @brief ��������� ������ ����� ��������� ��� ��������.
	 * ���� � ����� ������������� @arg ArgDialogIdToActivate.
	 * - ���� ����� ������� ������, �� ��� ������, ��� ���������� ������� �� ��
	 * ��������� ������ ������ ����, � ������� � ����������� � ��������� ��
	 * ����� ����� � ����������� ���������. ���������� �������� ���������
	 * ������������ � ������� ��������� ����������� ������� ������� �����.
	 * - ���� �� ������, �� �������� � ���� ������������� @arg ArgDialogIdActive.
	 * 
	 * @param \p ArgDialogIdToActivate - ��-�� ���������� �������, ������� ����� ������������.
	 * @param \p ArgDialogIdActive - ��-�� ��������� (���������) �������.
	 */
    void AddCurrOrDeleteLoop(const uint8_t &  ArgDialogIdToActivate, const uint8_t & ArgDialogIdActive)
    {
        int16_t idx = -1;   // ��� ���������� �������-���������� ������ �� ��������� �����. -1 - �������, ��� ������� �� ������.
		if(iEnd > 0)
		{ // ���� � ����� ���� �������� (�� ������)
			// ���� � ����� �������������, ������ ArgDialogIdToActivate
			for (uint8_t i = 0; i < iEnd; i++)
			{
				if( pData[i] == ArgDialogIdToActivate )
				{
					idx = i;	// ���������� ������ ���������� �������� �����
					break;
				}
			}
		}

        if (idx == -1)
		{ // ���� ������������� �� ������, �������� ��� � �����,...
            AddToBack(ArgDialogIdActive);
        }
        else
		{ // ���� ������������� �� ������, ������� ��� ���� � �����,...
			/* ������ ������ "��������� �������", �.�. ���������� ������� �
			 * ���������� ������. ������, ����� ������� �������� ������� � �����
			 * �������.
			 */
			iEnd = static_cast<uint8_t>(idx);	// ������� ��������, ������� � idx-���� � �� �����
        }

    }
	//--------------------------------------------------------------------------
};
//******************************************************************************