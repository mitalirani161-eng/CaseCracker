#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define V 8
#define SUSPECTS 3
#define CLUES 3

/*
 * CaseCracker - Interactive Mystery Investigation System
 * Educational C/DSA prototype using an undirected graph, adjacency matrix,
 * BFS (queue), DFS (recursion), linear search, and clue-based case logic.
 * All case information is fictional and predefined.
 */

enum { ALEX = 0, MAYA = 1, LEO = 2, KEYCARD = 3,
       SHOEPRINT = 4, CCTV = 5, GALLERY = 6, STAFF_ROOM = 7 };

const char *nodes[V] = {
    "Alex (Suspect)", "Maya (Suspect)", "Leo (Suspect)",
    "Keycard Log (Clue)", "Muddy Shoeprint (Clue)",
    "CCTV Access Record (Clue)", "Diamond Gallery (Location)",
    "Staff Room (Location)"
};

const char *suspects[SUSPECTS] = {"Alex", "Maya", "Leo"};
const char *alibis[SUSPECTS] = {
    "I was at the front desk.",
    "I was in the staff room.",
    "I was outside the museum."
};

int graph[V][V] = {0};
int clue_found[CLUES] = {0};

const char *clue_names[CLUES] = {
    "Keycard access log",
    "Muddy shoeprint",
    "CCTV access record"
};

const char *clue_details[CLUES] = {
    "A keycard assigned to Maya was used at the Diamond Gallery at 8:00 PM.",
    "A muddy shoeprint was found by the gallery entrance. It has NOT been matched to a suspect.",
    "The CCTV blind spot was enabled using credentials assigned to Maya. Credentials can be shared or misused."
};

/* Each clue is linked to the entities supported by the fictional case file. */
void addEdge(int a, int b) {
    graph[a][b] = 1;
    graph[b][a] = 1;
}

void buildGraph(void) {
    addEdge(MAYA, KEYCARD);
    addEdge(MAYA, CCTV);
    addEdge(MAYA, STAFF_ROOM);
    addEdge(ALEX, SHOEPRINT);
    addEdge(ALEX, GALLERY);
    addEdge(LEO, SHOEPRINT);
    addEdge(LEO, STAFF_ROOM);
    addEdge(KEYCARD, GALLERY);
    addEdge(SHOEPRINT, GALLERY);
    addEdge(CCTV, GALLERY);
    addEdge(CCTV, STAFF_ROOM);
}

