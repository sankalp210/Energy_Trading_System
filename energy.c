#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TRANSACTIONS 1000
#define MAX_LENGTH 20
#define MAX_SELLERS 10
#define MAX_BUYERS 10

typedef struct {
    int transactionID;
    int buyerID;
    int sellerID;
    double energyAmount;   // in kWh
    double pricePerKWh;    
    char timestamp[20];    // Timestamp in format YYYY-MM-DD HH:MM:SS
} Transaction;

Transaction transactions[MAX_TRANSACTIONS];
int transactionCount = 0;

typedef struct {
    int sellerID;
    int buyerID;
    int transactionCount;
    double totalRevenue;
} SellerBuyerPair;

SellerBuyerPair pairs[MAX_TRANSACTIONS]; // Max pairs possible
int pairCount = 0;


void addTransaction(int buyerID, int sellerID, double energyAmount, double pricePerKWh, FILE *file) {
    if (transactionCount >= MAX_TRANSACTIONS) {
        printf("Transaction list is full!\n");
        return;
    }

    Transaction newTransaction;
    newTransaction.transactionID = transactionCount + 1;
    newTransaction.buyerID = buyerID;
    newTransaction.sellerID = sellerID;
    newTransaction.energyAmount = energyAmount;
    newTransaction.pricePerKWh = pricePerKWh;

    // Get the current time
    time_t now = time(NULL);
    strftime(newTransaction.timestamp, sizeof(newTransaction.timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    transactions[transactionCount] = newTransaction;
    transactionCount++;

    fprintf(file, "%d,%d,%d,%.2f,%.2f,%s\n", 
            newTransaction.transactionID, buyerID, sellerID, 
            energyAmount, pricePerKWh, newTransaction.timestamp);
    fflush(file);  // Ensure the transaction is written to the file immediately

    printf("Transaction added successfully!\n");
}


void displayTransactions() {
    if (transactionCount == 0) {
        printf("No transactions to display.\n");
        return;
    }

    printf("\n%-12s %-12s %-12s %-12s %-12s %s\n", "Trans ID", "Buyer ID", "Seller ID", "Energy (kWh)", "Price/kWh", "Timestamp");
    for (int i = 0; i < transactionCount; i++) {
        printf("%-12d %-12d %-12d %-12.2f %-12.2f %s\n",
               transactions[i].transactionID,
               transactions[i].buyerID,
               transactions[i].sellerID,
               transactions[i].energyAmount,
               transactions[i].pricePerKWh,
               transactions[i].timestamp);
    }
}


void listTransactionsForSeller(int sellerID) {
    printf("Transactions for Seller ID %d:\n", sellerID);
    for (int i = 0; i < transactionCount; i++) {
        if (transactions[i].sellerID == sellerID) {
            printf("%-12d %-12d %-12.2f %-12.2f %s\n", transactions[i].transactionID, transactions[i].buyerID,
                   transactions[i].energyAmount, transactions[i].pricePerKWh, transactions[i].timestamp);
        }
    }
}


void listTransactionsForBuyer(int buyerID) {
    printf("Transactions for Buyer ID %d:\n", buyerID);
    for (int i = 0; i < transactionCount; i++) {
        if (transactions[i].buyerID == buyerID) {
            printf("%-12d %-12d %-12.2f %-12.2f %s\n", transactions[i].transactionID, transactions[i].sellerID,
                   transactions[i].energyAmount, transactions[i].pricePerKWh, transactions[i].timestamp);
        }
    }
}


void listTransactionsInPeriod(const char* startTime, const char* endTime) {
    printf("Transactions from %s to %s:\n", startTime, endTime);
    for (int i = 0; i < transactionCount; i++) {
        if (strcmp(transactions[i].timestamp, startTime) >= 0 && strcmp(transactions[i].timestamp, endTime) <= 0) {
            printf("%-12d %-12d %-12d %-12.2f %-12.2f %s\n",
                   transactions[i].transactionID,
                   transactions[i].buyerID,
                   transactions[i].sellerID,
                   transactions[i].energyAmount,
                   transactions[i].pricePerKWh,
                   transactions[i].timestamp);
        }
    }
}


double calculateRevenueBySeller(int sellerID) {
    double totalRevenue = 0;
    for (int i = 0; i < transactionCount; i++) {
        if (transactions[i].sellerID == sellerID) {
            totalRevenue += transactions[i].energyAmount * transactions[i].pricePerKWh;
        }
    }
    return totalRevenue;
}


void sortSellersByRevenue() {
    double sellerRevenues[MAX_TRANSACTIONS] = {0}; 
    int sellerIDs[MAX_TRANSACTIONS] = {0};       
    
    for (int i = 0; i < transactionCount; i++) {
        sellerRevenues[i] = transactions[i].energyAmount * transactions[i].pricePerKWh;
        sellerIDs[i] = transactions[i].sellerID;
    }

    for (int i = 1; i < transactionCount; i++) {
        int keyID = sellerIDs[i];
        double keyRevenue = sellerRevenues[i];
        int j = i - 1;

        while (j >= 0 && sellerRevenues[j] > keyRevenue) {
            sellerRevenues[j + 1] = sellerRevenues[j];
            sellerIDs[j + 1] = sellerIDs[j];
            j--;
        }
        sellerRevenues[j + 1] = keyRevenue;  
        sellerIDs[j + 1] = keyID;         
    }

    printf("Sellers sorted by revenue:\n");
    for (int i = 0; i < transactionCount; i++) {
        if (sellerRevenues[i] > 0) {
            printf("Seller ID: %d, Revenue: %.2f\n", sellerIDs[i], sellerRevenues[i]);
        }
    }
}



void findTransactionWithHighestEnergy() {
    double maxEnergy = 0;
    int index = -1;
    for (int i = 0; i < transactionCount; i++) {
        if (transactions[i].energyAmount > maxEnergy) {
            maxEnergy = transactions[i].energyAmount;
            index = i;
        }
    }
    if (index != -1) {
        printf("Transaction with highest energy:\n");
        printf("%-12d %-12d %-12d %-12.2f %-12.2f %s\n",
               transactions[index].transactionID,
               transactions[index].buyerID,
               transactions[index].sellerID,
               transactions[index].energyAmount,
               transactions[index].pricePerKWh,
               transactions[index].timestamp);
    }
}


void sortBuyersByEnergy() {
    double buyerEnergy[MAX_BUYERS] = {0}; 

    for (int i = 0; i < transactionCount; i++) {
        buyerEnergy[transactions[i].buyerID] += transactions[i].energyAmount;
    }

    for (int i = 1; i < MAX_BUYERS; i++) {
        if (buyerEnergy[i] == 0) continue;

        double keyEnergy = buyerEnergy[i];
        int keyID = i;
        int j = i - 1;

        while (j >= 0 && buyerEnergy[j] < keyEnergy) {
            buyerEnergy[j + 1] = buyerEnergy[j];
            j--;
        }
        buyerEnergy[j + 1] = keyEnergy;
    }

    printf("Buyers sorted by energy purchased:\n");
    for (int i = 0; i < MAX_BUYERS; i++) {
        if (buyerEnergy[i] > 0) {
            printf("Buyer ID: %d, Energy: %.2f\n", i, buyerEnergy[i]);
        }
    }
}



void findMonthWithMaxTransactions() {
    int transactionCountPerMonth[12] = {0}; // Count of transactions for each month
    char month[3]; // For extracting month from timestamp
    char year[5];  // For extracting year from timestamp

    for (int i = 0; i < transactionCount; i++) {
        sscanf(transactions[i].timestamp, "%4s-%2s", year, month); // Extract year and month
        int monthIndex = atoi(month) - 1; // Convert month string to integer index (0-11)
        if (monthIndex >= 0 && monthIndex < 12) {
            transactionCountPerMonth[monthIndex]++;
        }
    }

    int maxTransactions = 0;
    int maxMonthIndex = 0;

    // Find the month with the maximum transactions
    for (int i = 0; i < 12; i++) {
        if (transactionCountPerMonth[i] > maxTransactions) {
            maxTransactions = transactionCountPerMonth[i];
            maxMonthIndex = i;
        }
    }

    printf("Month with maximum transactions: %d (Transactions: %d)\n", maxMonthIndex + 1, maxTransactions);
}


void findMaxTransactionPair() {
    int maxTransactions = 0;
    SellerBuyerPair maxPair = {0};

    // Iterate through all transactions to count transactions between pairs
    for (int i = 0; i < transactionCount; i++) {
        int found = 0;
        for (int j = 0; j < pairCount; j++) {
            if (pairs[j].sellerID == transactions[i].sellerID && pairs[j].buyerID == transactions[i].buyerID) {
                pairs[j].transactionCount++;
                pairs[j].totalRevenue += transactions[i].energyAmount * transactions[i].pricePerKWh;
                found = 1;
                break;
            }
        }
        // If pair not found, create a new pair
        if (!found) {
            pairs[pairCount].sellerID = transactions[i].sellerID;
            pairs[pairCount].buyerID = transactions[i].buyerID;
            pairs[pairCount].transactionCount = 1;
            pairs[pairCount].totalRevenue = transactions[i].energyAmount * transactions[i].pricePerKWh;
            pairCount++;
        }
    }

    // Find the pair with the maximum transactions
    for (int i = 0; i < pairCount; i++) {
        if (pairs[i].transactionCount > maxTransactions) {
            maxTransactions = pairs[i].transactionCount;
            maxPair = pairs[i];
        }
    }

    printf("Pair with maximum transactions: Seller ID: %d, Buyer ID: %d, Transactions: %d\n",
           maxPair.sellerID, maxPair.buyerID, maxPair.transactionCount);
}


void sortPairsByTransactionCount() {
    for (int i = 1; i < pairCount; i++) {
        SellerBuyerPair key = pairs[i];
        int j = i - 1;

        // Move elements of pairs[0..i-1] that are less than key.transactionCount
        while (j >= 0 && pairs[j].transactionCount < key.transactionCount) {
            pairs[j + 1] = pairs[j];
            j--;
        }
        pairs[j + 1] = key; // Insert the key at the correct position
    }

    printf("Seller/Buyer pairs sorted by number of transactions:\n");
    for (int i = 0; i < pairCount; i++) {
        printf("Seller ID: %d, Buyer ID: %d, Transactions: %d\n",
               pairs[i].sellerID, pairs[i].buyerID, pairs[i].transactionCount);
    }
}



void sortPairsByTotalRevenue() {
    for (int i = 1; i < pairCount; i++) {
        SellerBuyerPair key = pairs[i];
        int j = i - 1;

        // Move elements of pairs[0..i-1] that are less than key.totalRevenue
        while (j >= 0 && pairs[j].totalRevenue > key.totalRevenue) {
            pairs[j + 1] = pairs[j];
            j--;
        }
        pairs[j + 1] = key; // Insert the key at the correct position
    }

    printf("Seller/Buyer pairs sorted by total revenue:\n");
    for (int i = 0; i < pairCount; i++) {
        printf("Seller ID: %d, Buyer ID: %d, Total Revenue: %.2f\n",
               pairs[i].sellerID, pairs[i].buyerID, pairs[i].totalRevenue);
    }
}


void loadTransactionsFromFile(FILE *file) {
    char line[256];  

    while (fgets(line, sizeof(line), file)) {
        
        if (sscanf(line, "%d,%d,%d,%lf,%lf,%19[^\n]",
                   &transactions[transactionCount].transactionID,
                   &transactions[transactionCount].buyerID,
                   &transactions[transactionCount].sellerID,
                   &transactions[transactionCount].energyAmount,
                   &transactions[transactionCount].pricePerKWh,
                   transactions[transactionCount].timestamp) == 6) {
            transactionCount++;
        } else {
            printf("Error parsing line: %s", line);
        }
    }

    printf("Loaded %d transactions from file.\n", transactionCount);
}

int main() {

    FILE *file = fopen("transactions.txt", "a+"); 
    if (!file) {
        printf("Error opening file!\n");
        return 1;
    }

    loadTransactionsFromFile(file);

    // Menu for adding and displaying transactions
    int choice, buyerID, sellerID;
    double energyAmount, pricePerKWh;
    char startTime[MAX_LENGTH], endTime[MAX_LENGTH];

    do {
        printf("\nEnergy Trading Record Management System\n");
        printf("1. Add Transaction\n");
        printf("2. Display All Transactions\n");
        printf("3. List Transactions for Seller\n");
        printf("4. List Transactions for Buyer\n");
        printf("5. List Transactions in Time Period\n");
        printf("6. Calculate Revenue by Seller\n");
        printf("7. Sort Sellers by Revenue\n");
        printf("8. Find Transaction with Highest Energy\n");
        printf("9. Sort Buyers by Energy Purchased\n");
        printf("10. Month with Maximum Transactions\n");
        printf("11. Seller/Buyer pair involved in Maximum Number of Transactions\n");
        printf("12. Sort Seller/Buyer pairs by Number of Vransactions\n");
        printf("13. Sort Seller/Buyer pairs by Total Tevenue Exchanged\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter Buyer ID: ");
                scanf("%d", &buyerID);
                printf("Enter Seller ID: ");
                scanf("%d", &sellerID);
                printf("Enter Energy Amount (kWh): ");
                scanf("%lf", &energyAmount);
                printf("Enter Price per kWh: ");
                scanf("%lf", &pricePerKWh);
                addTransaction(buyerID, sellerID, energyAmount, pricePerKWh, file);
                break;
            case 2:
                displayTransactions();
                break;
            case 3:
                printf("Enter Seller ID: ");
                scanf("%d", &sellerID);
                listTransactionsForSeller(sellerID);
                break;
            case 4:
                printf("Enter Buyer ID: ");
                scanf("%d", &buyerID);
                listTransactionsForBuyer(buyerID);
                break;
            case 5:
                // printf("Enter start time (YYYY-MM-DD HH:MM:SS): ");
                // // fflush(stdout);
                // fgets(startTime, sizeof(startTime), stdin);
                // startTime[strcspn(startTime, "\n")] = '\0';
                // printf("Enter end time (YYYY-MM-DD HH:MM:SS): ");
                // // fflush(stdout);
                // fgets(endTime, sizeof(endTime), stdin);
                // endTime[strcspn(endTime, "\n")] = '\0';
                // listTransactionsInPeriod(startTime, endTime);
                // break;
                printf("Enter start time (YYYY-MM-DD HH:MM:SS): ");
                scanf("%s", startTime);
                printf("Enter end time (YYYY-MM-DD HH:MM:SS): ");
                scanf("%s", endTime);
                listTransactionsInPeriod(startTime, endTime);
                break;
            case 6:
                printf("Enter Seller ID: ");
                scanf("%d", &sellerID);
                printf("Total Revenue by Seller %d: %.2f\n", sellerID, calculateRevenueBySeller(sellerID));
                break;
            case 7:
                sortSellersByRevenue();
                break;
            case 8:
                findTransactionWithHighestEnergy();
                break;
            case 9:
                sortBuyersByEnergy();
                break;
            case 10:
                findMonthWithMaxTransactions();    
                break;
            case 11:
                findMaxTransactionPair();
                break;
            case 12:
                sortPairsByTransactionCount();
                break;
            case 13:
                sortPairsByTotalRevenue();      
                break;  
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 0);

    fclose(file); 
    return 0;
}
