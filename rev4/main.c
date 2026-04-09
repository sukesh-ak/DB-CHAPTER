#include <stdio.h>
#include "app/account_service.h"

static int show_menu(void) {
    int choice;
    printf("\n--- Bank Account System ---\n"
           "1 - List all accounts\n"
           "2 - Update an account\n"
           "3 - Add a new account\n"
           "4 - Delete an account\n"
           "5 - Exit\n"
           "? ");
    scanf("%d", &choice);
    return choice;
}

int main(void) {
    if (account_service_init() != 0) {
        printf("Failed to initialize. Exiting.\n");
        return 1;
    }

    int choice;
    while ((choice = show_menu()) != 5) {
        switch (choice) {
            case 1: account_service_list();   break;
            case 2: account_service_update(); break;
            case 3: account_service_create(); break;
            case 4: account_service_delete(); break;
            default: puts("Invalid choice."); break;
        }
    }

    account_service_cleanup();
    printf("Goodbye!\n");
    return 0;
}
