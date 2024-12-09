#include <ctype.h>

#include "stm32h7xx_hal.h"
#include "keyboard/ps2_keyboard.h"
#include "keyboard/queue.h"

typedef enum
{
    IDLE = 10,
    BIT0 = 0,
    BIT1 = 1,
    BIT2 = 2,
    BIT3 = 3,
    BIT4 = 4,
    BIT5 = 5,
    BIT6 = 6,
    BIT7 = 7,
    PARITY = 8,
    STOP = 9
} ps2_read_status;

static int32_t ps2_status;
static int32_t kb_data;

static uint32_t lastReceived;
static uint8_t lastData;
uint32_t ModifierKeyState;
static volatile uint8_t _parity;

void Ps2_Initialize()
{
    ps2_status = IDLE;
    QueueInit();
    kb_data = 0;
    lastReceived = 0;
    ModifierKeyState = 0;
}

int32_t Ps2_GetScancode()
{
    int32_t result;
    if (QueueGet(&result) == -1)
    {
        return 0;
    }

    int32_t code = result & 0xFF;
    bool isPressed = ((result & 0xFF00) != 0xF000);

    switch (code)
    {
    case KEY_LEFTSHIFT:
    	if (isPressed)
    	{
            ModifierKeyState |= ModifierKeys::LeftShift;
    	}
    	else
    	{
            ModifierKeyState &= ~ModifierKeys::LeftShift;
    	}
    	break;
    case KEY_RIGHTSHIFT:
    	if (isPressed)
    	{
            ModifierKeyState |= ModifierKeys::RightShift;
    	}
    	else
    	{
            ModifierKeyState &= ~ModifierKeys::RightShift;
    	}
    	break;
    case KEY_L_GUI:
    	if (isPressed)
    	{
            ModifierKeyState |= ModifierKeys::LeftWindows;
    	}
    	else
    	{
            ModifierKeyState &= ~ModifierKeys::LeftWindows;
    	}
    	break;
    case KEY_R_GUI:
    	if (isPressed)
    	{
            ModifierKeyState |= ModifierKeys::RightWindows;
    	}
    	else
    	{
            ModifierKeyState &= ~ModifierKeys::RightWindows;
    	}
    	break;
    case KEY_LEFTCONTROL:
    	if (isPressed)
    	{
            ModifierKeyState |= ModifierKeys::LeftControl;
    	}
    	else
    	{
            ModifierKeyState &= ~ModifierKeys::LeftControl;
    	}
    	break;
    case KEY_RIGHTCONTROL:
    	if (isPressed)
    	{
            ModifierKeyState |= ModifierKeys::RightControl;
    	}
    	else
    	{
            ModifierKeyState &= ~ModifierKeys::RightControl;
    	}
    	break;
    case KEY_ALT:
    	if (isPressed)
    	{
            ModifierKeyState |= ModifierKeys::LeftAlt;
    	}
    	else
    	{
            ModifierKeyState &= ~ModifierKeys::LeftAlt;
    	}
    	break;
    case KEY_RIGHTALT:
    	if (isPressed)
    	{
            ModifierKeyState |= ModifierKeys::RightAlt;
    	}
    	else
    	{
            ModifierKeyState &= ~ModifierKeys::RightAlt;
    	}
    	break;
    }

    return result;
}

