#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BOATS 120 // Max num of boats allowed
#define MAX_NAME_LENGTH 128 // Max length of boat names

typedef enum { // Different boat locations
    slip,
    land,
    trailor,
    storage,
    no_place
} PlaceType;

typedef union {
    int slipNumber;       // For slips
    char bayLetter;       // For land bays
    char trailorTag[10];  // For trailors
    int storageSpace;     // For storage
} PlaceInfo;

typedef struct { // Boat info
    char name[MAX_NAME_LENGTH]; // Boat name
    float length;               // Boat length
    PlaceType place;            // Place type
    PlaceInfo info;             // Extra info
    float amountOwed;           // Money owed
} Boat;

// Store pointers for boats and counter for num of boats
Boat *boats[MAX_BOATS];
int boatCount = 0;

// Convert string to PlaceType enum
PlaceType StringToPlaceType(char *PlaceString) {
    if (!strcasecmp(PlaceString, "slip")) return slip;
    if (!strcasecmp(PlaceString, "land")) return land;
    if (!strcasecmp(PlaceString, "trailor")) return trailor;
    if (!strcasecmp(PlaceString, "storage")) return storage;
    return no_place;
}

// Convert PLaceType enum to string rep
char *PlaceToString(PlaceType place) {
    switch (place) {
        case slip: return "slip";
        case land: return "land";
        case trailor: return "trailor";
        case storage: return "storage";
        default: return "no_place";
    }
}

// File I/O Functions
// Load boat data from BoatData.csv
void LoadDataFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file.\n"); // Error message
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (boatCount >= MAX_BOATS) { // Check if marina is full
            printf("Marina is full!\n");
            break;
        }
        Boat *newBoat = (Boat *)malloc(sizeof(Boat));
        char placeType[20], extraInfo[20];
        sscanf(line, "%127[^,],%f,%19[^,],%19[^,],%f",
               newBoat->name, &newBoat->length, placeType, extraInfo, &newBoat->amountOwed);

        newBoat->place = StringToPlaceType(placeType);
        switch (newBoat->place) {
            case slip:
                newBoat->info.slipNumber = atoi(extraInfo);
                break;
            case land:
                newBoat->info.bayLetter = extraInfo[0];
                break;
            case trailor:
                strcpy(newBoat->info.trailorTag, extraInfo);
                break;
            case storage:
                newBoat->info.storageSpace = atoi(extraInfo);
                break;
            default:
                break;
        }

        boats[boatCount++] = newBoat; // Add new boat to array
    }
    fclose(file); // Close file
}

// Save boat data to BoatData.csv
void SaveDataToFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Failed to save data.\n"); // Error message
        return;
    }

    // Write boat data into file
    for (int i = 0; i < boatCount; i++) {
        Boat *boat = boats[i];

        fprintf(file, "%s,%.2f,%s,", boat->name, boat->length, PlaceToString(boat->place));

        switch (boat->place) { // Location of boat and tag/bay/slip num
            case slip:
                fprintf(file, "%d,", boat->info.slipNumber);
                break;
            case land:
                fprintf(file, "%c,", boat->info.bayLetter);
                break;
            case trailor:
                fprintf(file, "%s,", boat->info.trailorTag);
                break;
            case storage:
                fprintf(file, "%d,", boat->info.storageSpace);
                break;
            default:
                fprintf(file, "N/A,");
                break;
        }

        fprintf(file, "%.2f\n", boat->amountOwed); // Display amount owed
    }

    fclose(file); // Close file
}


// Menu Option Functions
// Print the inventory of the marina
void PrintInventory() {
    // Sort the boats array alphabetically
    for (int i = 0; i < boatCount - 1; i++) {
        for (int j = 0; j < boatCount - i - 1; j++) {
            if (strcasecmp(boats[j]->name, boats[j + 1]->name) > 0) {
                Boat *temp = boats[j];
                boats[j] = boats[j + 1];
                boats[j + 1] = temp;
            }
        }
    }

    // Print the sorted inventory alphabetically
    for (int i = 0; i < boatCount; i++) {
        Boat *boat = boats[i];
        
        // Formatting
        printf("%-20s %4.0f' %-7s ", boat->name, boat->length, PlaceToString(boat->place));

        switch (boat->place) {
            case slip:
                printf("# %-3d   ", boat->info.slipNumber);
                break;
            case land:
                printf("   %c    ", boat->info.bayLetter);
                break;
            case trailor:
                printf("%-7s ", boat->info.trailorTag);
                break;
            case storage:
                printf("# %-3d   ", boat->info.storageSpace);
                break;
            default:
                printf("N/A       ");
                break;
        }

        printf("Owes $%7.2f\n", boat->amountOwed);
    }
}


