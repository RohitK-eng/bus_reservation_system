/*
    Bus Ticket Reservation + Cancellation System
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROUTES 3

// ---------- STRUCTURES ----------

struct Route {
    int routeID;
    char source[30];
    char destination[30];
    int fare;
    int totalSeats;
};

struct Ticket {
    int ticketID;
    char passengerName[50];
    int routeID;
    int seatNumber;
    int fare;
};

// ---------- GLOBAL ROUTES ----------

struct Route routes[MAX_ROUTES] = {
    {1, "Chennai", "Bangalore", 500, 10},
    {2, "Chennai", "Hyderabad", 700, 10},
    {3, "Chennai", "Coimbatore", 400, 10}
};

// ---------- FUNCTION DECLARATIONS ----------

void displayRoutes();
void displayAvailableSeats(int routeID);
int isSeatBooked(int routeID, int seatNo);
void bookTicket();
void cancelTicket();
void searchTicket();
void generateReport();
int getNextTicketID();

// ---------- MAIN ----------

int main() {
    int choice;

    while (1) {
        printf("\n===== BUS RESERVATION SYSTEM =====\n");
        printf("1. Display Routes\n");
        printf("2. View Available Seats\n");
        printf("3. Book Ticket\n");
        printf("4. Cancel Ticket\n");
        printf("5. Search Ticket\n");
        printf("6. Daily Report\n");
        printf("7. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: displayRoutes(); break;
            case 2: {
                int id;
                printf("Enter Route ID: ");
                scanf("%d", &id);
                displayAvailableSeats(id);
                break;
            }
            case 3: bookTicket(); break;
            case 4: cancelTicket(); break;
            case 5: searchTicket(); break;
            case 6: generateReport(); break;
            case 7: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
}

// ---------- FUNCTIONS ----------

// Unique Ticket ID
int getNextTicketID() {
    FILE *fp = fopen("tickets.dat", "rb");
    struct Ticket t;
    int lastID = 1000;

    if (fp == NULL)
        return lastID + 1;

    while (fread(&t, sizeof(t), 1, fp)) {
        if (t.ticketID > lastID)
            lastID = t.ticketID;
    }

    fclose(fp);
    return lastID + 1;
}

// Display routes
void displayRoutes() {
    printf("\nAvailable Routes:\n");
    for (int i = 0; i < MAX_ROUTES; i++) {
        printf("ID: %d | %s -> %s | Fare: %d | Seats: %d\n",
            routes[i].routeID,
            routes[i].source,
            routes[i].destination,
            routes[i].fare,
            routes[i].totalSeats);
    }
}

// Check if seat booked
int isSeatBooked(int routeID, int seatNo) {
    FILE *fp = fopen("tickets.dat", "rb");
    struct Ticket t;

    if (fp == NULL) return 0;

    while (fread(&t, sizeof(t), 1, fp)) {
        if (t.routeID == routeID && t.seatNumber == seatNo) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// Show available seats
void displayAvailableSeats(int routeID) {
    int found = 0, available = 0;

    for (int i = 0; i < MAX_ROUTES; i++) {
        if (routes[i].routeID == routeID) {
            found = 1;
            printf("\nAvailable Seats:\n");

            for (int s = 1; s <= routes[i].totalSeats; s++) {
                if (!isSeatBooked(routeID, s)) {
                    printf("%d ", s);
                    available++;
                }
            }
            printf("\nTotal Available Seats: %d\n", available);
        }
    }

    if (!found)
        printf("Invalid Route ID!\n");
}

// Book ticket
void bookTicket() {
    struct Ticket t;
    int routeID, seatNo, found = 0, index = -1;

    printf("Enter Passenger Name: ");
    scanf(" %[^\n]", t.passengerName);

    printf("Enter Route ID: ");
    scanf("%d", &routeID);

    // Find route
    for (int i = 0; i < MAX_ROUTES; i++) {
        if (routes[i].routeID == routeID) {
            found = 1;
            index = i;
            break;
        }
    }

    if (!found) {
        printf("Invalid Route ID!\n");
        return;
    }

    displayAvailableSeats(routeID);

    int n;
    printf("Enter number of seats: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        printf("Enter Seat Number %d: ", i + 1);
        scanf("%d", &seatNo);

        if (seatNo < 1 || seatNo > routes[index].totalSeats) {
            printf("Invalid seat!\n");
            i--;
            continue;
        }

        if (isSeatBooked(routeID, seatNo)) {
            printf("Seat already booked!\n");
            i--;
            continue;
        }

        t.ticketID = getNextTicketID();
        t.routeID = routeID;
        t.seatNumber = seatNo;
        t.fare = routes[index].fare;

        FILE *fp = fopen("tickets.dat", "ab");
        fwrite(&t, sizeof(t), 1, fp);
        fclose(fp);

        printf("Seat %d booked successfully! Ticket ID: %d\n", seatNo, t.ticketID);
    }
}

// Cancel ticket
void cancelTicket() {
    int id, found = 0;
    struct Ticket t;

    FILE *fp = fopen("tickets.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (fp == NULL) {
        printf("No bookings found!\n");
        return;
    }

    printf("Enter Ticket ID to cancel: ");
    scanf("%d", &id);

    while (fread(&t, sizeof(t), 1, fp)) {
        if (t.ticketID == id) {
            found = 1;
            continue;
        }
        fwrite(&t, sizeof(t), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove("tickets.dat");
    rename("temp.dat", "tickets.dat");

    if (found)
        printf("Ticket Cancelled Successfully!\n");
    else
        printf("Ticket ID not found!\n");
}

// Search ticket
void searchTicket() {
    int id, found = 0;
    struct Ticket t;

    FILE *fp = fopen("tickets.dat", "rb");

    if (fp == NULL) {
        printf("No records found!\n");
        return;
    }

    printf("Enter Ticket ID: ");
    scanf("%d", &id);

    while (fread(&t, sizeof(t), 1, fp)) {
        if (t.ticketID == id) {
            printf("\nTicket Found:\n");
            printf("Name: %s\n", t.passengerName);
            printf("Route ID: %d\n", t.routeID);
            printf("Seat No: %d\n", t.seatNumber);
            printf("Fare: %d\n", t.fare);
            found = 1;
            break;
        }
    }

    fclose(fp);

    if (!found)
        printf("Ticket not found!\n");
}

// Report
void generateReport() {
    struct Ticket t;
    int count[MAX_ROUTES] = {0};
    int revenue[MAX_ROUTES] = {0};

    FILE *fp = fopen("tickets.dat", "rb");

    if (fp == NULL) {
        printf("No data available!\n");
        return;
    }

    while (fread(&t, sizeof(t), 1, fp)) {
        for (int i = 0; i < MAX_ROUTES; i++) {
            if (routes[i].routeID == t.routeID) {
                count[i]++;
                revenue[i] += t.fare;
            }
        }
    }

    fclose(fp);

    printf("\n===== DAILY REPORT =====\n");
    for (int i = 0; i < MAX_ROUTES; i++) {
        printf("Route %d (%s -> %s):\n",
            routes[i].routeID,
            routes[i].source,
            routes[i].destination);
        printf("Tickets Booked: %d\n", count[i]);
        printf("Total Revenue: %d\n\n", revenue[i]);
    }
}
