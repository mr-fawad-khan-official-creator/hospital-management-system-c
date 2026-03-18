#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ============================================================
   Hospital Management System
   - Binary file persistence
   - Input validation
   - Duplicate ID checks
   - Delete / Update operations
   - Case-insensitive city search
   ============================================================ */

#define HOSP_FILE     "hospitals.dat"
#define PAT_FILE      "patients.dat"
#define MAX_RECORDS   100

/* ──────────────── Structures ──────────────── */

typedef struct {
    char  city[30];
    char  name[40];
    int   id;
    int   availableBeds;
    float bedPrice;
    float rating;
    int   reviews;
} Hospital;

typedef struct {
    char name[40];
    int  id;
    int  age;
    char disease[40];
    int  hospitalID;
} Patient;

/* ──────────────── Utility Helpers ──────────────── */

/* Flush leftover characters from stdin */
static void flushInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Read a trimmed string safely (replaces scanf %[^\n]) */
static void readString(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, size, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';   /* strip newline */
    }
}

/* Read an integer with basic validation */
static int readInt(const char *prompt) {
    int val;
    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        if (scanf("%d", &val) == 1) { flushInput(); return val; }
        printf("  Invalid input. Please enter a whole number.\n");
        flushInput();
    }
}

/* Read a float with basic validation */
static float readFloat(const char *prompt) {
    float val;
    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        if (scanf("%f", &val) == 1) { flushInput(); return val; }
        printf("  Invalid input. Please enter a number.\n");
        flushInput();
    }
}