// Add a new boat to the inventory
void AddBoat(char *csvData) {
    if (boatCount >= MAX_BOATS) {
        printf("Marina is full!\n");
        return;
    }

    Boat *newBoat = (Boat *)malloc(sizeof(Boat));
    char placeType[20], extraInfo[20];
    sscanf(csvData, "%127[^,],%f,%19[^,],%19[^,],%f",
           newBoat->name, &newBoat->length, placeType, extraInfo, &newBoat->amountOwed);

    newBoat->place = StringToPlaceType(placeType);
    switch (newBoat->place) {
        case slip:
            newBoat->info.slipNumber = atoi(extraInfo);
            break;
        case land:
            newBoat->info.bayLetter = extraInfo[0];
            break;
        case trailor:
            strcpy(newBoat->info.trailorTag, extraInfo);
            break;
        case storage:
            newBoat->info.storageSpace = atoi(extraInfo);
            break;
        default:
            break;
    }

    boats[boatCount++] = newBoat; // Add new boat to array
}

// Remove boat from inventory by name
void RemoveBoat(char *name) {
    for (int i = 0; i < boatCount; i++) {
        if (!strcasecmp(boats[i]->name, name)) {
            free(boats[i]);
            boats[i] = boats[--boatCount];
            return;
        }
    }
    printf("No boat with that name\n"); // If there is no boat with input name
}

// Make payment to boat marina
void AcceptPayment(char *name, float amount) {
    for (int i = 0; i < boatCount; i++) {
        if (!strcasecmp(boats[i]->name, name)) {
            if (amount > boats[i]->amountOwed) { // If input amount is greater than owed
                printf("That is more than the amount owed, $%.2f\n", boats[i]->amountOwed);
                return;
            }
            boats[i]->amountOwed -= amount;
            return;
        }
    }
    printf("No boat with that name\n"); // No boat name found
}

// Update monthly charge
void UpdateMonthlyCharges() {
    for (int i = 0; i < boatCount; i++) {
        switch (boats[i]->place) {
            case slip:
                boats[i]->amountOwed += boats[i]->length * 12.50;
                break;
            case land:
                boats[i]->amountOwed += boats[i]->length * 14.00;
                break;
            case trailor:
                boats[i]->amountOwed += boats[i]->length * 25.00;
                break;
            case storage:
                boats[i]->amountOwed += boats[i]->length * 11.20;
                break;
            default:
                break;
        }
    }
}

// Main Function
int main(int argc, char *argv[]) {
    printf("Welcome to the Boat Management System\n");
    printf("-------------------------------------\n");

    if (argc != 2) {
        printf("Usage: %s <BoatData.csv>\n", argv[0]);
        return EXIT_FAILURE;
    }

    LoadDataFromFile(argv[1]);
    char choice, line[256];

    while (1) {
        printf("\n(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it: ");
        scanf(" %c", &choice);
        switch (tolower(choice)) {
            case 'i':
                PrintInventory();
                break;
            case 'a':
                printf("Please enter the boat data in CSV format: ");
                scanf(" %[^\n]", line);
                AddBoat(line);
                break;
            case 'r':
                printf("Please enter the boat name: ");
                scanf(" %[^\n]", line);
                RemoveBoat(line);
                break;
            case 'p': {
                printf("Please enter the boat name: ");
                scanf(" %[^\n]", line);

                int found = 0; // Check if the boat exists
                for (int i = 0; i < boatCount; i++) {
                    if (!strcasecmp(boats[i]->name, line)) {
                        found = 1;

                        // Ask for payment amount only if the boat exists
                        printf("Please enter the amount to be paid: ");
                        float amount;
                        scanf("%f", &amount);

                        if (amount > boats[i]->amountOwed) {
                            printf("That is more than the amount owed, $%.2f\n", boats[i]->amountOwed);
                        } else {
                            boats[i]->amountOwed -= amount;
                        }

                        break;
                    }
                }

                if (!found) {
                    printf("No boat with that name\n"); 
                }
                break;
            }
            case 'm':
                UpdateMonthlyCharges();
                break;
            case 'x':
                SaveDataToFile(argv[1]);
                printf("Exiting the Boat Management System\n"); // Display message when user exits
                return EXIT_SUCCESS;
            default:
                printf("Invalid option '%c'\n", choice);
                break;
        }
    }
}
