#include <iostream>
#include <Windows.h>
#include <shellapi.h>


bool closed = false;
HANDLE hSerial;

// Check connection status
bool isConnected(HANDLE hSerial)
{
    DWORD dwErrors;
    COMSTAT cs;
    if (!ClearCommError(hSerial, &dwErrors, &cs))
        return false;

    return (dwErrors == 0);
}

void CloseConnection()
{
    if (!closed)
    {
        CloseHandle(hSerial);
        closed = false;
    }
}


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        CloseConnection();
        exit(0);
        return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        CloseConnection();
        exit(0);
        return TRUE;

        // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        CloseConnection();
        exit(0);
        return FALSE;

    case CTRL_LOGOFF_EVENT:
        CloseConnection();
        exit(0);
        return FALSE;

    case CTRL_SHUTDOWN_EVENT:
        CloseConnection();
        exit(0);
        return FALSE;

    default:
        CloseConnection();
        exit(0);
        return FALSE;
    }
}



int main()
{
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };

    SetConsoleCtrlHandler(CtrlHandler, TRUE);
    // Open COM port
    hSerial = CreateFile(L"COM5", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to open COM port\n";
        return 1;
    }

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        std::cout << "Error getting COM port parameters\n";
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        std::cout << "Error setting COM port parameters\n";
        return 1;
    }

    // Checking the connection
    if (!isConnected(hSerial))
    {
        std::cout << "Connection not established\n";
        return 1;
    }
    std::cout << "Connection established\n";

    // Reading data from COM port
    DWORD bytesRead;
    char incomingData[2];
    while (1)
    {
        ReadFile(hSerial, incomingData, sizeof(incomingData), &bytesRead, NULL);
        if (bytesRead > 0)
        {
            std::string receivedMsg(incomingData, bytesRead);
            std::cout << std::endl << receivedMsg << std::endl;
            if (receivedMsg == "Al")
            {
                ShellExecute(0, 0, L"https://en.wikipedia.org/wiki/Punic_Wars", 0, 0, SW_SHOW);
                break;
            }
            

        }

    }

    // Close com port
    CloseConnection();
    

    return 0;
}