void clearLine(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

int readNumber(const char *prompt, int low, int high, int *value) {
    char line[100], extra;
    int n;
    printf("%s", prompt);
    if (!fgets(line, sizeof line, stdin)) return 0;
    if (sscanf(line, " %d %c", &n, &extra) != 1 || n < low || n > high) {
        printf("Please enter a valid number from %d to %d.\n", low, high);
        return -1;
    }
    *value = n;
    return 1;
}

int sameName(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

void printDivider(void) {
    puts("------------------------------------------------------------");
}

void showCase(void) {
    puts("\nCASE FILE: THE MUSEUM DIAMOND THEFT");
    printDivider();
    puts("At 8:00 PM, a diamond disappeared from the museum gallery.");
    puts("Three staff members had possible access. Investigate before accusing anyone.");
    puts("\nSuspects and their statements:");
    for (int i = 0; i < SUSPECTS; i++)
        printf("%d. %s: \"%s\"\n", i + 1, suspects[i], alibis[i]);
    puts("\nEvidence must be inspected before it is counted as discovered.");
}

void showNodes(void) {
    puts("\nInvestigation graph nodes:");
    for (int i = 0; i < V; i++)
        printf("%d. %s\n", i, nodes[i]);
    puts("Edges show relationships between suspects, clues, and locations.");
}

void showConnections(void) {
    puts("\nKnown direct connections:");
    for (int i = 0; i < V; i++) {
        int found = 0;
        printf("%-29s -> ", nodes[i]);
        for (int j = 0; j < V; j++) {
            if (graph[i][j]) {
                printf("%s; ", nodes[j]);
                found = 1;
            }
        }
        if (!found) printf("none");
        puts("");
    }
}

void showMatrix(void) {
    puts("\nAdjacency matrix (1 = connected, 0 = not directly connected):");
    printf("     ");
    for (int j = 0; j < V; j++) printf("%d ", j);
    puts("");
    for (int i = 0; i < V; i++) {
        printf("%2d | ", i);
        for (int j = 0; j < V; j++) printf("%d ", graph[i][j]);
        puts("");
    }
}

void inspectClue(void) {
    int c;
    puts("\nInspect evidence:");
    for (int i = 0; i < CLUES; i++)
        printf("%d. %s %s\n", i + 1, clue_names[i],
               clue_found[i] ? "[INSPECTED]" : "[NOT INSPECTED]");
    int result = readNumber("Choose a clue (1-3): ", 1, CLUES, &c);
    if (result != 1) return;
    c--;
    clue_found[c] = 1;
    printf("\nEVIDENCE REPORT: %s\n%s\n", clue_names[c], clue_details[c]);
    if (c == 0)
        puts("Interpretation: this links Maya's assigned keycard to the gallery, but does not prove who carried it.");
    else if (c == 1)
        puts("Interpretation: this clue is inconclusive because there is no verified shoeprint match.");
    else
        puts("Interpretation: this links Maya's assigned credentials to the CCTV event, but credentials may be misused.");
}

void showInvestigationStatus(void) {
    int count = 0;
    puts("\nInvestigation status:");
    for (int i = 0; i < CLUES; i++) {
        printf("- %s: %s\n", clue_names[i], clue_found[i] ? "inspected" : "not yet inspected");
        count += clue_found[i];
    }
    printf("Evidence inspected: %d/%d\n", count, CLUES);
}

void bfs(int start) {
    int visited[V] = {0};
    int queue[V], front = 0, rear = 0;
    visited[start] = 1;
    queue[rear++] = start;

    printf("\nBFS from %s (breadth-first / level by level):\n", nodes[start]);
    while (front < rear) {
        int current = queue[front++];
        printf(" -> %s\n", nodes[current]);
        for (int i = 0; i < V; i++) {
            if (graph[current][i] && !visited[i]) {
                visited[i] = 1;
                queue[rear++] = i;
            }
        }
    }
   puts("BFS visits connected nodes level by level.");
}

void dfsVisit(int current, int visited[]) {
    visited[current] = 1;
    printf(" -> %s\n", nodes[current]);
    for (int i = 0; i < V; i++) {
        if (graph[current][i] && !visited[i])
            dfsVisit(i, visited);
    }
}

void dfs(int start) {
    int visited[V] = {0};
    printf("\nDFS from %s (depth-first):\n", nodes[start]);
    dfsVisit(start, visited);
    puts("DFS explores one path before moving to another.");
}

void searchSuspect(void) {
    char query[50];
    printf("\nSearch suspect (Alex, Maya, Leo): ");
    if (!fgets(query, sizeof query, stdin)) return;
    query[strcspn(query, "\n")] = '\0';

    for (int i = 0; i < SUSPECTS; i++) {
        if (sameName(query, suspects[i])) {
            printf("\nSuspect: %s\nStatement: %s\nDirect links:\n", suspects[i], alibis[i]);
            for (int j = 0; j < V; j++)
                if (graph[i][j]) printf("- %s\n", nodes[j]);
            return;
        }
    }
    puts("Suspect not found. Please enter Alex, Maya, or Leo.");
}

/* Fictional solution logic: clue discovery is required before a conclusion.
   Keycard + CCTV are relevant to Maya, but the report explicitly calls her a lead,
   not a proven culprit. The ending discloses the intended fictional answer. */
void makeAccusation(void) {
    int choice;
    int inspected = clue_found[0] + clue_found[1] + clue_found[2];

    puts("\nFINAL THEORY");
    puts("1. Alex");
    puts("2. Maya");
    puts("3. Leo");
    int result = readNumber("Who is your strongest lead? ", 1, 3, &choice);
    if (result != 1) return;

    if (inspected < 2) {
        printf("\nYou have inspected only %d clue(s). Investigate at least two clues before deciding.\n", inspected);
        puts("Your theory is recorded as premature; inspect evidence and try again.");
        return;
    }

    if (choice == 2) {
        puts("\nCASE SOLVED!");
        puts("Reasoning:");
        puts("- The keycard record links Maya's assigned keycard to the gallery at 8:00 PM.");
        puts("- The CCTV record links Maya's assigned credentials to the camera blind spot.");
        puts("- The shoeprint is inconclusive and does not reliably identify Alex or Leo.");
        puts("\nMaya was the culprit! Great detective work!");
        
    } else {
        puts("\nThat choice does not match the intended solution in this fictional case.");
        puts("Review the keycard and CCTV records, and note that the shoeprint is inconclusive.");
        puts("You can inspect clues again and submit another theory.");
    }
}

int main(void) {
    int choice;
    buildGraph();

    puts("============================================================");
    puts("                         CASECRACKER");
    puts("              Interactive Mystery Investigation");
    puts("============================================================");
    puts("Goal: inspect clues, explore graph connections, then form a theory.");

    do {
        puts("\nMAIN MENU");
        puts("1. Read case file and suspect statements");
        puts("2. View graph nodes");
        puts("3. View direct graph connections");
        puts("4. Explore connections with BFS");
        puts("5. Explore connections with DFS");
        puts("6. Search for a suspect");
        puts("7. Inspect a clue");
        puts("8. View investigation progress");
        puts("9. Make a final theory");
        puts("10. View adjacency matrix");
        puts("0. Exit");

        int result = readNumber("Enter choice: ", 0, 10, &choice);
        if (result == 0) break;
        if (result == -1) continue;

        int start;
        switch (choice) {
            case 1: showCase(); break;
            case 2: showNodes(); break;
            case 3: showConnections(); break;
            case 4:
            case 5:
                showNodes();
                result = readNumber("Starting node (0-7): ", 0, V - 1, &start);
                if (result != 1) break;
                if (choice == 4) bfs(start);
                else dfs(start);
                break;
            case 6: searchSuspect(); break;
            case 7: inspectClue(); break;
            case 8: showInvestigationStatus(); break;
            case 9: makeAccusation(); break;
            case 10: showMatrix(); break;
            case 0: puts("Exiting CaseCracker. Goodbye."); break;
        }
    } while (choice != 0);

    return 0;
}
