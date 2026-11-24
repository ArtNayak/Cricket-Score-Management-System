/* Cricket Score Management System
 * Record batting and bowling figures for two teams, update them later,
 * and print scorecards with the match result. Plain C99, no dependencies. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PLAYERS 11
#define NAME_LEN    25
#define MAX_OVERS   100

typedef struct {
    char name[NAME_LEN];
    int runs, balls, fours, sixes;
} Batsman;

typedef struct {
    char name[NAME_LEN];
    int balls_bowled, runs_given, wickets;   /* overs kept as balls: 3.4 overs = 22 */
} Bowler;

typedef struct {
    Batsman bat[MAX_PLAYERS];
    Bowler  bowl[MAX_PLAYERS];
    int nbat, nbowl, extras;
} Team;

/* ---------- input helpers: every read is validated ---------- */

static void readLine(char *buf, size_t size) {
    if (!fgets(buf, (int)size, stdin)) {
        printf("\nInput closed, exiting.\n");
        exit(1);
    }
    char *nl = strchr(buf, '\n');
    if (nl) {
        *nl = '\0';
    } else {
        int c;                                /* line was longer than buf: drop the rest */
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
}

static int readInt(const char *prompt, int min, int max) {
    char buf[64], *end;
    for (;;) {
        printf("%s", prompt);
        readLine(buf, sizeof buf);
        long v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0' && v >= min && v <= max) return (int)v;
        printf("  Enter a whole number from %d to %d.\n", min, max);
    }
}

static void readName(const char *prompt, char *out) {
    for (;;) {
        printf("%s", prompt);
        readLine(out, NAME_LEN);
        if (out[0]) return;
        printf("  Name cannot be empty.\n");
    }
}

/* Overs in cricket notation (3.4 = 3 overs 4 balls); returns total balls. */
static int readOvers(const char *prompt) {
    char buf[32], *end;
    for (;;) {
        printf("%s", prompt);
        readLine(buf, sizeof buf);
        long o = strtol(buf, &end, 10);
        int b = 0, ok = end != buf && o >= 0 && o <= MAX_OVERS;
        if (ok && *end == '.')
            ok = end[1] >= '0' && end[1] <= '5' && end[2] == '\0' && (b = end[1] - '0', 1);
        else if (ok)
            ok = *end == '\0';
        if (ok) return (int)o * 6 + b;
        printf("  Use overs.balls, e.g. 4 or 3.4 (balls 0-5, up to %d overs).\n", MAX_OVERS);
    }
}

/* ---------- entry ---------- */

static void inputBatsman(Batsman *b, int n) {
    char p[64];
    snprintf(p, sizeof p, "Batsman %d name: ", n);
    readName(p, b->name);
    for (;;) {
        b->runs  = readInt("  Runs scored: ", 0, 1000);
        b->balls = readInt("  Balls faced: ", 0, 1000);
        b->fours = readInt("  Fours: ", 0, 250);
        b->sixes = readInt("  Sixes: ", 0, 166);
        if (b->fours * 4 + b->sixes * 6 > b->runs)
            printf("  Fours and sixes add up to more than the runs scored, re-enter.\n");
        else if (b->fours + b->sixes > b->balls)
            printf("  More boundaries than balls faced, re-enter.\n");
        else
            return;
    }
}

static void inputBowler(Bowler *b, int n) {
    char p[64];
    snprintf(p, sizeof p, "Bowler %d name: ", n);
    readName(p, b->name);
    b->balls_bowled = readOvers("  Overs bowled (e.g. 3.4): ");
    b->runs_given   = readInt("  Runs given: ", 0, 2000);
    b->wickets      = readInt("  Wickets taken: ", 0, 10);
}

static void inputTeam(Team *t, int num) {
    printf("\n=== Team %d ===\n", num);
    t->nbat  = readInt("Number of batsmen (1-11): ", 1, MAX_PLAYERS);
    t->nbowl = readInt("Number of bowlers (1-11): ", 1, MAX_PLAYERS);
    for (int i = 0; i < t->nbat; i++)  inputBatsman(&t->bat[i], i + 1);
    for (int i = 0; i < t->nbowl; i++) inputBowler(&t->bowl[i], i + 1);
    t->extras = readInt("Total extra runs: ", 0, 1000);
}

/* ---------- derived figures and display ---------- */

static int teamTotal(const Team *t) {
    int total = t->extras;
    for (int i = 0; i < t->nbat; i++) total += t->bat[i].runs;
    return total;
}

static void displayTeam(const Team *t, int num) {
    puts("\n==================================================================");
    printf("Team %d\n", num);
    puts("==================================================================");
    printf("%-24s %5s %6s %4s %4s %8s\n", "Batsman", "Runs", "Balls", "4s", "6s", "SR");
    for (int i = 0; i < t->nbat; i++) {
        const Batsman *b = &t->bat[i];
        double sr = b->balls ? b->runs * 100.0 / b->balls : 0.0;
        printf("%-24s %5d %6d %4d %4d %8.2f\n", b->name, b->runs, b->balls, b->fours, b->sixes, sr);
    }
    printf("\n%-24s %6s %5s %4s %8s\n", "Bowler", "Overs", "Runs", "Wkts", "Econ");
    for (int i = 0; i < t->nbowl; i++) {
        const Bowler *b = &t->bowl[i];
        double econ = b->balls_bowled ? b->runs_given * 6.0 / b->balls_bowled : 0.0;
        printf("%-24s %3d.%-2d %5d %4d %8.2f\n", b->name, b->balls_bowled / 6,
               b->balls_bowled % 6, b->runs_given, b->wickets, econ);
    }
    printf("\nExtras: %d   Total: %d\n", t->extras, teamTotal(t));
}

static void displayResult(const Team *t) {
    int a = teamTotal(&t[0]), b = teamTotal(&t[1]);
    puts("\n------------------------------------------------------------------");
    printf("Team 1: %d   Team 2: %d\n", a, b);
    if (a == b) puts("Match tied.");
    else printf("Team %d leads by %d runs.\n", a > b ? 1 : 2, abs(a - b));
}

/* ---------- update ---------- */

static int sameName(const char *x, const char *y) {
    for (; *x && *y; x++, y++)
        if (tolower((unsigned char)*x) != tolower((unsigned char)*y)) return 0;
    return *x == *y;
}

static void updatePlayer(Team *t, int batting) {
    char name[NAME_LEN];
    readName("Name of the player to update: ", name);
    int n = batting ? t->nbat : t->nbowl;
    for (int i = 0; i < n; i++) {
        if (batting && sameName(t->bat[i].name, name)) { inputBatsman(&t->bat[i], i + 1); return; }
        if (!batting && sameName(t->bowl[i].name, name)) { inputBowler(&t->bowl[i], i + 1); return; }
    }
    printf("Player \"%s\" not found.\n", name);
}

int main(void) {
    Team teams[2] = {0};
    inputTeam(&teams[0], 1);
    inputTeam(&teams[1], 2);

    for (;;) {
        printf("\n1) Show scorecards\n2) Update a batsman\n3) Update a bowler\n"
               "4) Update extras\n0) Exit\n");
        int choice = readInt("Choose: ", 0, 4);
        if (choice == 0) break;
        if (choice == 1) {
            displayTeam(&teams[0], 1);
            displayTeam(&teams[1], 2);
            displayResult(teams);
            continue;
        }
        Team *t = &teams[readInt("Which team (1 or 2): ", 1, 2) - 1];
        if (choice == 2) updatePlayer(t, 1);
        else if (choice == 3) updatePlayer(t, 0);
        else t->extras = readInt("Total extra runs: ", 0, 1000);
    }
    return 0;
}
