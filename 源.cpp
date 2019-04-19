#include"��ͷ.h"
#include<string>
using namespace std;
INSTRUCTION  Instruction;

bool FillMODR_M12345(INSTRUCTION &instruction)
{
	instruction.MODR_M12 = instruction.ModR_M >> 6;
	instruction.ModR_M345 = (instruction.ModR_M & 0x38) >> 3;
	return TRUE;
}
bool FillSIB_Base(INSTRUCTION &instruction)
{
	instruction.SIB_Base = instruction.SIB & 7;
	return TRUE;
}

bool FillInstruction(byte * ArrayofData, INSTRUCTION &Instruction)       //ָ����亯��
{
	byte * P_index = ArrayofData;                                        //��¼����ȡ����λ��
	switch (OpCode_Table[*P_index])
	{// ��ǰ׺
	case _prefix: //��һ���ֽھ���prefix����                                            
		Instruction.Prefix = *P_index;
		P_index++;
	case _modr_m: 
	case _imme8:
	case _imme16:
	case _imme32:
	case _onebyte:
	case _double_opcode:
	case _modr_m | _imme8:
	case _modr_m|_imme32:
	case _modr_m|_group|_imme8:
	case _modr_m |_group|_imme32:
	case _modr_m|_group:
	case _group:
	case _imme16|_imme32:
			switch (OpCode_Table[*P_index])
	{
		case _modr_m:  //�ڶ����ֽ�opcode����modr���� 
			Instruction.Opecode = *P_index;
			P_index++;
			Instruction.ModR_M = *P_index;
			P_index++;
			if (Instruction.Prefix ==0x67)                                 //����0x67ǰ׺,ʹ��16λ��ַ��
			{
				switch (_16Bit_ModRM[Instruction.ModR_M])
				{
				case _nomore:  //�������ֽ�modrm����nomore���� 
					Instruction.InstructionLength = P_index - ArrayofData;         //����ָ��ռ���ֽ���
					return TRUE;

				case _disp8:   //�������ֽ�modrm����disp8����
					Instruction.Displacement.Disp8 = *P_index;
					P_index++;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				case _disp16: //�������ֽ�modrm����disp16����

					Instruction.Displacement.Disp16 = *(WORD *)P_index;
					P_index += 2;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				}
			}
			
				switch (_32Bit_ModRM[Instruction.ModR_M])
				{
				case _nomore:  //�������ֽ�modrm����nomore���� 
					Instruction.InstructionLength = P_index - ArrayofData;         //����ָ��ռ���ֽ���
					return TRUE;

				case _disp8:   //�������ֽ�modrm����disp8����
					Instruction.Displacement.Disp8 = *P_index;
					P_index++;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				case _disp32: //�������ֽ�modrm����disp32����

					Instruction.Displacement.Disp32 = *(DWORD *)P_index;
					P_index += 4;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;
				case _sib:  //�������ֽ�modrm����sib����

					Instruction.SIB = *P_index;
					P_index++;
					FillMODR_M12345(Instruction);
					if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
					{
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;

					}
					else                                //���ĸ��ֽ�sib��displacement
					{
						FillSIB_Base(Instruction);     //���SIB_Base
						if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
						}
						else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp8 = *P_index;
							P_index++;
						}
						else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
						}
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;
					}
				case _sib | _disp8:    //�������ֽ�modrm����sib|disp8���� 
					Instruction.SIB = *P_index;
					P_index++;
					FillMODR_M12345(Instruction);
					if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
					{  
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;

					}
					else
					{
						FillSIB_Base(Instruction);
						//���ĸ��ֽ�sib��displacement
						if (Instruction.MODR_M12 == 0 &&Instruction.SIB_Base == 5)//modrm��sib����displacement
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *P_index;
							P_index++;
						}
						else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp16 = *P_index;
							P_index++;
							Instruction.Displacement.Disp16 += *P_index;
							P_index++;
						}
						else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *P_index;
							P_index++;
						}

						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;
					}
				case _sib|_disp32:  //�������ֽ�modrm����sib|disp32����
					Instruction.SIB = *P_index;
					P_index++;
					FillMODR_M12345(Instruction);
					if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
					{
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;

					}
					else
					{
						                                                                                      //���ĸ��ֽ�sib��displacement
						if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)//modrm��sib����displacement
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *(DWORD *)P_index;
							P_index+=4;
						}
						else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *P_index;
							P_index++;
							Instruction.Displacement.Disp32 += *(DWORD *)P_index;
							P_index+=4;
						}
						else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *(DWORD *)P_index;
							P_index+=4;
						}

						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;
					}

				}
			
	case _imme8://�ڶ����ֽ�opcode����imme8����
		Instruction.Opecode = *P_index;
		P_index++;
		Instruction.Immediate.Imme8 = *P_index;
		P_index++;
		Instruction.InstructionLength = P_index - ArrayofData;
		return TRUE;
	case _imme16://�ڶ����ֽ�opcode����imme16����
		Instruction.Opecode = *P_index;
		P_index++;
		if (Instruction.Prefix == 0x66)//����0x66ǰ׺
		{
			Instruction.Immediate.Imme32 = *(DWORD *)P_index;
			P_index += 4;
			Instruction.InstructionLength = P_index - ArrayofData;
			return TRUE;
		}
		else 
		{
			Instruction.Immediate.Imme16 = *(WORD *)P_index;
			P_index += 2;
			Instruction.InstructionLength = P_index - ArrayofData;
			return TRUE;
		}
	case _imme32://�ڶ����ֽ�opcode����imme32����
		Instruction.Opecode = *P_index;
		P_index++;
		if (Instruction.Prefix == 0x66)//����0x66ǰ׺
		{
			Instruction.Immediate.Imme16 = *(WORD *)P_index;
			P_index += 2;
			Instruction.InstructionLength = P_index - ArrayofData;
			return TRUE;
		}
		else
		{
			Instruction.Immediate.Imme32 = *(DWORD *)P_index;
			P_index += 4;
			Instruction.InstructionLength = P_index - ArrayofData;
			return TRUE;
		}
	case _onebyte://�ڶ����ֽ�opcode����onebyte����
		Instruction.Opecode = *P_index;
		P_index++;
		Instruction.InstructionLength = P_index - ArrayofData;
		return TRUE;
	case _double_opcode://�ڶ����ֽ�opcode����double-opcode����
		P_index++;    //��һ���ֽڱ�Ϊ0x0f�����洢ֱ�Ӵ�ӡ
		Instruction.Double_Opcode = *P_index;
		P_index++;
		Instruction.InstructionLength = P_index - ArrayofData;
		return TRUE;
	case _modr_m |_imme8: // �ڶ����ֽ�opcode����_modr_m |_imme8����
		Instruction.Opecode = *P_index;
		P_index++;
		Instruction.ModR_M = *P_index;
		P_index++;
		if (Instruction.Prefix == 0x67)                                 //����0x67ǰ׺,ʹ��16λ��ַ��
		{
			switch (_16Bit_ModRM[Instruction.ModR_M])
			{
			case _nomore:  //�������ֽ�modrm����nomore���� 
				Instruction.Immediate.Imme8 = *P_index;
				P_index++;
				Instruction.InstructionLength = P_index - ArrayofData;         //����ָ��ռ���ֽ���
				return TRUE;

			case _disp8:   //�������ֽ�modrm����disp8����
				Instruction.Displacement.Disp8 = *P_index;
				P_index++;
				Instruction.Immediate.Imme8 = *P_index;
				P_index++;
				Instruction.InstructionLength = P_index - ArrayofData;
				return TRUE;

			case _disp16: //�������ֽ�modrm����disp16����

				Instruction.Displacement.Disp16 = *(WORD *)P_index;
				P_index += 2;
				Instruction.Immediate.Imme8 = *P_index;
				P_index++;
				Instruction.InstructionLength = P_index - ArrayofData;
				return TRUE;

			}
		}
		
			switch (_32Bit_ModRM[Instruction.ModR_M])
			{
			case _nomore:  //�������ֽ�modrm����nomore����
				Instruction.Immediate.Imme8 = *P_index;
				P_index++;
				Instruction.InstructionLength = P_index - ArrayofData;         //����ָ��ռ���ֽ���
				return TRUE;

			case _disp8:   //�������ֽ�modrm����disp8����
				Instruction.Displacement.Disp8 = *P_index;
				P_index++;
				Instruction.Immediate.Imme8 = *P_index;
				P_index++;
				Instruction.InstructionLength = P_index - ArrayofData;
				return TRUE;

			case _disp32: //�������ֽ�modrm����disp32����

				Instruction.Displacement.Disp32 = *(DWORD *)P_index;
				P_index += 4;
				Instruction.Immediate.Imme8 = *P_index;
				P_index++;
				Instruction.InstructionLength = P_index - ArrayofData;
				return TRUE;
			case _sib:  //�������ֽ�modrm����sib����

				Instruction.SIB = *P_index;
				P_index++;
				FillMODR_M12345(Instruction);
				if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
				{
					Instruction.Immediate.Imme8 = *P_index;
					P_index++;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				}
				else                                //���ĸ��ֽ�sib��displacement
				{
					FillSIB_Base(Instruction);     //���SIB_Base
					if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
					}
					else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp8 = *P_index;
						P_index++;
					}
					else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
					}
					Instruction.Immediate.Imme8 = *P_index;
					P_index++;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;
				}
			case _sib | _disp8:    //�������ֽ�modrm����sib|disp8���� 
				Instruction.SIB = *P_index;
				P_index++;
				FillMODR_M12345(Instruction);
				if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
				{
					Instruction.Immediate.Imme8 = *P_index;
					P_index++;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				}
				else
				{
					FillSIB_Base(Instruction);
					//���ĸ��ֽ�sib��displacement
					if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)//modrm��sib����displacement
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
						Instruction.Displacement.Disp32 += *P_index;
						P_index++;
					}
					else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp16 = *P_index;
						P_index++;
						Instruction.Displacement.Disp16 += *P_index;
						P_index++;
					}
					else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
						Instruction.Displacement.Disp32 += *P_index;
						P_index++;
					}
					Instruction.Immediate.Imme8 = *P_index;
					P_index++;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;
				}
			case _sib | _disp32:  //�������ֽ�modrm����sib|disp32����
				Instruction.SIB = *P_index;
				P_index++;
				FillMODR_M12345(Instruction);
				if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
				{
					Instruction.Immediate.Imme8 = *P_index;
					P_index++;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				}
				else
				{
					//���ĸ��ֽ�sib��displacement
					if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)//modrm��sib����displacement
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
						Instruction.Displacement.Disp32 += *(DWORD *)P_index;
						P_index += 4;
					}
					else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp32 = *P_index;
						P_index++;
						Instruction.Displacement.Disp32 += *(DWORD *)P_index;
						P_index += 4;
					}
					else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
						Instruction.Displacement.Disp32 += *(DWORD *)P_index;
						P_index += 4;
					}
				
					Instruction.Immediate.Imme8 = *P_index;
					P_index++;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;
				}

			}



		

		case _modr_m | _imme32:  //�ڶ����ֽ�opcode����_modr_m |_imme32����
			Instruction.Opecode = *P_index;
			P_index++;
			Instruction.ModR_M = *P_index;
			P_index++;
			if (Instruction.Prefix == 0x67)                                 //����0x67ǰ׺,ʹ��16λ��ַ��
			{
				switch (_16Bit_ModRM[Instruction.ModR_M])
				{
				case _nomore:  //�������ֽ�modrm����nomore���� 
					Instruction.Immediate.Imme32 = *(DWORD *)P_index;
					P_index+=4;
					Instruction.InstructionLength = P_index - ArrayofData;         //����ָ��ռ���ֽ���
					return TRUE;

				case _disp8:   //�������ֽ�modrm����disp8����
					Instruction.Displacement.Disp8 = *P_index;
					P_index++;
					Instruction.Immediate.Imme32 = *(DWORD *)P_index;
					P_index += 4;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				case _disp16: //�������ֽ�modrm����disp16����

					Instruction.Displacement.Disp16 = *(WORD *)P_index;
					P_index += 2;
					Instruction.Immediate.Imme32 = *(DWORD *)P_index;
					P_index += 4;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				}
			}
			else if (Instruction.Prefix == 0x66)                           //����0x66ǰ׺
			{
				switch (_32Bit_ModRM[Instruction.ModR_M])
				{
				case _nomore:  //�������ֽ�modrm����nomore���� 
					Instruction.Immediate.Imme16 = *(WORD *)P_index;        //0x66ǰ׺ʹ����������Ϊ16λ
					P_index+=2;
					Instruction.InstructionLength = P_index - ArrayofData;         //����ָ��ռ���ֽ���
					return TRUE;

				case _disp8:   //�������ֽ�modrm����disp8����
					Instruction.Displacement.Disp8 = *P_index;
					P_index++;
					Instruction.Immediate.Imme16 = *(WORD *)P_index;        //0x66ǰ׺ʹ����������Ϊ16λ
					P_index += 2;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				case _disp32: //�������ֽ�modrm����disp32����

					Instruction.Displacement.Disp32 = *(DWORD *)P_index;
					P_index += 4;
					Instruction.Immediate.Imme16 = *(WORD *)P_index;        //0x66ǰ׺ʹ����������Ϊ16λ
					P_index += 2;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;
				case _sib:  //�������ֽ�modrm����sib����

					Instruction.SIB = *P_index;
					P_index++;
					FillMODR_M12345(Instruction);
					if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
					{
						Instruction.Immediate.Imme16 = *(WORD *)P_index;        //0x66ǰ׺ʹ����������Ϊ16λ
						P_index += 2;
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;

					}
					else                                //���ĸ��ֽ�sib��displacement
					{
						FillSIB_Base(Instruction);     //���SIB_Base
						if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
						}
						else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp8 = *P_index;
							P_index++;
						}
						else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
						}
						Instruction.Immediate.Imme16 = *(WORD *)P_index;        //0x66ǰ׺ʹ����������Ϊ16λ
						P_index += 2;
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;
					}
				case _sib | _disp8:    //�������ֽ�modrm����sib|disp8���� 
					Instruction.SIB = *P_index;
					P_index++;
					FillMODR_M12345(Instruction);
					if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
					{
						Instruction.Immediate.Imme16 = *(WORD *)P_index;        //0x66ǰ׺ʹ����������Ϊ16λ
						P_index += 2;
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;

					}
					else
					{
						FillSIB_Base(Instruction);
						//���ĸ��ֽ�sib��displacement
						if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)//modrm��sib����displacement
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *P_index;
							P_index++;
						}
						else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp16 = *P_index;
							P_index++;
							Instruction.Displacement.Disp16 += *P_index;
							P_index++;
						}
						else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *P_index;
							P_index++;
						}
						Instruction.Immediate.Imme16 = *(WORD *)P_index;        //0x66ǰ׺ʹ����������Ϊ16λ
						P_index += 2;
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;
					}
				case _sib | _disp32:  //�������ֽ�modrm����sib|disp32����
					Instruction.SIB = *P_index;
					P_index++;
					FillMODR_M12345(Instruction);
					if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
					{
						Instruction.Immediate.Imme16 = *(WORD *)P_index;        //0x66ǰ׺ʹ����������Ϊ16λ
						P_index += 2;
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;

					}
					else
					{
						//���ĸ��ֽ�sib��displacement
						if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)//modrm��sib����displacement
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *(DWORD *)P_index;
							P_index += 4;
						}
						else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *P_index;
							P_index++;
							Instruction.Displacement.Disp32 += *(DWORD *)P_index;
							P_index += 4;
						}
						else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *(DWORD *)P_index;
							P_index += 4;
						}
						Instruction.Immediate.Imme16 = *(WORD *)P_index;        //0x66ǰ׺ʹ����������Ϊ16λ
						P_index += 2;
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;
					}
				}
			}
			else //û��0x66ǰ׺
{
          switch (_32Bit_ModRM[Instruction.ModR_M])
			{
			case _nomore:  //�������ֽ�modrm����nomore���� 
				Instruction.Immediate.Imme32 = *(DWORD *)P_index;      
				P_index += 4;
				Instruction.InstructionLength = P_index - ArrayofData;         //����ָ��ռ���ֽ���
				return TRUE;

			case _disp8:   //�������ֽ�modrm����disp8����
				Instruction.Displacement.Disp8 = *P_index;
				P_index++;
				Instruction.Immediate.Imme32 = *(DWORD *)P_index;      
				P_index += 4;
				Instruction.InstructionLength = P_index - ArrayofData;
				return TRUE;

			case _disp32: //�������ֽ�modrm����disp32����

				Instruction.Displacement.Disp32 = *(DWORD *)P_index;
				P_index += 4;
				Instruction.Immediate.Imme32 = *(DWORD *)P_index;       
				P_index += 4;
				Instruction.InstructionLength = P_index - ArrayofData;
				return TRUE;
			case _sib:  //�������ֽ�modrm����sib����

				Instruction.SIB = *P_index;
				P_index++;
				FillMODR_M12345(Instruction);
				if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
				{
					Instruction.Immediate.Imme32 = *(DWORD *)P_index;
					P_index += 4;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				}
				else                                //���ĸ��ֽ�sib��displacement
				{
					FillSIB_Base(Instruction);     //���SIB_Base
					if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
					}
					else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp8 = *P_index;
						P_index++;
					}
					else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
					}
					Instruction.Immediate.Imme32 = *(DWORD *)P_index;
					P_index += 4;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;
				}
			case _sib | _disp8:    //�������ֽ�modrm����sib|disp8���� 
				Instruction.SIB = *P_index;
				P_index++;
				FillMODR_M12345(Instruction);
				if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
				{
					Instruction.Immediate.Imme32 = *(DWORD *)P_index;
					P_index += 4;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				}
				else
				{
					FillSIB_Base(Instruction);
					//���ĸ��ֽ�sib��displacement
					if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)//modrm��sib����displacement
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
						Instruction.Displacement.Disp32 += *P_index;
						P_index++;
					}
					else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp16 = *P_index;
						P_index++;
						Instruction.Displacement.Disp16 += *P_index;
						P_index++;
					}
					else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
						Instruction.Displacement.Disp32 += *P_index;
						P_index++;
					}
					Instruction.Immediate.Imme32 = *(DWORD *)P_index;
					P_index += 4;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;
				}
			case _sib | _disp32:  //�������ֽ�modrm����sib|disp32����
				Instruction.SIB = *P_index;
				P_index++;
				FillMODR_M12345(Instruction);
				if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
				{
					Instruction.Immediate.Imme32 = *(DWORD *)P_index;
					P_index += 4;
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				}
				else
				{
					//���ĸ��ֽ�sib��displacement
					if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)//modrm��sib����displacement
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
						Instruction.Displacement.Disp32 += *(DWORD *)P_index;
						P_index += 4;
					}
					else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp32 = *P_index;
						P_index++;
						Instruction.Displacement.Disp32 += *(DWORD *)P_index;
						P_index += 4;
					}
					else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
					{
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
						Instruction.Displacement.Disp32 += *(DWORD *)P_index;
						P_index += 4;
					}
					Instruction.Immediate.Imme32 = *(DWORD *)P_index;
					P_index += 4; 
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;
				}
			
          }
			

}
        case _group:                        //�ڶ����ֽ�opcode����_group���� 
		case _modr_m | _group :             //�ڶ����ֽ�opcode����modrm|_group����
		case _modr_m | _group | _imme8:     //�ڶ����ֽ�opcode����_modr_m | _group | _imme8���� 
		case _modr_m | _group | _imme32:    //�ڶ����ֽ�opcode���� _modr_m | _group | _imme32���� 
			Instruction.Opecode = *P_index;
			P_index++;
			Instruction.ModR_M = *P_index;   //��group���Ա���modr_m����
			P_index++;
			FillMODR_M12345(Instruction);//���ڲ�ѯgroup��
			if (Instruction.Prefix == 0x67)                                //����0x67ǰ׺,ʹ��16λ��ַ��
			{
				switch (_16Bit_ModRM[Instruction.ModR_M])
				{
				case _nomore:  //�������ֽ�modrm����nomore���� 
					if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
					{
						Instruction.Immediate.Imme8 = *P_index;
						P_index++;
					}
					else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
					{
						Instruction.Immediate.Imme32 = *(DWORD *)P_index;
						P_index += 4;
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else
						{
							Instruction.Immediate.Imme32 += *P_index;
							P_index++;
						}
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							Instruction.Immediate.Imme32 = *(DWORD *)P_index;
							P_index += 4;
						}
						else
						{
							Instruction.Immediate.Imme32 += *(DWORD *)P_index;
							P_index += 4;
						}
					}

					Instruction.InstructionLength = P_index - ArrayofData;         //����ָ��ռ���ֽ���
					return TRUE;

				case _disp8:   //�������ֽ�modrm����disp8����
					Instruction.Displacement.Disp8 = *P_index;
					P_index++;
					if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
					{
						Instruction.Immediate.Imme8 = *P_index;
						P_index++;
					}
					else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
					{
						Instruction.Immediate.Imme32 = *(DWORD *)P_index;
						P_index += 4;
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else
						{
							Instruction.Immediate.Imme32 += *P_index;
							P_index++;
						}
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							Instruction.Immediate.Imme32 = *(DWORD *)P_index;
							P_index += 4;
						}
						else
						{
							Instruction.Immediate.Imme32 += *(DWORD *)P_index;
							P_index += 4;
						}
					}

					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				case _disp16: //�������ֽ�modrm����disp16����

					Instruction.Displacement.Disp16 = *(WORD *)P_index;
					P_index += 2;
					if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
					{
						Instruction.Immediate.Imme8 = *P_index;
						P_index++;
					}
					else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
					{
						Instruction.Immediate.Imme32 = *(DWORD *)P_index;
						P_index += 4;
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else
						{
							Instruction.Immediate.Imme32 += *P_index;
							P_index++;
						}
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							Instruction.Immediate.Imme32 = *(DWORD *)P_index;
							P_index += 4;
						}
						else
						{
							Instruction.Immediate.Imme32 += *(DWORD *)P_index;
							P_index += 4;
						}
					}

					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				}
			}
			else  //û��0x67ǰ׺                         
			{
				switch (_32Bit_ModRM[Instruction.ModR_M])
				{
				case _nomore:  //�������ֽ�modrm����nomore����
					if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
					{
						Instruction.Immediate.Imme8 = *P_index;
						P_index++;
					}
					else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
					{
						if (Instruction.Prefix == 0x66)
						{
							Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
							P_index += 2;
						}
						else
						{
							Instruction.Immediate.Imme32 = *(DWORD *)P_index;
							P_index += 4;
						}
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else
						{
							Instruction.Immediate.Imme32 += *P_index;
							P_index++;
						}
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
								P_index += 2;
							}
							else
							{
								Instruction.Immediate.Imme32 = *(DWORD *)P_index;
								P_index += 4;
							}
						}
						else
						{ 
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme32 += *(WORD *)P_index;
								P_index += 2;
							}
							else
							{
								Instruction.Immediate.Imme32 += *(DWORD *)P_index;
								P_index += 4;
							}
						}
					}

					Instruction.InstructionLength = P_index - ArrayofData;         //����ָ��ռ���ֽ���
					return TRUE;

				case _disp8:   //�������ֽ�modrm����disp8����
					Instruction.Displacement.Disp8 = *P_index;
					P_index++;
					if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
					{
						Instruction.Immediate.Imme8 = *P_index;
						P_index++;
					}
					else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
					{
						if (Instruction.Prefix == 0x66)
						{
							Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
							P_index += 2;
						}
						else
						{
							Instruction.Immediate.Imme32 = *(DWORD *)P_index;
							P_index += 4;
						}
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else
						{
							Instruction.Immediate.Imme32 += *P_index;
							P_index++;
						}
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
								P_index += 2;
							}
							else
							{
								
								Instruction.Immediate.Imme32 = *(DWORD *)P_index;
								P_index += 4;
							}
						}
						else
						{
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme32 += *(WORD *)P_index;
								P_index += 2;
							}
							else
							{
								Instruction.Immediate.Imme32 += *(DWORD *)P_index;
								P_index += 4;
							}
						}
					}
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				case _disp32: //�������ֽ�modrm����disp32����

					Instruction.Displacement.Disp32 = *(DWORD *)P_index;
					P_index += 4;
					if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
					{
						Instruction.Immediate.Imme8 = *P_index;
						P_index++;
					}
					else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
					{
						if (Instruction.Prefix == 0x66)
						{
							Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
							P_index += 2;
						}
						else
						{
							Instruction.Immediate.Imme32 = *(DWORD *)P_index;
							P_index += 4;
						}
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else
						{
							Instruction.Immediate.Imme32 += *P_index;
							P_index++;
						}
					}
					if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
					{
						if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
						{
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
								P_index += 2;
							}
							else
							{

								Instruction.Immediate.Imme32 = *(DWORD *)P_index;
								P_index += 4;
							}
						}
						else
						{
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme32 += *(WORD *)P_index;
								P_index += 2;
							}
							else
							{
								Instruction.Immediate.Imme32 += *(DWORD *)P_index;
								P_index += 4;
							}
						}
					}
					Instruction.InstructionLength = P_index - ArrayofData;
					return TRUE;

				case _sib:  //�������ֽ�modrm����sib����

					Instruction.SIB = *P_index;
					P_index++;
					FillMODR_M12345(Instruction);
					if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
					{
						if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
						{
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
								P_index += 2;
							}
							else
							{
								Instruction.Immediate.Imme32 = *(DWORD *)P_index;
								P_index += 4;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								Instruction.Immediate.Imme8 = *P_index;
								P_index++;
							}
							else
							{
								Instruction.Immediate.Imme32 += *P_index;
								P_index++;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
									P_index += 2;
								}
								else
								{

									Instruction.Immediate.Imme32 = *(DWORD *)P_index;
									P_index += 4;
								}
							}
							else
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme32 += *(WORD *)P_index;
									P_index += 2;
								}
								else
								{
									Instruction.Immediate.Imme32 += *(DWORD *)P_index;
									P_index += 4;
								}
							}
						}
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;


					}
					else                                //���ĸ��ֽ�sib��displacement
					{
						FillSIB_Base(Instruction);     //���SIB_Base
						if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
						}
						else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp8 = *P_index;
							P_index++;
						}
						else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
						}
						if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
						{
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
								P_index += 2;
							}
							else
							{
								Instruction.Immediate.Imme32 = *(DWORD *)P_index;
								P_index += 4;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								Instruction.Immediate.Imme8 = *P_index;
								P_index++;
							}
							else
							{
								Instruction.Immediate.Imme32 += *P_index;
								P_index++;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
									P_index += 2;
								}
								else
								{

									Instruction.Immediate.Imme32 = *(DWORD *)P_index;
									P_index += 4;
								}
							}
							else
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme32 += *(WORD *)P_index;
									P_index += 2;
								}
								else
								{
									Instruction.Immediate.Imme32 += *(DWORD *)P_index;
									P_index += 4;
								}
							}
						}
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;
					}
				case _sib | _disp8:    //�������ֽ�modrm����sib|disp8���� 
					Instruction.SIB = *P_index;
					P_index++;
					FillMODR_M12345(Instruction);
					if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
					{
						if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
						{
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
								P_index += 2;
							}
							else
							{
								Instruction.Immediate.Imme32 = *(DWORD *)P_index;
								P_index += 4;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								Instruction.Immediate.Imme8 = *P_index;
								P_index++;
							}
							else
							{
								Instruction.Immediate.Imme32 += *P_index;
								P_index++;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
									P_index += 2;
								}
								else
								{

									Instruction.Immediate.Imme32 = *(DWORD *)P_index;
									P_index += 4;
								}
							}
							else
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme32 += *(WORD *)P_index;
									P_index += 2;
								}
								else
								{
									Instruction.Immediate.Imme32 += *(DWORD *)P_index;
									P_index += 4;
								}
							}
						}
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;
					}
					else
					{
						FillSIB_Base(Instruction);
						//���ĸ��ֽ�sib��displacement
						if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)//modrm��sib����displacement
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *P_index;
							P_index++;
						}
						else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp16 = *P_index;
							P_index++;
							Instruction.Displacement.Disp16 += *P_index;
							P_index++;
						}
						else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *P_index;
							P_index++;
						}
						if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
						{
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
								P_index += 2;
							}
							else
							{
								Instruction.Immediate.Imme32 = *(DWORD *)P_index;
								P_index += 4;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								Instruction.Immediate.Imme8 = *P_index;
								P_index++;
							}
							else
							{
								Instruction.Immediate.Imme32 += *P_index;
								P_index++;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
									P_index += 2;
								}
								else
								{

									Instruction.Immediate.Imme32 = *(DWORD *)P_index;
									P_index += 4;
								}
							}
							else
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme32 += *(WORD *)P_index;
									P_index += 2;
								}
								else
								{
									Instruction.Immediate.Imme32 += *(DWORD *)P_index;
									P_index += 4;
								}
							}
						}
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;
					}
				case _sib | _disp32:  //�������ֽ�modrm����sib|disp32����
					Instruction.SIB = *P_index;
					P_index++;
					FillMODR_M12345(Instruction);
					if (Instruction.MODR_M12 != 0 && Instruction.MODR_M12 != 1 && Instruction.MODR_M12 != 2)   //���ĸ��ֽ�sibû��displacement
					{
						if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
						{
							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
								P_index += 2;
							}
							else
							{
								Instruction.Immediate.Imme32 = *(DWORD *)P_index;
								P_index += 4;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								Instruction.Immediate.Imme8 = *P_index;
								P_index++;
							}
							else
							{
								Instruction.Immediate.Imme32 += *P_index;
								P_index++;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
									P_index += 2;
								}
								else
								{

									Instruction.Immediate.Imme32 = *(DWORD *)P_index;
									P_index += 4;
								}
							}
							else
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme32 += *(WORD *)P_index;
									P_index += 2;
								}
								else
								{
									Instruction.Immediate.Imme32 += *(DWORD *)P_index;
									P_index += 4;
								}
							}
						}
						Instruction.Displacement.Disp32 = *(DWORD *)P_index;
						P_index += 4;
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;

					}
					else
					{
						//���ĸ��ֽ�sib��displacement
						if (Instruction.MODR_M12 == 0 && Instruction.SIB_Base == 5)//modrm��sib����displacement
						{
							if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
							{
								Instruction.Immediate.Imme8 = *P_index;
								P_index++;
							}
							else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
							{
								Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
								P_index += 2;
							}
							if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
							{
								if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
								{
									Instruction.Immediate.Imme8 = *P_index;
									P_index++;
								}
								else
								{
									Instruction.Immediate.Imme32 += *P_index;
									P_index++;
								}
							}
							if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
							{
								if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
								{
									if (Instruction.Prefix == 0x66)
									{
										Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
										P_index += 2;
									}
									else
									{
										Instruction.Immediate.Imme32 = *(DWORD *)P_index;
										P_index += 4;
									}
								}
								else
								{
									if (Instruction.Prefix == 0x66)
									{
										Instruction.Immediate.Imme32 += *(WORD *)P_index;
										P_index += 2;
									}
									else 
									{
										Instruction.Immediate.Imme32 += *(DWORD *)P_index;
										P_index += 4;


										 }
								}
							}
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *(DWORD *)P_index;
							P_index += 4;
						}
						else if (Instruction.MODR_M12 == 1 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *P_index;
							P_index++;
							Instruction.Displacement.Disp32 += *(DWORD *)P_index;
							P_index += 4;
						}
						else if (Instruction.MODR_M12 = 2 && Instruction.SIB_Base == 5)
						{
							Instruction.Displacement.Disp32 = *(DWORD *)P_index;
							P_index += 4;
							Instruction.Displacement.Disp32 += *(DWORD *)P_index;
							P_index += 4;
						}
						if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF6)   //��Ҫ������
						{
							Instruction.Immediate.Imme8 = *P_index;
							P_index++;
						}
						else if (Instruction.ModR_M345 == 0 && Instruction.Opecode == 0xF7)
						{

							if (Instruction.Prefix == 0x66)
							{
								Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
								P_index += 2;
							}
							else
							{
								Instruction.Immediate.Imme32 = *(DWORD *)P_index;
								P_index += 4;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000040)//opcode����imme8����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								Instruction.Immediate.Imme8 = *P_index;
								P_index++;
							}
							else
							{
								Instruction.Immediate.Imme32 += *P_index;
								P_index++;
							}
						}
						if (OpCode_Table[Instruction.Opecode] & 0x00000100)//opcode����imme32����
						{
							if (Instruction.Immediate.Imme32 == INVALID)//ǰ��û�����������
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme16 = *(WORD *)P_index;     //0x66ǰ׺
									P_index += 2;
								}
								else
								{
									Instruction.Immediate.Imme32 = *(DWORD *)P_index;
									P_index += 4;
								}
							}
							else
							{
								if (Instruction.Prefix == 0x66)
								{
									Instruction.Immediate.Imme32 += *(WORD *)P_index;
									P_index += 2;
								}
								else
								{

									Instruction.Immediate.Imme32 += *(DWORD *)P_index;
									P_index += 4;
								}
							}
						}
						Instruction.InstructionLength = P_index - ArrayofData;
						return TRUE;
					}

				}

			}
			
	case _imme16|_imme32:   
		Instruction.Opecode = *P_index;
		Instruction.Immediate.Imme16 = *(WORD *)P_index;
		P_index += 2;
		Instruction.Displacement.Disp32 = *(DWORD *)P_index;
		Instruction.InstructionLength = P_index - ArrayofData;
		return true;
	case _imme16|_imme8:
		Instruction.Opecode = *P_index;
		Instruction.Immediate.Imme16 = *(WORD *)P_index;
		P_index += 2;
		Instruction.Displacement.Disp8 =  *P_index;
		Instruction.InstructionLength = P_index - ArrayofData;
		return true;

    }
	
	}
}

int  main()
{

                                    //���ڴ洢������ɵ�ָ��ĸ�������
	byte ArrayOfData[50] = { 0x46 ,0xf6, 0xE4, 0xF0, 0x44, 0x6E, 0xB4};        //�洢δ������1�ֽڴ�С���飬��ʮ�����Ʊ�ʾ
   	FillInstruction(ArrayOfData, Instruction);
	Print_Instrution(Instruction);
	InitStringTable(Instruction);	
	system("pause");
	return 0;
}