/* Case-insensitive string comparison */
static int strcasecmpPortable(const char *a, const char *b) {
    while (*a && *b) {
        int diff = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (diff != 0) return diff;
        a++; b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* ──────────────── File Helpers ──────────────── */

static int loadHospitals(Hospital h[]) {
    FILE *fp = fopen(HOSP_FILE, "rb");
    int count = 0;
    if (!fp) return 0;
    while (count < MAX_RECORDS && fread(&h[count], sizeof(Hospital), 1, fp) == 1)
        count++;
    fclose(fp);
    return count;
}

static int saveHospitals(Hospital h[], int n) {
    FILE *fp = fopen(HOSP_FILE, "wb");
    if (!fp) { printf("  File error! Could not save hospitals.\n"); return 0; }
    fwrite(h, sizeof(Hospital), n, fp);
    fclose(fp);
    return 1;
}

static int loadPatients(Patient p[]) {
    FILE *fp = fopen(PAT_FILE, "rb");
    int count = 0;
    if (!fp) return 0;
    while (count < MAX_RECORDS && fread(&p[count], sizeof(Patient), 1, fp) == 1)
        count++;
    fclose(fp);
    return count;
}

static int savePatients(Patient p[], int n) {
    FILE *fp = fopen(PAT_FILE, "wb");
    if (!fp) { printf("  File error! Could not save patients.\n"); return 0; }
    fwrite(p, sizeof(Patient), n, fp);
    fclose(fp);
    return 1;
}

static Hospital findHospitalByID(int id) {
    Hospital h[MAX_RECORDS];
    int n = loadHospitals(h);
    for (int i = 0; i < n; i++)
        if (h[i].id == id) return h[i];
    Hospital notFound = {0};
    notFound.id = -1;
    return notFound;
}

static int hospitalIDExists(int id) {
    return findHospitalByID(id).id != -1;
}

static int patientIDExists(int id) {
    Patient p[MAX_RECORDS];
    int n = loadPatients(p);
    for (int i = 0; i < n; i++)
        if (p[i].id == id) return 1;
    return 0;
}

/* ──────────────── Display Helpers ──────────────── */

static void printHospital(const Hospital *h) {
    printf("  ┌─────────────────────────────────────┐\n");
    printf("  │ %-36s│\n", h->name);
    printf("  ├─────────────────────────────────────┤\n");
    printf("  │ ID       : %-25d│\n", h->id);
    printf("  │ City     : %-25s│\n", h->city);
    printf("  │ Beds     : %-25d│\n", h->availableBeds);
    printf("  │ Price/Day: $%-24.2f│\n", h->bedPrice);
    printf("  │ Rating   : %.1f/5.0 (%d reviews)%-6s│\n",
           h->rating, h->reviews, "");
    printf("  └─────────────────────────────────────┘\n");
}

static void printPatient(const Patient *p, const Hospital *h) {
    printf("  ┌─────────────────────────────────────┐\n");
    printf("  │ %-36s│\n", p->name);
    printf("  ├─────────────────────────────────────┤\n");
    printf("  │ ID       : %-25d│\n", p->id);
    printf("  │ Age      : %-25d│\n", p->age);
    printf("  │ Disease  : %-25s│\n", p->disease);
    printf("  │ Hospital : %-25s│\n", (h->id == -1) ? "Unknown" : h->name);
    printf("  └─────────────────────────────────────┘\n");
}

/* ──────────────── Hospital Functions ──────────────── */

static void addHospital(void) {
    Hospital h[MAX_RECORDS];
    int n = loadHospitals(h);

    if (n >= MAX_RECORDS) {
        printf("  Maximum hospital capacity reached (%d).\n", MAX_RECORDS);
        return;
    }

    Hospital nh = {0};

    /* ID with duplicate check */
    while (1) {
        nh.id = readInt("  Hospital ID: ");
        if (nh.id <= 0)      { printf("  ID must be a positive number.\n"); continue; }
        if (hospitalIDExists(nh.id)) { printf("  ID %d already exists. Choose another.\n", nh.id); continue; }
        break;
    }

    readString("  City: ",          nh.city,    sizeof(nh.city));
    readString("  Hospital Name: ", nh.name,    sizeof(nh.name));

    while ((nh.availableBeds = readInt("  Available Beds: ")) < 0)
        printf("  Beds cannot be negative.\n");

    while ((nh.bedPrice = readFloat("  Bed Price per Day ($): ")) < 0)
        printf("  Price cannot be negative.\n");

    while (1) {
        nh.rating = readFloat("  Rating (0.0 – 5.0): ");
        if (nh.rating >= 0.0f && nh.rating <= 5.0f) break;
        printf("  Rating must be between 0.0 and 5.0.\n");
    }

    while ((nh.reviews = readInt("  Number of Reviews: ")) < 0)
        printf("  Reviews cannot be negative.\n");

    h[n++] = nh;
    if (saveHospitals(h, n))
        printf("  Hospital '%s' added successfully!\n", nh.name);
}

static void displayHospitals(void) {
    Hospital h[MAX_RECORDS];
    int n = loadHospitals(h);
    if (n == 0) { printf("  No hospital records found.\n"); return; }
    printf("\n  === All Hospitals (%d) ===\n", n);
    for (int i = 0; i < n; i++) printHospital(&h[i]);
}

static void displayHospitalByCity(void) {
    Hospital h[MAX_RECORDS];
    int n = loadHospitals(h);
    char city[30];
    readString("  Enter city name: ", city, sizeof(city));

    int found = 0;
    for (int i = 0; i < n; i++) {
        if (strcasecmpPortable(h[i].city, city) == 0) {
            printHospital(&h[i]);
            found++;
        }
    }
    if (!found) printf("  No hospitals found in '%s'.\n", city);
    else        printf("  %d hospital(s) found in '%s'.\n", found, city);
}

static void updateHospital(void) {
    Hospital h[MAX_RECORDS];
    int n = loadHospitals(h);
    if (n == 0) { printf("  No hospital records found.\n"); return; }

    int id = readInt("  Enter Hospital ID to update: ");
    for (int i = 0; i < n; i++) {
        if (h[i].id == id) {
            printf("  Updating '%s'. Press Enter to keep current value.\n", h[i].name);

            char buf[40];
            readString("  New City (current: " , buf, sizeof(buf));
            /* If user typed something, update; otherwise keep old */
            /* We reuse buf for each field */

            /* City */
            printf("  New City [%s]: ", h[i].city);
            fflush(stdout);
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = '\0';
            if (strlen(buf) > 0) strncpy(h[i].city, buf, sizeof(h[i].city) - 1);

            /* Name */
            printf("  New Name [%s]: ", h[i].name);
            fflush(stdout);
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = '\0';
            if (strlen(buf) > 0) strncpy(h[i].name, buf, sizeof(h[i].name) - 1);

            /* Beds */
            printf("  New Available Beds [%d] (0 to keep): ", h[i].availableBeds);
            int beds = readInt("");
            if (beds > 0) h[i].availableBeds = beds;

            /* Price */
            printf("  New Bed Price [%.2f] (0 to keep): ", h[i].bedPrice);
            float price = readFloat("");
            if (price > 0) h[i].bedPrice = price;

            /* Rating */
            printf("  New Rating [%.1f] (-1 to keep): ", h[i].rating);
            float rating = readFloat("");
            if (rating >= 0.0f && rating <= 5.0f) h[i].rating = rating;

            if (saveHospitals(h, n))
                printf("  Hospital updated successfully!\n");
            return;
        }
    }
    printf("  Hospital with ID %d not found.\n", id);
}

static void deleteHospital(void) {
    Hospital h[MAX_RECORDS];
    int n = loadHospitals(h);
    if (n == 0) { printf("  No hospital records found.\n"); return; }

    int id = readInt("  Enter Hospital ID to delete: ");
    for (int i = 0; i < n; i++) {
        if (h[i].id == id) {
            printf("  Deleting '%s'. Are you sure? (1=Yes / 0=No): ", h[i].name);
            int confirm = readInt("");
            if (confirm != 1) { printf("  Deletion cancelled.\n"); return; }
            /* Shift records left */
            for (int j = i; j < n - 1; j++) h[j] = h[j + 1];
            n--;
            if (saveHospitals(h, n))
                printf("  Hospital deleted successfully!\n");
            return;
        }
    }
    printf("  Hospital with ID %d not found.\n", id);
}

/* ──────────────── Patient Functions ──────────────── */

static void addPatient(void) {
    Patient p[MAX_RECORDS];
    int n = loadPatients(p);

    if (n >= MAX_RECORDS) {
        printf("  Maximum patient capacity reached (%d).\n", MAX_RECORDS);
        return;
    }

    Patient np = {0};

    while (1) {
        np.id = readInt("  Patient ID: ");
        if (np.id <= 0)         { printf("  ID must be a positive number.\n"); continue; }
        if (patientIDExists(np.id)) { printf("  ID %d already exists.\n", np.id); continue; }
        break;
    }

    readString("  Patient Name: ", np.name,    sizeof(np.name));

    while ((np.age = readInt("  Age: ")) <= 0 || np.age > 150)
        printf("  Please enter a valid age (1-150).\n");

    readString("  Disease: ",      np.disease, sizeof(np.disease));

    while (1) {
        np.hospitalID = readInt("  Hospital ID: ");
        if (hospitalIDExists(np.hospitalID)) break;
        printf("  Hospital ID %d does not exist. Please enter a valid Hospital ID.\n", np.hospitalID);
    }

    p[n++] = np;
    if (savePatients(p, n))
        printf("  Patient '%s' added successfully!\n", np.name);
}

static void displayPatients(void) {
    Patient p[MAX_RECORDS];
    int n = loadPatients(p);
    if (n == 0) { printf("  No patient records found.\n"); return; }
    printf("\n  === All Patients (%d) ===\n", n);
    for (int i = 0; i < n; i++) {
        Hospital h = findHospitalByID(p[i].hospitalID);
        printPatient(&p[i], &h);
    }
}

static void deletePatient(void) {
    Patient p[MAX_RECORDS];
    int n = loadPatients(p);
    if (n == 0) { printf("  No patient records found.\n"); return; }

    int id = readInt("  Enter Patient ID to delete (discharge): ");
    for (int i = 0; i < n; i++) {
        if (p[i].id == id) {
            printf("  Discharging '%s'. Are you sure? (1=Yes / 0=No): ", p[i].name);
            int confirm = readInt("");
            if (confirm != 1) { printf("  Discharge cancelled.\n"); return; }
            for (int j = i; j < n - 1; j++) p[j] = p[j + 1];
            n--;
            if (savePatients(p, n))
                printf("  Patient discharged successfully!\n");
            return;
        }
    }
    printf("  Patient with ID %d not found.\n", id);
}

/* ──────────────── Sorting Functions ──────────────── */

/* Generic swap */
#define SWAP(a, b) do { Hospital _t = (a); (a) = (b); (b) = _t; } while (0)

static void sortByBedPrice(void) {
    Hospital h[MAX_RECORDS];
    int n = loadHospitals(h);
    if (n == 0) { printf("  No records to sort.\n"); return; }

    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (h[i].bedPrice < h[j].bedPrice) SWAP(h[i], h[j]);

    printf("\n  === Sorted by Bed Price (High → Low) ===\n");
    for (int i = 0; i < n; i++)
        printf("  %d. %-35s $%.2f/day\n", i + 1, h[i].name, h[i].bedPrice);
}

static void sortByBeds(void) {
    Hospital h[MAX_RECORDS];
    int n = loadHospitals(h);
    if (n == 0) { printf("  No records to sort.\n"); return; }

    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (h[i].availableBeds < h[j].availableBeds) SWAP(h[i], h[j]);

    printf("\n  === Sorted by Available Beds (High → Low) ===\n");
    for (int i = 0; i < n; i++)
        printf("  %d. %-35s %d beds\n", i + 1, h[i].name, h[i].availableBeds);
}

static void sortByName(void) {
    Hospital h[MAX_RECORDS];
    int n = loadHospitals(h);
    if (n == 0) { printf("  No records to sort.\n"); return; }

    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (strcmp(h[i].name, h[j].name) > 0) SWAP(h[i], h[j]);

    printf("\n  === Sorted by Name (A → Z) ===\n");
    for (int i = 0; i < n; i++)
        printf("  %d. %s\n", i + 1, h[i].name);
}

static void sortByRatingReviews(void) {
    Hospital h[MAX_RECORDS];
    int n = loadHospitals(h);
    if (n == 0) { printf("  No records to sort.\n"); return; }

    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (h[i].rating < h[j].rating ||
               (h[i].rating == h[j].rating && h[i].reviews < h[j].reviews))
                SWAP(h[i], h[j]);

    printf("\n  === Sorted by Rating & Reviews (Best First) ===\n");
    for (int i = 0; i < n; i++)
        printf("  %d. %-35s %.1f/5.0 (%d reviews)\n",
               i + 1, h[i].name, h[i].rating, h[i].reviews);
}

/* ──────────────── Menus ──────────────── */

static void hospitalMenu(void) {
    int ch;
    do {
        printf("\n  ╔══════════════════════════════╗\n");
        printf("  ║     Hospital Management      ║\n");
        printf("  ╠══════════════════════════════╣\n");
        printf("  ║  1. Add Hospital             ║\n");
        printf("  ║  2. Display All Hospitals    ║\n");
        printf("  ║  3. Search by City           ║\n");
        printf("  ║  4. Update Hospital          ║\n");
        printf("  ║  5. Delete Hospital          ║\n");
        printf("  ║  0. Back                     ║\n");
        printf("  ╚══════════════════════════════╝\n");
        ch = readInt("  Choice: ");
        switch (ch) {
            case 1: addHospital();         break;
            case 2: displayHospitals();    break;
            case 3: displayHospitalByCity();break;
            case 4: updateHospital();      break;
            case 5: deleteHospital();      break;
            case 0: break;
            default: printf("  Invalid choice!\n");
        }
    } while (ch != 0);
}

static void patientMenu(void) {
    int ch;
    do {
        printf("\n  ╔══════════════════════════════╗\n");
        printf("  ║      Patient Management      ║\n");
        printf("  ╠══════════════════════════════╣\n");
        printf("  ║  1. Add Patient              ║\n");
        printf("  ║  2. Display All Patients     ║\n");
        printf("  ║  3. Discharge Patient        ║\n");
        printf("  ║  0. Back                     ║\n");
        printf("  ╚══════════════════════════════╝\n");
        ch = readInt("  Choice: ");
        switch (ch) {
            case 1: addPatient();      break;
            case 2: displayPatients(); break;
            case 3: deletePatient();   break;
            case 0: break;
            default: printf("  Invalid choice!\n");
        }
    } while (ch != 0);
}

static void sortingMenu(void) {
    int ch;
    do {
        printf("\n  ╔══════════════════════════════╗\n");
        printf("  ║       Sorting Options        ║\n");
        printf("  ╠══════════════════════════════╣\n");
        printf("  ║  1. Sort by Bed Price        ║\n");
        printf("  ║  2. Sort by Available Beds   ║\n");
        printf("  ║  3. Sort by Name             ║\n");
        printf("  ║  4. Sort by Rating & Reviews ║\n");
        printf("  ║  0. Back                     ║\n");
        printf("  ╚══════════════════════════════╝\n");
        ch = readInt("  Choice: ");
        switch (ch) {
            case 1: sortByBedPrice();       break;
            case 2: sortByBeds();           break;
            case 3: sortByName();           break;
            case 4: sortByRatingReviews();  break;
            case 0: break;
            default: printf("  Invalid choice!\n");
        }
    } while (ch != 0);
}

/* ──────────────── Main ──────────────── */

int main(void) {
    int choice;
    do {
        printf("\n  ╔══════════════════════════════════╗\n");
        printf("  ║   HOSPITAL MANAGEMENT SYSTEM     ║\n");
        printf("  ╠══════════════════════════════════╣\n");
        printf("  ║  1. Hospital Management          ║\n");
        printf("  ║  2. Patient Management           ║\n");
        printf("  ║  3. Sorting Options              ║\n");
        printf("  ║  0. Exit                         ║\n");
        printf("  ╚══════════════════════════════════╝\n");
        choice = readInt("  Choice: ");
        switch (choice) {
            case 1: hospitalMenu();  break;
            case 2: patientMenu();   break;
            case 3: sortingMenu();   break;
            case 0: printf("\n  Goodbye!\n\n"); break;
            default: printf("  Invalid choice!\n");
        }
    } while (choice != 0);

    return 0;
}