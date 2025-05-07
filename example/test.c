// OpenHL - Open Hook Library
// This library is licensed under the GPL 3.0 license
#include <stdio.h>
#include <openhl.h>

// Original function that will be hooked
void original()
{
    printf("original function called\n");
}

// Hooked function that replaces the original
void hooked()
{
    printf("hooked function called\n");
}

int main()
{
    // Initialize Open Hook Library
    if (OpenHL_Init() != OPENHL_OK) {
        fprintf(stderr, "Failed to init OpenHL\n");
        return -1;
    }

    // Install hook to replace the original function with the hooked function
    if (OpenHL_InstallHook((void*)original, (void*)hooked) != OPENHL_OK) {
        fprintf(stderr, "Failed to install hook\n");
        return -1;
    }

    // Call the original function (will actually call the hooked function)
    original(); // Output: hooked function called

    // Remove the hook, restoring the original function
    if (OpenHL_RemoveHook((void*)original) != OPENHL_OK) {
        fprintf(stderr, "Failed to remove hook\n");
        return -1;
    }

    // Call the original function again (without hook)
    original(); // Output: original function called

    // Shutdown the library
    OpenHL_Shutdown(); 

    return 0;
}

/*
Output:
hooked function called
original function called
*/
