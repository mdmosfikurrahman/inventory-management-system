#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

#define USER_FILE "users.txt"
#define INVENTORY_FILE "inventory.dat"

char loggedInUsername[50];

struct User {
    char username[50];
    char email[100];
    char password[50];
};

struct Item {
    int id;
    char name[30];
    char quantity[20];
   char price[20];
   char username[50];
};

void inventoryMenu();
void addItem();
void displayItems();
void searchItem();
void editItem();
void deleteItem();
int registerUser();
int loginUser();
int isUsernameTaken(const char *username);
int isEmailTaken(const char *email);
int isValidEmail(const char *email);
int containsCapital(const char *username);
int containsSpace(const char *username);
int isStrongPassword(const char *password);
void getHiddenPassword(char *password, int maxLength);

void getHiddenPassword(char *password, int maxLength) {
    int index = 0;
    char ch;
    while ((ch = _getch()) != '\r' && index < maxLength - 1) {
        if (ch == '\b') {
            if (index > 0) {
                index--;
                printf("\b \b");
            }
        } else if (isprint(ch)) {
            password[index++] = ch;
            printf("*");
        }
    }
    password[index] = '\0';
    printf("\n");
}

int isUsernameTaken(const char *username) {
    struct User temp;
    FILE *file = fopen(USER_FILE, "r");
    if (!file) return 0;

    while (fscanf(file, "%s %s %s", temp.username, temp.email, temp.password) == 3) {
        if (strcmp(username, temp.username) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int isEmailTaken(const char *email) {
    struct User temp;
    FILE *file = fopen(USER_FILE, "r");
    if (!file) return 0;

    while (fscanf(file, "%s %s %s", temp.username, temp.email, temp.password) == 3) {
        if (strcmp(email, temp.email) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int isValidEmail(const char *email) {
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');

    if (!at || !dot || at >= dot || strchr(email, ' ') != NULL) {
        return 0;
    }

    int atIndex = at - email;
    int dotIndex = dot - email;
    if (atIndex < 1 || dotIndex - atIndex < 2 || strlen(email) - dotIndex <= 1) {
        return 0;
    }

    return 1;
}

int containsCapital(const char *username) {
    for (int i = 0; username[i] != '\0'; i++) {
        if (isupper(username[i])) return 1;
    }
    return 0;
}

int containsSpace(const char *username) {
    for (int i = 0; username[i] != '\0'; i++) {
        if (username[i] == ' ') return 1;
    }
    return 0;
}

int isStrongPassword(const char *password) {
    int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0;
    int length = strlen(password);

    if (length < 8) return 0;

    for (int i = 0; i < length; i++) {
        if (isupper(password[i])) hasUpper = 1;
        if (islower(password[i])) hasLower = 1;
        if (isdigit(password[i])) hasDigit = 1;
        if (strchr("!@#$%^&*()-_=+[]{}|;:'\",.<>?/", password[i])) hasSpecial = 1;
    }

    return (hasUpper && hasLower && hasDigit && hasSpecial);
}

int registerUser() {
    struct User user;
    FILE *file = fopen(USER_FILE, "a");
    if (!file) {
        printf("Error opening file!\n");
        return 0;
    }

    while (1) {
        printf("Enter username (lowercase, use underscore instead of space): ");
        fgets(user.username, sizeof(user.username), stdin);
        user.username[strcspn(user.username, "\n")] = 0;

        if (containsCapital(user.username)) {
            printf("Username cannot contain capital letters.\n");
            continue;
        }
        if (containsSpace(user.username)) {
            printf("Username cannot contain spaces.\n");
            continue;
        }
        if (isUsernameTaken(user.username)) {
            printf("Username already exists.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Enter email: ");
        fgets(user.email, sizeof(user.email), stdin);
        user.email[strcspn(user.email, "\n")] = 0;

        if (!isValidEmail(user.email)) {
            printf("Invalid email format! Use format like: name@example.com\n\n");
            continue;
        }
        if (isEmailTaken(user.email)) {
            printf("Email already exists.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Enter password (min 8 chars, 1 uppercase, 1 lowercase, 1 digit, 1 special char): ");
        getHiddenPassword(user.password, sizeof(user.password));

        if (!isStrongPassword(user.password)) {
            printf("Password not strong enough.\n");
            continue;
        }
        break;
    }

    fprintf(file, "%s %s %s\n", user.username, user.email, user.password);
    fclose(file);
    printf("Registration successful!\n");
    return 1;
}

int loginUser() {
    struct User user, temp;
    FILE *file = fopen(USER_FILE, "r");
    if (!file) {
        printf("Error opening file!\n");
        return 0;
    }

    printf("Enter username: ");
    fgets(user.username, sizeof(user.username), stdin);
    user.username[strcspn(user.username, "\n")] = 0;

    printf("Enter password: ");
    getHiddenPassword(user.password, sizeof(user.password));

    while (fscanf(file, "%s %s %s", temp.username, temp.email, temp.password) == 3) {
        if (strcmp(user.username, temp.username) == 0 &&
            strcmp(user.password, temp.password) == 0) {
            strcpy(loggedInUsername, user.username);
            fclose(file);
            printf("Login successful! Welcome %s.\n", user.username);
            return 1;
        }
    }

    fclose(file);
    printf("Invalid credentials!\n");
    return 0;
}

void inventoryMenu() {
    int choice;
    while (1) {
        printf("\n===== WELCOME TO INVENTORY MANAGEMENT SYSTEM =====\n");
        printf("1. Add New Item\n");
        printf("2. Display Items\n");
        printf("3. Search Item\n");
        printf("4. Edit Item\n");
        printf("5. Delete Item\n");
        printf("6. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: addItem(); break;
            case 2: displayItems(); break;
            case 3: searchItem(); break;
            case 4: editItem(); break;
            case 5: deleteItem(); break;
            case 6:  return;
            default: printf("Invalid choice. Try again.\n");
        }
    }
}

void addItem() {
    struct Item item,temp;
    FILE *fp = fopen(INVENTORY_FILE, "ab+");
    if (!fp) {
        printf("Error opening inventory file!\n");
        return;
    }

    printf("Enter Item ID: ");
    scanf("%d", &item.id);
    getchar();
    while (fread(&temp, sizeof(struct Item), 1, fp)) {
        if (temp.id == item.id && strcmp(temp.username, loggedInUsername) == 0) {
            printf("Duplicate ID detected in your inventory.\n");
            fclose(fp);
            return;
        }
    }
    printf("Enter Item Name: ");
    scanf("%s", item.name);
    printf("Enter Quantity: ");
    scanf("%s", item.quantity);
    printf("Enter Price: ");
    scanf("%s", item.price);

    strcpy(item.username, loggedInUsername);

    fwrite(&item, sizeof(struct Item), 1, fp);
    fclose(fp);
    printf("Item added successfully!\n");
}

void displayItems() {
    struct Item item;
    struct Item items[100]; // Adjust the size based on max expected items
    int count = 0;
    FILE *fp = fopen(INVENTORY_FILE, "rb");

    if (!fp) {
        printf("Error opening inventory file!\n");
        return;
    }

    // Read and collect items belonging to the logged-in user
    while (fread(&item, sizeof(struct Item), 1, fp)) {
        if (strcmp(item.username, loggedInUsername) == 0) {
            items[count++] = item;
        }
    }
    fclose(fp);

    if (count == 0) {
        printf("No items found. Please add items first.\n");
        return;
    }

    // Sort the array by item.id (ascending order)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (items[i].id > items[j].id) {
                struct Item temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }

    // Display sorted items
    printf("\nID\tName\tQuantity\tPrice\n");
    printf("--------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%d\t%s\t%s\t\t%s\n", items[i].id, items[i].name, items[i].quantity, items[i].price);
    }
}


void searchItem() {
    struct Item item;
    int id, found = 0, hasData = 0;

    FILE *fp = fopen(INVENTORY_FILE, "rb");
    if (!fp) {
        printf("Error opening inventory file!\n");
        return;
    }

    while (fread(&item, sizeof(struct Item), 1, fp)) {
        hasData = 1;
        break;
    }
    if (!hasData) {
        printf("No items found. Please add items first.\n");
        fclose(fp);
        return;
    }

    rewind(fp);

    printf("Enter Item ID to search: ");
    scanf("%d", &id);

    while (fread(&item, sizeof(struct Item), 1, fp)) {
        if (item.id == id && strcmp(item.username,loggedInUsername)== 0) {
            printf("Item Found \n");
            printf("ID: %d\nName: %s\nQuantity: %s\nPrice: %s\n",
                   item.id, item.name, item.quantity, item.price);
            found = 1;
            break;
        }
    }

    if (!found) printf("Item not found!\n");
    fclose(fp);
}

void editItem() {
    struct Item item;
    int id, found = 0;
    FILE *fp = fopen(INVENTORY_FILE, "rb+");
    if (!fp) {
        printf("Error opening inventory file!\n");
        return;
    }

    int hasData = 0;

    while (fread(&item, sizeof(struct Item), 1, fp)) {
        if (strcmp(item.username, loggedInUsername) == 0) {
            hasData = 1;
            break;
        }
    }

    if (!hasData) {
        printf("No items found. Please add items first.\n");
        fclose(fp);
        return;
    }

    rewind(fp);

    printf("\nID\tName\tQuantity\tPrice\n");
    printf("--------------------------------------------\n");
    while (fread(&item, sizeof(struct Item), 1, fp)) {
        if (strcmp(item.username, loggedInUsername) == 0) {
            printf("%d\t%s\t%s\t\t%s\n", item.id, item.name, item.quantity, item.price);
        }
    }

    printf("Enter Item ID to edit: ");
    scanf("%d", &id);
    rewind(fp);

    while (fread(&item, sizeof(struct Item), 1, fp)) {
        if (item.id == id && strcmp(item.username, loggedInUsername) == 0) {
            printf("Enter new name: ");
            scanf("%s", item.name);
            printf("Enter new quantity: ");
            scanf("%s", item.quantity);
            printf("Enter new price: ");
            scanf("%s", item.price);

            fseek(fp, -sizeof(struct Item), SEEK_CUR);
            fwrite(&item, sizeof(struct Item), 1, fp);
            found = 1;
            break;
        }
    }

    if (found) {
        printf("Item updated successfully!\n");
    } else {
        printf("Item not found or you don't have permission to edit it.\n");
    }

    fclose(fp);
}


void deleteItem() {
    struct Item item;
    int id, found = 0,hasData=0;
    FILE *fp = fopen(INVENTORY_FILE, "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (!fp || !temp) {
        printf("Error opening file!\n");
        return;
    }

    while (fread(&item, sizeof(struct Item), 1, fp)) {
        hasData = 1;
        break;
    }
    if (!hasData) {
        printf("No items found. Please add items first.\n");
        fclose(fp);
        return;
    }

    rewind(fp);
    printf("\nID\tName\tQuantity\tPrice\n");
    printf("--------------------------------------------\n");
    while (fread(&item, sizeof(struct Item), 1, fp)) {
        if(strcmp(item.username,loggedInUsername) != 0 ){
            continue;
        }    
        printf("%d\t%s\t%s\t\t%s\n", item.id, item.name, item.quantity, item.price);
    }
    printf("Enter Item ID to delete: ");
    scanf("%d", &id);

    while (fread(&item, sizeof(struct Item), 1, fp)) {
        if (item.id != id || strcmp(item.username,loggedInUsername)!=0) {
            fwrite(&item, sizeof(struct Item), 1, temp);
        } else {
            found = 1;
        }
    }

    fclose(fp);
    fclose(temp);

    remove(INVENTORY_FILE);
    rename("temp.dat", INVENTORY_FILE);

    if (found) {
        printf("Item deleted successfully!\n");
    } else {
        printf("Item not found!\n");
    }
}

int main() {
    int choice;
    while (1) {
        printf("\n===== WELCOME TO INVENTORY MANAGEMENT SYSTEM=====\n");
        printf("\n===== USER AUTHENTICATION SYSTEM =====\n");
        printf("\n1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                if (registerUser()) {
                    inventoryMenu();
                }
                break;
            case 2:
                if (loginUser()) {
                    inventoryMenu();
                }
                break;
            case 3:
                printf("Goodbye!\n");
                exit(0);
            default:
                printf("Invalid option!\n");
        }
    }

    return 0;
}