#ifndef SUSPENDRESUME_H
#define SUSPENDRESUME_H

#include "pch.h"
#include <TlHelp32.h>

namespace ThreadManager {

	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	bool Suspend()
	{
		hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hThreadSnap)
		{
			te32.dwSize = sizeof(THREADENTRY32);

			if (!Thread32First(hThreadSnap, &te32))
			{
				CloseHandle(hThreadSnap);
				return 0;
			}

			do
			{
				if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId()) //Ignore threads from other processes AND the own thread of course
				{
					HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, 0, te32.th32ThreadID);
					if (hThread)
					{
						CONTEXT context;
						context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
						SuspendThread(hThread); //Suspend the thread so we can safely set a breakpoint

						//ResumeThread(hThread);
						CloseHandle(hThread);
					}
				}
			} while (Thread32Next(hThreadSnap, &te32));
		}
		CloseHandle(hThreadSnap);
		return 0;
	}

	bool Resume()
	{
		hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hThreadSnap)
		{
			te32.dwSize = sizeof(THREADENTRY32);

			if (!Thread32First(hThreadSnap, &te32))
			{
				CloseHandle(hThreadSnap);
				return 0;
			}

			do
			{
				if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId()) // Ignore threads from other processes AND the own thread of course
				{
					HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, 0, te32.th32ThreadID);
					if (hThread)
					{
						CONTEXT context;
						context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

						ResumeThread(hThread);
						CloseHandle(hThread);
					}
				}
			} while (Thread32Next(hThreadSnap, &te32));
			CloseHandle(hThreadSnap);
		}
		return 0;
	}
}


#endif SUSPENDRESUME_H