#include "energia.h"
#include <errno.h>

/*define RAPL Environment*/

#define CPU_SANDYBRIDGE 42
#define CPU_SANDYBRIDGE_EP 45
#define CPU_IVYBRIDGE 58
#define CPU_IVYBRIDGE_EP 62
#define CPU_HASWELL 60 // 70 too?
#define CPU_HASWELL_EP 63
#define CPU_HASWELL_1 69
#define CPU_BROADWELL 61 // 71 too?
#define CPU_BROADWELL_EP 79
#define CPU_BROADWELL_DE 86
#define CPU_SKYLAKE 78
#define CPU_SKYLAKE_1 94
#define NUM_RAPL_DOMAINS 4
#define MAX_CPUS 128 //1024
#define MAX_PACKAGES 4 //16

static int package_map[MAX_PACKAGES];
static int total_packages = 0;
static int total_cores = 0;

static char rapl_domain_names[NUM_RAPL_DOMAINS][30] = { "energy-cores",
                                                        "energy-gpu",
                                                        "energy-pkg",
                                                        "energy-ram" };

char event_names[MAX_PACKAGES][NUM_RAPL_DOMAINS][256];
char filenames[MAX_PACKAGES][NUM_RAPL_DOMAINS][256];
char packname[MAX_PACKAGES][256];
int valid[MAX_PACKAGES][NUM_RAPL_DOMAINS];

long long kernel_before[MAX_PACKAGES][NUM_RAPL_DOMAINS];
long long kernel_after[MAX_PACKAGES][NUM_RAPL_DOMAINS];

void detect_cpu();
void detect_packages();

void rapl_init() {
    detect_cpu();
    detect_packages();
}

/* Function used by the Intel RAPL to detect the CPU Architecture*/
void detect_cpu() {
    int family;
    int model = -1;
    char buffer[BUFSIZ];
    char vendor[BUFSIZ];
    FILE* fff = fopen("/proc/cpuinfo", "r");
    while (1) {
        char* result = fgets(buffer, BUFSIZ, fff);
        if (result == NULL) {
            break;
        }
        if (!strncmp(result, "vendor_id", 8)) {
            sscanf(result, "%*s%*s%s", vendor);
            if (strncmp(vendor, "GenuineIntel", 12) != 0) {
                printf("%s not an Intel chip\n", vendor);
            }
        }
        if (!strncmp(result, "cpu family", 10)) {
            sscanf(result, "%*s%*s%*s%d", &family);
            if (family != 6) {
                fprintf(stderr, "Wrong CPU family %d\n", family);
            }
        }
        if (!strncmp(result, "model", 5)) {
            sscanf(result, "%*s%*s%d", &model);
        }
    }
    fclose(fff);

    printf("Vendor: %s\n", vendor);
    printf("CPU family: %d\n", family);
    printf("Model: %d\n", model);
}

/* Function used by the Intel RAPL to detect the number of cores and CPU sockets*/
void detect_packages() {
    char filename[BUFSIZ];

    for (size_t i = 0; i < MAX_PACKAGES; i++) {
        package_map[i] = -1;
    }

    int i;
    for (i = 0; i < MAX_CPUS; i++) {
        sprintf(filename, "/sys/devices/system/cpu/cpu%d/topology/physical_package_id", i);
        FILE* fff = fopen(filename, "r");
        if (fff == NULL) {
            break;
        }
        int package;
        fscanf(fff, "%d", &package);
        fclose(fff);
        if (package_map[package] == -1) {
            total_packages++;
            package_map[package] = i;
        }
    }

    total_cores = i;

    printf("Total cores: %d\n", total_cores);
    printf("Total packages: %d\n", total_packages);
}

/* Function used by the Intel RAPL to store the actual value of the hardware counter*/
void start_rapl_sysfs() {
    for (int j = 0; j < total_packages; j++) {
        int i = 0;
        char tempfile[256];
        sprintf(packname[j], "/sys/class/powercap/intel-rapl/intel-rapl:%d", j);
        sprintf(tempfile, "%s/name", packname[j]);
        FILE* fff = fopen(tempfile, "r");
        if (fff == NULL) {
            fprintf(stderr, "\tCould not open %s\n", tempfile);
            exit(0);
        }
        fscanf(fff, "%s", event_names[j][i]);
        valid[j][i] = 1;
        fclose(fff);
        sprintf(filenames[j][i], "%s/energy_uj", packname[j]);

        /* Handle subdomains */
        for (i = 1; i < NUM_RAPL_DOMAINS; i++) {
            sprintf(tempfile, "%s/intel-rapl:%d:%d/name", packname[j], j, i - 1);
            fff = fopen(tempfile, "r");
            if (fff == NULL) {
                //fprintf(stderr,"\tCould not open %s\n",tempfile);
                valid[j][i] = 0;
                continue;
            }
            valid[j][i] = 1;
            fscanf(fff, "%s", event_names[j][i]);
            fclose(fff);
            sprintf(filenames[j][i], "%s/intel-rapl:%d:%d/energy_uj", packname[j], j, i - 1);
        }
    }

    /* Gather before values */
    for (int j = 0; j < total_packages; j++) {
        for (int i = 0; i < NUM_RAPL_DOMAINS; i++) {
            if (valid[j][i]) {
                FILE* fff = fopen(filenames[j][i], "r");
                if (fff == NULL) {
                    fprintf(stderr, "Error opening %s\n", filenames[j][i]);
                    fprintf(stderr, "errno: %d\n", errno);
                } else {
                    fscanf(fff, "%lld", &kernel_before[j][i]);
                    fclose(fff);
                }
            }
        }
    }
}

/* Function used by the Intel RAPL to load the value of the hardware counter and returns the energy consumption*/
double end_rapl_sysfs() {
    double total = 0;
    for (int j = 0; j < total_packages; j++) {
        for (int i = 0; i < NUM_RAPL_DOMAINS; i++) {
            if (valid[j][i]) {
                FILE* fff = fopen(filenames[j][i], "r");
                if (fff == NULL) {
                    fprintf(stderr, "\tError opening %s!\n", filenames[j][i]);
                } else {
                    fscanf(fff, "%lld", &kernel_after[j][i]);
                    fclose(fff);
                }
            }
        }
    }
    for (int j = 0; j < total_packages; j++) {
        for (int i = 0; i < NUM_RAPL_DOMAINS; i++) {
            if (valid[j][i]) {
                if (strcmp(event_names[j][i], "core") != 0
                    && strcmp(event_names[j][i], "uncore") != 0)
                {
                    total +=
                        (((double)kernel_after[j][i] - (double)kernel_before[j][i]) / 1000000.0);
                }
            }
        }
    }
    return total;
}
