#include <windows.h>
#include <stdio.h>

#define THREADCOUNT 1 

HANDLE ghWriteEvent;
HANDLE ghThreads[THREADCOUNT];

DWORD WINAPI ThreadProc(LPVOID);

void CreateEventsAndThreads(void)
{
    int i;
    DWORD dwThreadID;

    // Create a manual-reset event object. The write thread sets this
    // object to the signaled state when it finishes writing to a 
    // shared buffer. 

    ghWriteEvent = CreateEvent(
        NULL,               // default security attributes
        TRUE,               // manual-reset event
        FALSE,              // initial state is nonsignaled
        TEXT("WriteEvent")  // object name
    );

    if (ghWriteEvent == NULL)
    {
        printf("CreateEvent failed (%d)\n", GetLastError());
        return;
    }

    // Create multiple threads to read from the buffer.

    for (i = 0; i < THREADCOUNT; i++)
    {
        // TODO: More complex scenarios may require use of a parameter
        //   to the thread procedure, such as an event per thread to  
        //   be used for synchronization.
        ghThreads[i] = CreateThread(
            NULL,              // default security
            0,                 // default stack size
            ThreadProc,        // name of the thread function
            NULL,              // no thread parameters
            0,                 // default startup flags
            &dwThreadID);

        if (ghThreads[i] == NULL)
        {
            printf("CreateThread failed (%d)\n", GetLastError());
            return;
        }
    }
}

void CloseEvents()
{
    // Close all event handles (currently, only one global handle).

    CloseHandle(ghWriteEvent);
}

int main(void)
{
    DWORD dwWaitResult;

    // TODO: Create the shared buffer

    CreateEventsAndThreads();

    Sleep(2000);

    printf("Thread %d sets event...\n", GetCurrentThreadId());
    BOOL ret = SetEvent(ghWriteEvent);

    printf("ret = %d\n", ret);

    Sleep(1000);

    printf("Thread %d sets event...\n", GetCurrentThreadId());
    ret = SetEvent(ghWriteEvent);

    printf("ret = %d\n", ret);

    while (true) {};
    
    CloseEvents();

    return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
    // lpParam not used in this example.
    UNREFERENCED_PARAMETER(lpParam);

    DWORD dwWaitResult;

    printf("Thread %d waiting for write event...\n", GetCurrentThreadId());

    dwWaitResult = WaitForSingleObject(
        ghWriteEvent, // event handle
        INFINITE);    // indefinite wait


    printf("WaitForSingleObject returns %d\n", dwWaitResult);

    while (true) {};
    printf("Thread %d exiting\n", GetCurrentThreadId());
    return 1;
}