char Ps2_ConvertScancode(int32_t scanCode)
{
	char result;

	switch (scanCode)
	{
	case KEY_A:
		result = 'a';
		break;
	case KEY_B:
		result = 'b';
		break;
	case KEY_C:
		result = 'c';
		break;
	case KEY_D:
		result = 'd';
		break;
	case KEY_E:
		result = 'e';
		break;
	case KEY_F:
		result = 'f';
		break;
	case KEY_G:
		result = 'g';
		break;
	case KEY_H:
		result = 'h';
		break;
	case KEY_I:
		result = 'i';
		break;
	case KEY_J:
		result = 'j';
		break;
	case KEY_K:
		result = 'k';
		break;
	case KEY_L:
		result = 'l';
		break;
	case KEY_M:
		result = 'm';
		break;
	case KEY_N:
		result = 'n';
		break;
	case KEY_O:
		result = 'o';
		break;
	case KEY_P:
		result = 'p';
		break;
	case KEY_Q:
		result = 'q';
		break;
	case KEY_R:
		result = 'r';
		break;
	case KEY_S:
		result = 's';
		break;
	case KEY_T:
		result = 't';
		break;
	case KEY_U:
		result = 'u';
		break;
	case KEY_V:
		result = 'v';
		break;
	case KEY_W:
		result = 'w';
		break;
	case KEY_X:
		result = 'x';
		break;
	case KEY_Y:
		result = 'y';
		break;
	case KEY_Z:
		result = 'z';
		break;
	case KEY_0:
		result = '0';
		break;
	case KEY_1:
		result = '1';
		break;
	case KEY_2:
		result = '2';
		break;
	case KEY_3:
		result = '3';
		break;
	case KEY_4:
		result = '4';
		break;
	case KEY_5:
		result = '5';
		break;
	case KEY_6:
		result = '6';
		break;
	case KEY_7:
		result = '7';
		break;
	case KEY_8:
		result = '8';
		break;
	case KEY_9:
		result = '9';
		break;
	case KEY_BACKSPACE:
		result = '\b';
		break;
	case KEY_SPACEBAR:
		result = ' ';
		break;
	case KEY_COMMA:
		result = ',';
		break;
	case KEY_MINUS:
		result = '-';
		break;
	case KEY_DOT:
		result = '.';
		break;
	case KEY_DIV:
		result = '/';
		break;
	case KEY_SINGLE:
		result = '`';
		break;
	case KEY_APOS:
		result = '\'';
		break;
	case KEY_SEMI:
		result = ';';
		break;
	case KEY_BACK:
		result = '\\';
		break;
	case KEY_OPEN_SQ:
		result = '[';
		break;
	case KEY_CLOSE_SQ:
		result = ']';
		break;
	case KEY_EQUAL:
		result = '=';
		break;
	default:
		result = '\0';
		break;
	}

	if (ModifierKeyState & (ModifierKeys::LeftShift | ModifierKeys::RightShift))
	{
		switch (scanCode)
		{
		case KEY_0:
			result = ')';
			break;
		case KEY_1:
			result = '!';
			break;
		case KEY_2:
			result = '@';
			break;
		case KEY_3:
			result = '#';
			break;
		case KEY_4:
			result = '$';
			break;
		case KEY_5:
			result = '%';
			break;
		case KEY_6:
			result = '^';
			break;
		case KEY_7:
			result = '&';
			break;
		case KEY_8:
			result = '*';
			break;
		case KEY_9:
			result = '(';
			break;
		case KEY_COMMA:
			result = '<';
			break;
		case KEY_MINUS:
			result = '_';
			break;
		case KEY_DOT:
			result = '>';
			break;
		case KEY_DIV:
			result = '?';
			break;
		case KEY_SINGLE:
			result = '~';
			break;
		case KEY_APOS:
			result = '"';
			break;
		case KEY_SEMI:
			result = ':';
			break;
		case KEY_BACK:
			result = '|';
			break;
		case KEY_OPEN_SQ:
			result = '{';
			break;
		case KEY_CLOSE_SQ:
			result = '}';
			break;
		case KEY_EQUAL:
			result = '+';
			break;
		default:
			result = toupper(result);
			break;
		}
	}

	return result;
}

inline void Update(uint8_t dataBit)
{
    switch (ps2_status)
    {
    case IDLE:
        ps2_status = BIT0;
        _parity = 0;
        break;

    case BIT0:
        ps2_status = BIT1;
        kb_data |= dataBit;
        _parity ^= dataBit;
        break;

    case BIT1:
    case BIT2:
    case BIT3:
    case BIT4:
    case BIT5:
    case BIT6:
    case BIT7:
        kb_data |= (dataBit << ps2_status);
        ps2_status++;
        _parity ^= dataBit;
        break;

    case PARITY:
        if (_parity == dataBit)
        {
            // Parity error
            _parity = 0xFD;
            kb_data = 0;

            // TODO ask to resend
        }
        ps2_status = STOP;
        break;

    case STOP:
        ps2_status = IDLE;
        if (_parity != 0xFD)
        {
            if ((kb_data & 0xE0) == 0xE0)
            {
                kb_data <<= 8;
            }
            else
            {
                QueuePut(kb_data);
                kb_data = 0;
            }
        }
        break;
    }
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin != CLK_PIN)
	{
		return;
	}

	uint32_t currentTick = HAL_GetTick();
	if (ps2_status != IDLE)
	{
		if (lastReceived != 0 && ((currentTick - lastReceived) > 250))
		{
			// Timeout
			ps2_status = IDLE;
			kb_data = 0;
		}

	}
	lastReceived = currentTick;

	uint32_t gpioBits = KBD_GPIO->IDR;
	lastData = (gpioBits & DATA_PIN) ? 1 : 0;
	Update(lastData);
}
