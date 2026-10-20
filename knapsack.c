#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define CLEAR_CMD "cls"
void sleep_ms(int ms) { Sleep(ms); }
#else
#include <unistd.h>
#define CLEAR_CMD "clear"
void sleep_ms(int ms) { usleep(ms * 1000); }
#endif

#define STEP_DELAY_MS 60

void trim(char *str) {
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || str[len - 1] == ' ' || str[len - 1] == '\t')) {
        str[len - 1] = '\0';
        len--;
    }
}

void print_horizontal_border(int row_label_width, int col_width, int W) {
    printf("+");
    for (int i = 0; i < row_label_width; i++) printf("-");
    printf("+");
    for (int w = 0; w <= W; w++) {
        for (int j = 0; j < col_width; j++) printf("-");
        printf("+");
    }
    printf("\n");
}

void print_centered(const char *text, int width) {
    int len = (int)strlen(text);
    if (len >= width) {
        printf("%.*s", width, text);
        return;
    }
    int left_pad = (width - len) / 2;
    int right_pad = width - len - left_pad;
    for (int i = 0; i < left_pad; i++) printf(" ");
    printf("%s", text);
    for (int i = 0; i < right_pad; i++) printf(" ");
}

void render_ascii_table(int **dp, int *weights, int *values, int n, int W,
                        int active_i, int active_w, const char *test_header,
                        const char *status_msg) {
    system(CLEAR_CMD);

    int col_width = 6;
    int row_label_width = 18;
    char buf[64];

    printf("=== Live 0/1 Knapsack DP Table Visualization: %s ===\n", test_header);
    printf("Items (n): %d | Capacity (W): %d\n\n", n, W);

    print_horizontal_border(row_label_width, col_width, W);
    printf("|");
    print_centered("Item (wt, val)", row_label_width);
    printf("|");
    for (int w = 0; w <= W; w++) {
        snprintf(buf, sizeof(buf), "w=%d", w);
        print_centered(buf, col_width);
        printf("|");
    }
    printf("\n");
    print_horizontal_border(row_label_width, col_width, W);

    for (int i = 0; i <= n; i++) {
        if (i == 0) {
            snprintf(buf, sizeof(buf), "i=0 (Empty)");
        } else {
            snprintf(buf, sizeof(buf), "i=%d (w:%d, v:%d)", i, weights[i - 1], values[i - 1]);
        }
        printf("|%-*s|", row_label_width, buf);

        for (int w = 0; w <= W; w++) {
            if (dp[i][w] == -1) {
                snprintf(buf, sizeof(buf), ".");
            } else if (i == active_i && w == active_w) {
                snprintf(buf, sizeof(buf), "[%d]", dp[i][w]);
            } else {
                snprintf(buf, sizeof(buf), "%d", dp[i][w]);
            }
            print_centered(buf, col_width);
            printf("|");
        }
        printf("\n");
        print_horizontal_border(row_label_width, col_width, W);
    }

    printf("\nStep Detail: %s\n", status_msg);
    fflush(stdout);
    sleep_ms(STEP_DELAY_MS);
}

int solve_knapsack_live(const char *test_header, int n, int W, int *weights, int *values) {
    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = (int *)malloc((W + 1) * sizeof(int));
        for (int w = 0; w <= W; w++) {
            dp[i][w] = -1;
        }
    }

    char status_msg[256];

    for (int w = 0; w <= W; w++) {
        dp[0][w] = 0;
    }
    render_ascii_table(dp, weights, values, n, W, 0, W, test_header,
                       "Initialized base row (0 items -> value 0 for all capacities).");

    for (int i = 1; i <= n; i++) {
        int wt = weights[i - 1];
        int val = values[i - 1];

        for (int w = 0; w <= W; w++) {
            if (wt <= w) {
                int exclude = dp[i - 1][w];
                int include = val + dp[i - 1][w - wt];
                dp[i][w] = (include > exclude) ? include : exclude;

                snprintf(status_msg, sizeof(status_msg),
                         "Item %d (wt=%d, val=%d) fits in w=%d: max(exclude=%d, include=%d+%d) = %d",
                         i, wt, val, w, exclude, val, dp[i - 1][w - wt], dp[i][w]);
            } else {
                dp[i][w] = dp[i - 1][w];
                snprintf(status_msg, sizeof(status_msg),
                         "Item %d (wt=%d > w=%d) cannot fit: carry down dp[%d][%d] = %d",
                         i, wt, w, i - 1, w, dp[i][w]);
            }
            render_ascii_table(dp, weights, values, n, W, i, w, test_header, status_msg);
        }
    }

    int max_val = dp[n][W];

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);

    return max_val;
}

int main(void) {
    FILE *in = fopen("input.txt", "r");
    if (!in) {
        fprintf(stderr, "Error: Could not open 'input.txt'.\n");
        return 1;
    }

    FILE *out = fopen("result.txt", "w");
    if (!out) {
        fprintf(stderr, "Error: Could not open 'result.txt' for writing.\n");
        fclose(in);
        return 1;
    }

    char line[256];
    char test_header[128] = "";
    int n = 0, W = 0;
    int *weights = NULL;
    int *values = NULL;
    int current_item = 0;
    int parsing_items = 0;

    while (fgets(line, sizeof(line), in)) {
        trim(line);
        if (strlen(line) == 0) continue;

        if (strncmp(line, "#Test", 5) == 0) {
            strcpy(test_header, line);
            n = 0;
            W = 0;
            parsing_items = 0;
            current_item = 0;
        } else if (n == 0 && W == 0) {
            sscanf(line, "%d %d", &n, &W);
            weights = (int *)malloc(n * sizeof(int));
            values = (int *)malloc(n * sizeof(int));
            current_item = 0;
            parsing_items = 1;
        } else if (parsing_items && current_item < n) {
            sscanf(line, "%d %d", &weights[current_item], &values[current_item]);
            current_item++;
        }

        if (parsing_items && current_item == n) {
            int max_val = solve_knapsack_live(test_header, n, W, weights, values);
            fprintf(out, "%s (%d %d)  %d\n", test_header, n, W, max_val);
            fflush(out); /* Immediately save to result.txt */

            printf("\nCompleted %s -> Max Value: %d\n", test_header, max_val);
            sleep_ms(1000);

            free(weights);
            weights = NULL;
            values = NULL;
            n = 0;
            W = 0;
            parsing_items = 0;
            current_item = 0;
        }
    }

    fclose(in);
    fclose(out);

    printf("\n==================================================\n");
    printf("SUCCESS: All test cases computed & saved to result.txt\n");
    printf("==================================================\n");
    return 0;
}